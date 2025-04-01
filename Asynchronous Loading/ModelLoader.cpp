#include "ModelLoader.h"
#include <CommonUtilities/Vector.h>
#include <cassert>
#include <vector>

#include <Engine/AssetManagement/AM_Mesh.h>
#include "Engine/Debug/Debug_Log.h"
#include <Engine/Utils_Misc/Utility.h>
#include "Engine/Utils_Misc/WorkingThreadManager.h"

#include <TGAFBXImporter/TGAFbx.h>
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

ModelLoader* ModelLoader::Instance = nullptr;

void ModelLoader::CreateManager()
{
	if (Instance) return;

	Instance = new ModelLoader();

	Instance->Init();
}

void ModelLoader::DestroyManager()
{

	if (Instance == nullptr) return;

	Instance->myIsRunning = false;
	Instance->myWorkerThread.wait();

	TGA::FBX::Importer::UninitImporter();

	delete Instance;
	Instance = nullptr;

}


//
//Model ModelLoader::GetModel(const std::string& aPath)
//{
//	AssetPointer<Mesh> mesh;
//	if (AM_Mesh::GetInstance().TryGet(aPath, mesh) == false)
//	{
//		return CreateModel(aPath);
//	}
//
//	Model model;
//	model.myMesh = mesh;
//	return model;
//}

//int ModelLoader::CreateMesh(const std::string& aPath, bool aClearVertexData)
//{
//	Mesh mesh;
//	CreateMesh(aPath, mesh, aClearVertexData);
//	return mesh;
//}

int ModelLoader::CreateMesh(const std::string& aPath, bool aClearVertexData)
{

	Instance->myLock.lock();

	int id = Instance->myIDCount++;
	Task_LoadModel& task	= Instance->myTasks[id];
	task.Path				= aPath;
	task.Type				= ModelType::Mesh;
	task.Extra				= aClearVertexData;

	Instance->myLock.unlock();


	return id;
}

int ModelLoader::CreateAnimation(const std::string& aPath)
{

	Instance->myLock.lock();

	int id = Instance->myIDCount++;
	Task_LoadModel& task	= Instance->myTasks[id];
	task.Path				= aPath;
	task.Type				= ModelType::Animation;

	Instance->myLock.unlock();


	return id;
}

ModelLoader::LoadResult ModelLoader::CheckTask(int aID)
{
	return Instance->myTasks.at(aID).LoadResult;
}

ModelLoader::LoadResult ModelLoader::GetMesh(int aID, Mesh** aMeshPtr)
{
	return GetObjectData(aID, reinterpret_cast<void**>(aMeshPtr));
}

ModelLoader::LoadResult ModelLoader::GetAnimation(int aID, TGA::FBX::Animation** aAnimationPtr)
{
	return GetObjectData(aID, reinterpret_cast<void**>(aAnimationPtr));
}

ModelLoader::LoadResult ModelLoader::CreateMesh(const std::string& aPath, Mesh& outMesh, bool aClearVertexData)
{
	
	auto loadMeshResult = TGA::FBX::Importer::LoadMesh(aPath, outMesh.mesh);
	if (loadMeshResult == false) // TODO- IMPROVE ERROR HANDLING HERE PLS!
	{
		if (loadMeshResult.Result == TGA::FBX::FbxImportResult::FileNotFound)
		{
			return LoadResult::NotFound;
		}

		//LOG_ERROR("COULD NOT FIND MESH IN MESHLOADER.CPP"); - error fucking crashes here for some reason man wtf
		return LoadResult::Failed;	//Could not find
	}

	/*for (auto& bone : outMesh.mesh.Skeleton.Bones)
	{
		bone.BindPoseInverse = bone.BindPoseInverse.GetTranspose();
	}*/


	auto& elements = outMesh.mesh.Elements;

	if (elements[0].Vertices.empty())						return LoadResult::Failed; //Animation Missing

	AM_Mesh& am_Mesh = AM_Mesh::GetInstance();
	auto& meshDataList = outMesh.meshData;
	meshDataList.resize(elements.size());

	for (size_t i = 0; i < elements.size(); i++)
	{
		auto& element = elements[i];

		meshDataList[i] = am_Mesh.CreateMeshData(
			element.MeshName + std::to_string(i),
			element.Vertices.data(),
			static_cast<int>(element.Vertices.size()),
			element.Indices.data(),
			static_cast<int>(element.Indices.size()));
	}

	if (aClearVertexData)
	{
		for (auto& element : elements)
		{
			element.Indices.clear();
			element.Vertices.clear();
		}
	}


	return LoadResult::Success;
}

//Model ModelLoader::CreateModel(const std::string& aPath, const bool aIsPermanent)
//{
//	aIsPermanent;
//
//
//	//Assimp::Importer imp;
//
//	if (CommonUtilities::HasExtension(aPath, ".fbx"))
//	{
//		//return LoadFBX(aPath, aPath, imp, aIsPermanent);
//	}
//	if (CommonUtilities::HasExtension(aPath, ".obj"))
//	{
//		LOG_ERROR("OBJ files isn't implemented");
//		//return LoadOBJ(aPath, aPath, imp, aIsPermanent);
//	}
//
//	LOG_ERROR("Unknown File Type ERROR!");
//	return {}; // TODO- load ERROR MODEL here!
//}

bool ModelLoader::Init()
{
	TGA::FBX::Importer::InitImporter();

	//Create thread
	myIsRunning = true;
	Instance->myWorkerThread = WorkingThreadManager::Enqueue(
		[this]()
		{
			WorkingThreadFunction();
		}
	);

	return true;
}

inline bool ModelLoader::GetVertices(std::vector<Vertex>& aOutVertex, aiMesh* aMesh)
{
	if (!aMesh->HasPositions())
	{
		LOG_ERROR("Mesh has no Position");
		return false;
	}
	aOutVertex.resize(aMesh->mNumVertices);

	const aiVector3D* vertx = aMesh->mVertices;
	aiVector3D** uv = aMesh->mTextureCoords;
	for (size_t i = 0; i < aMesh->mNumVertices; i++)
	{
		aOutVertex[i].Position.x = vertx[i].x;
		aOutVertex[i].Position.y = vertx[i].y;
		aOutVertex[i].Position.z = vertx[i].z;
		aOutVertex[i].Position.w = 1.0f;


		if (aMesh->HasTextureCoords(0))
		{
			aOutVertex[i].uv.x = uv[0][i].x;

			//Flip V in UV since it's wrong  
			aOutVertex[i].uv.y = uv[0][i].y;
		}
		else
		{
			aOutVertex[i].uv.x = 0.0f;
			aOutVertex[i].uv.y = 1.0f;
			LOG_WARNING("Mesh has no UV: ", std::string(aMesh->mName.C_Str()));
		}

	}

	if (!GetCalcNormals(aOutVertex, aMesh))
	{
		LOG_ERROR("Could not create Normals: ", std::string(aMesh->mName.C_Str()));
		return false;
	}

	return true;
}

inline bool ModelLoader::GetCalcNormals(std::vector<Vertex>& aOutVertex, const aiMesh* aMesh)
{
	//Normal
	if (aMesh->HasNormals())
	{
		const aiVector3D* normal = aMesh->mNormals;
		for (size_t i = 0; i < aMesh->mNumVertices; i++)
		{
			aOutVertex[i].Normal.x = normal[i].x;
			aOutVertex[i].Normal.y = normal[i].y;
			aOutVertex[i].Normal.z = normal[i].z;
		}
	}
	else
	{
		LOG_MESSAGE("Calculating normals...");
		const size_t size = aMesh->mNumFaces;
		const auto face = aMesh->mFaces;
		for (size_t i = 0; i < size; i++)
		{
			const unsigned int i0 = face[i].mIndices[0];
			const unsigned int i1 = face[i].mIndices[1];
			const unsigned int i2 = face[i].mIndices[2];

			auto& v0 = aOutVertex[i0].Position;
			auto& v1 = aOutVertex[i1].Position;
			auto& v2 = aOutVertex[i2].Position;

			CU::Vector3f edge1 = v1 - v0;
			CU::Vector3f edge2 = v2 - v0;

			const CU::Vector3f normal = edge1.Cross(edge2).GetNormalized();

			aOutVertex[i0].Normal = normal;
			aOutVertex[i1].Normal = normal;
			aOutVertex[i2].Normal = normal;
		}
	}

	if (aMesh->mBitangents && aMesh->mTangents)
	{
		aiVector3D* biTangent = aMesh->mBitangents;
		const aiVector3D* tangent = aMesh->mTangents;
		for (size_t i = 0; i < aMesh->mNumVertices; i++)
		{
			aOutVertex[i].BiNormal.x = biTangent[i].x;
			aOutVertex[i].BiNormal.y = biTangent[i].y;
			aOutVertex[i].BiNormal.z = biTangent[i].z;

			aOutVertex[i].Tangent.x = tangent[i].x;
			aOutVertex[i].Tangent.y = tangent[i].y;
			aOutVertex[i].Tangent.z = tangent[i].z;
		}
	}
	else
	{
		LOG_MESSAGE("Calculating BiNormals and Tangents...");
		const size_t size = aMesh->mNumFaces;
		const auto face = aMesh->mFaces;
		for (size_t i = 0; i < size; i++)
		{
			const unsigned int i0 = face[i].mIndices[0];
			const unsigned int i1 = face[i].mIndices[1];
			const unsigned int i2 = face[i].mIndices[2];

			auto& v0 = aOutVertex[i0].Position;
			auto& v1 = aOutVertex[i1].Position;
			auto& v2 = aOutVertex[i2].Position;


			CU::Vector3f uv0 = { aOutVertex[i0].uv.x, aOutVertex[i0].uv.y };
			CU::Vector3f uv1 = { aOutVertex[i1].uv.x, aOutVertex[i1].uv.y };
			CU::Vector3f uv2 = { aOutVertex[i2].uv.x, aOutVertex[i2].uv.y };

			const CU::Vector3f edge1 = v1 - v0;
			const CU::Vector3f edge2 = v2 - v0;

			const CU::Vector2i deltaUV1 = uv1 - uv0;
			const CU::Vector2i deltaUV2 = uv2 - uv0;

			const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			CU::Vector3f tangent;
			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent.Normalize();

			CU::Vector3f biTangent;
			biTangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			biTangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			biTangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			biTangent.Normalize();

			aOutVertex[i0].Tangent = tangent;
			aOutVertex[i1].Tangent = tangent;
			aOutVertex[i2].Tangent = tangent;

			aOutVertex[i0].BiNormal = biTangent;
			aOutVertex[i1].BiNormal = biTangent;
			aOutVertex[i2].BiNormal = biTangent;
		}
	}

	return true;
}

inline bool ModelLoader::GetIndices(std::vector<unsigned int>& aOutIndices, const aiMesh* aMesh)
{
	aOutIndices.reserve(static_cast<std::vector<unsigned int>::size_type>(aMesh->mNumFaces) * 3);
	for (size_t i = 0; i < aMesh->mNumFaces; i++)
	{
		const auto& face = aMesh->mFaces[i];
		TITAN_ASSERT(face.mNumIndices == 3);
		aOutIndices.push_back(face.mIndices[0]);
		aOutIndices.push_back(face.mIndices[1]);
		aOutIndices.push_back(face.mIndices[2]);
	}

	return true;
}

inline Mesh ModelLoader::CreateMesh(const std::string& aName, const aiScene* aAIScene)
{
	if (aAIScene == nullptr) return {};

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Mesh meshAsset;
	meshAsset.meshData.reserve(aAIScene->mNumMeshes);
	//meshAsset.Name = aName;

	for (size_t i = 0; i < aAIScene->mNumMeshes; i++)
	{

		aiMesh* mesh = aAIScene->mMeshes[i];
		std::string name = aName;
		name += "_";
		name += mesh->mName.C_Str();
		name += '_';
		name += std::to_string(i);

		//To be implemented into Mesh
		auto aabb = mesh->mAABB;
		if (!GetVertices(vertices, mesh))
		{
			LOG_ERROR("Could not load vertex on: ", name.c_str());
			return {};
		}
		if (!GetIndices(indices, mesh))
		{
			LOG_ERROR("Could not load indices on: ", name.c_str());
			return {};
		}

		meshAsset.meshData.push_back(
			AM_Mesh::CreateMeshData(
				name, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size())
			)
		);

		vertices.clear();
		indices.clear();
	}

	return meshAsset;
}

Model ModelLoader::LoadFBX(const std::string& aOriginalPath, const std::string& aPathWithModelAtFront, Assimp::Importer& aAssimp, const bool aIsPermanent)
{
	//Ensure that the aiProcess_FlipUVs post-processing flag is used if your texture coordinates 
	// need to be flipped (often required for certain file formats like OBJ)
	//Assimp supports up to 8 sets of texture coordinates, which are accessed using indices [0] to [7] for mTextureCoords.
	//LOG_MESSAGE("Loading model at path: " + aPathWithModelAtFront);
	const auto asModel = aAssimp.ReadFile(aPathWithModelAtFront,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_MakeLeftHanded |
		aiProcess_FlipWindingOrder
	);

	Model model;
	//Check if model found
	if (asModel == nullptr)
	{
		LOG_ERROR("Failed to load model at: ", aPathWithModelAtFront.c_str());
		return model;
	}

	model.mesh = AM_Mesh::GetInstance().Set(aOriginalPath, CreateMesh(aOriginalPath, asModel), aIsPermanent);
	//Used when creating mesh, and stuff
	return model;
}

// TODO- FIX FUNCTION. CURRENTLY BROKEN PROBABLY MAYBE.
Model ModelLoader::LoadOBJ(const std::string& aOriginalPath, const std::string& aPathWithModelAtFront, Assimp::Importer& aAssimp, const bool aIsPermanent)
{
	//Ensure that the aiProcess_FlipUVs post-processing flag is used if your texture coordinates 
	// need to be flipped (often required for certain file formats like OBJ)
	//Assimp supports up to 8 sets of texture coordinates, which are accessed using indices [0] to [7] for mTextureCoords.
	LOG_MESSAGE("Loading model at path: ", aPathWithModelAtFront.c_str());
	const auto asModel = aAssimp.ReadFile(aPathWithModelAtFront,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	LOG_WARNING("THE FLAGS FOR LOADING OBJ FILES MIGHT BE WRONG. THIS IS UNTESTED.");

	Model model;
	//Check if model found
	if (asModel == nullptr)
	{
		LOG_ERROR("Failed to load model at: ", aPathWithModelAtFront.c_str());
		return model;
	}

	model.mesh = AM_Mesh::GetInstance().Set(aOriginalPath, CreateMesh(aOriginalPath, asModel), aIsPermanent);
	//Used when creating mesh, and stuff
	return model;
}

void ModelLoader::WorkingThreadFunction()
{
	while (myIsRunning)
	{

		myLock.lock();

		if (myIDCurrentCount == myIDCount)
		{
			myLock.unlock();
			continue;
		}

		Task_LoadModel& task = myTasks.at(myIDCurrentCount);
		task.LoadResult = LoadResult::InProgress;

		myLock.unlock();
		LoadResult success = LoadResult::Success;

		switch (task.Type)
		{
		case ModelType::Mesh:
		{
			success = WT_CreateMesh(task.Path, reinterpret_cast<Mesh**>(&task.Resource), task.Extra);
			break;
		}

		case ModelType::Animation:
		{
			success = WT_CreateAnimation(task.Path, reinterpret_cast<TGA::FBX::Animation**>(&task.Resource));
			break;
		}

		case ModelType::Navmesh:
		{
			throw;
			//break;
		}
		}


		myLock.lock();

		task.LoadResult = success;

		myLock.unlock();

		myIDCurrentCount++;
	}
}

ModelLoader::LoadResult ModelLoader::WT_CreateMesh(const std::string& aPath, Mesh** aResource, bool aClearVertexData)
{
	Mesh* mesh = new Mesh();
	LoadResult result = CreateMesh(aPath, *mesh, aClearVertexData);

	if (result == LoadResult::Failed || result == LoadResult::NotFound) // TODO- IMPROVE ERROR HANDLING HERE PLS!
	{
		delete mesh;
		mesh = nullptr;
	}

	*aResource = mesh;

	return result;
}

ModelLoader::LoadResult ModelLoader::WT_CreateAnimation(const std::string& aPath, TGA::FBX::Animation** aResource)
{
	LoadResult result = LoadResult::Success;
	TGA::FBX::Animation* animation = new TGA::FBX::Animation();

	TGA::FBX::FbxImportStatus loadMeshResult = TGA::FBX::Importer::LoadAnimation(aPath, *animation);
	
	if (loadMeshResult == false) // TODO- IMPROVE ERROR HANDLING HERE PLS!
	{

		if (0 < loadMeshResult.Warnings.size())
		{
			delete animation;
			animation = nullptr;
			result = loadMeshResult.Result == TGA::FBX::FbxImportResult::FileNotFound ? LoadResult::NotFound : LoadResult::Failed;
		}
		else
		{
			LOG_ERROR("Animation failed. Warnings were 0 and the error was bypassed. The asset were: ", aPath.c_str());
			result = LoadResult::Success;
		}

	}

	*aResource = animation;

	return result;
}

ModelLoader::LoadResult ModelLoader::GetObjectData(int aID, void** aMeshPtr)
{
	Instance->myLock.lock();
	auto find = Instance->myTasks.find(aID);

	if (find == Instance->myTasks.end())
	{
		Instance->myLock.unlock();
		return LoadResult::NotFound;
	}

	LoadResult result = find->second.LoadResult;

	if (result == LoadResult::Waiting	||
		result == LoadResult::InProgress)
	{
		Instance->myLock.unlock();
		return result;
	}

	if (result == LoadResult::Success)
	{
		*aMeshPtr = find->second.Resource;
	}

	Instance->myTasks.erase(find);
	Instance->myLock.unlock();

	return result;
}

#pragma once
#include <string>
#include <vector>
#include <GraphicsEngine/Mesh_Model/Model.h>
#include <GraphicsEngine/Mesh_Model/Vertex.h>
#include "Assimp/Importer.hpp"
#include <future>
#include "Engine\Utils_Misc\SpinLock.h"

struct aiScene;
struct aiMesh;

namespace TGA { namespace FBX { struct Mesh; } }
namespace TGA { namespace FBX { struct Animation; } }

class ModelLoader
{
public:
	enum LoadResult
	{
		NotFound,
		Success,
		Failed,
		InProgress,
		Waiting
	};

private:
	enum class ModelType
	{
		Mesh,
		Animation,
		Navmesh
	};

	struct Task_LoadModel
	{
		int			Extra			= 0;
		ModelType	Type		= ModelType::Mesh;
		void*		Resource	= nullptr;


		std::string	Path		= "";
		LoadResult	LoadResult	= LoadResult::Waiting;
	};

	static ModelLoader* Instance;

	std::future<void> myWorkerThread;
	std::unordered_map<int, Task_LoadModel> myTasks	= {};
	int myIDCount			= 0;
	int myIDCurrentCount	= 0;
	bool myIsRunning = false;
	SpinLock myLock;

public:
	static void CreateManager();
	static void DestroyManager();


	//static Model GetModel(const std::string& aPath);
	//static Model CreateModel(const std::string& aPath, bool aIsPermanent = false);
	

	//Only FBX files
	//Returns a ID of Task
	static int CreateMesh(const std::string& aPath, bool aClearVertexData = true);

	static int CreateAnimation(const std::string& aPath);
	//static bool CreateNavMesh(const std::string& aPath, NavMesh& outNavMesh, bool aShouldTriangulate = false);

	static LoadResult CheckTask(int aID);

	static LoadResult GetMesh(int aID, Mesh** aMeshPtr);
	static LoadResult GetAnimation(int aID, TGA::FBX::Animation** aAnimationPtr);


	ModelLoader(const ModelLoader&) = delete;
private:
	//Worker thread handling works
	//Work Queues
	//Push request from Asset managers
	//
	
	ModelLoader()	= default;
	~ModelLoader()	= default;

	bool Init();

	static inline bool GetVertices(std::vector<Vertex>& aOutVertex, aiMesh* aMesh);
	static inline bool GetCalcNormals(std::vector<Vertex>& aOutVertex, const aiMesh* aMesh);
	static inline bool GetIndices(std::vector<unsigned int>& aOutIndices, const aiMesh* aMesh);
	static inline Mesh CreateMesh(const std::string& aName, const aiScene* aAIScene);


	//Only FBX files
	static LoadResult CreateMesh(const std::string& aPath, Mesh& outMesh, bool aClearVertexData = true);

	// loaders per file type
	// fbx
	static Model LoadFBX(const std::string& aOriginalPath, const std::string& aPathWithModelAtFront, Assimp::Importer& aAssimp, bool aIsPermanent);
	// obj, currently broken.
	static Model LoadOBJ(const std::string& aOriginalPath, const std::string& aPathWithModelAtFront, Assimp::Importer& aAssimp, bool aIsPermanent);

	//static Model LoadErrorModel(); // todo- load source engine error model here.
	void WorkingThreadFunction();
	LoadResult WT_CreateMesh(const std::string& aPath, Mesh** aResource, bool aClearVertexData);
	LoadResult WT_CreateAnimation(const std::string& aPath, TGA::FBX::Animation** aResource);

	static LoadResult GetObjectData(int aID, void** aMeshPtr);
};


#include "RenderBatchManager.h"
#include <algorithm>
#include "Engine\ComponentSystem\Components\RendererModel.h"
#include "Engine\ComponentSystem\Components\Transform.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Buffers/GPUBufferManager.h"

RenderBatchManager* RenderBatchManager::Instance = nullptr;

#pragma region Templates

///Require less operator
///Return Index
template<typename T>
int AddToList(std::vector<T>& outList, const T& someData)
{
	return InsertIntoList(outList, outList.begin(), outList.end(), someData);
}

///Require less operator
///Return Index
template<typename T>
int InsertIntoList(std::vector<T>& outList, typename std::vector<T>::iterator aBegin, typename std::vector<T>::iterator aEnd, const T& someData)
{
	auto find = std::lower_bound(aBegin, aEnd, someData);

	if (find == outList.end() ||
		(someData < *find || *find < someData))	//!=
	{
		find = outList.insert(find, someData);
	}

	return static_cast<int>(std::distance(outList.begin(), find));
}

///Require less operator
///Return Index
template<typename T, typename Operator>
int AddToList(std::vector<T>& outList, const T& someData, Operator aOperator)
{
	return InsertIntoList<T, Operator>(outList, outList.begin(), outList.end(), someData, aOperator);
}

///Require less operator
///Return Index
template<typename T, typename Operator>
int InsertIntoList(std::vector<T>& outList, typename std::vector<T>::iterator aBegin, typename std::vector<T>::iterator aEnd, const T& someData, Operator aOperator)
{
	auto find = std::lower_bound(aBegin, aEnd, someData, aOperator);

	if (find == outList.end() ||
		(aOperator(someData, *find) || aOperator(*find, someData))) //!=
	{
		find = outList.insert(find, someData);
	}

	return static_cast<int>(std::distance(outList.begin(), find));
}


///Require less operator
///Return Index
template<typename T>
int EraseFromList(std::vector<T>& outList, const T& someData)
{
	return EraseFromList(outList, outList.begin(), outList.end(), someData);
}

///Require less operator
///Return Index
template<typename T>
int EraseFromList(std::vector<T>& outList, typename std::vector<T>::iterator aBegin, typename std::vector<T>::iterator aEnd, const T& someData)
{
	auto find = std::lower_bound(aBegin, aEnd, someData);

	if (find != outList.end() && !(someData < *find || *find < someData))
	{
		find = outList.erase(find, someData);
	}

	return static_cast<int>(std::distance(outList.begin(), find));
}

///Require less operator
///Return Index
template<typename T, typename Lesser>
int EraseFromList(std::vector<T>& outList, const T& someData, Lesser aOperator)
{
	return EraseFromList<T, Lesser>(outList, outList.begin(), outList.end(), someData, aOperator);
}

///Require less operator
///Return Index
template<typename T, typename Lesser>
int EraseFromList(std::vector<T>& outList, typename std::vector<T>::iterator aBegin, typename std::vector<T>::iterator aEnd, const T& someData, Lesser aOperator)
{
	auto find = std::lower_bound(aBegin, aEnd, someData, aOperator);

	if (find != outList.end() &&
		!(aOperator(someData, *find) || aOperator(*find, someData))) //Equal
	{
		find = outList.erase(find);
	}

	return static_cast<int>(std::distance(outList.begin(), find));
}
#pragma endregion


//bool RenderBatchManager::Init()
//{
//    return true;
//}
//
//bool RenderBatchManager::Clear()
//{
//    return true;
//}
//
//void RenderBatchManager::BeginOfFrame()
//{
//}
//
//void RenderBatchManager::EndOfFrame()
//{
//}

void RenderBatchManager::Update_RendererModel()
{
	std::vector<Animator*> animators = {};

	size_t size = 0;

	//Right now Update all
	for (size_t i = 0; i < myComponents_RendererModel.size(); ++i)
	{
		myInstanceData_RendererModel[i].transform = myComponents_RendererModel[i].transform->GetWorldMatrix();

		if (myComponents_RendererModel[i].animator)
		{
			myInstanceData_RendererModel[i].animationIndex = (int)size++;
			animators.push_back(myComponents_RendererModel[i].animator.Get());
		}
	}

	//Animations
	myBonesList.resize(size);
	//ResetAnimationListIndex();

	if (myBonesList.empty()) return;

	//Update Animation List
	for (size_t i = 0; i < size; ++i)
	{
		animators[i]->UpdateAnimator(0, TGA::FBX::Matrix(), (TGA::FBX::Matrix*)myBonesList[i].bones);
	}

	GPUBufferManager* gpuManager = GraphicsEngine::ourGPUBufferManager;
	
	if (gpuManager->GetBufferCount(DXBuffers::AnimationBones) <= size)
	{
		//Release previous SRV
		if (myInstanceSRV != nullptr)
		{
			myInstanceSRV->Release();
			myInstanceSRV = nullptr;
		}

		size += GraphicsEngineSettings::defaultInstanceSize;

		//Create new Buffer
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(CU::Matrix4x4f);

		gpuManager->InitBuffer(DXBuffers::AnimationBones, bufferDesc, sizeof(CU::Matrix4x4f), size * MAX_BONES_ALLOWED);

		//Create SRV
		auto& buffer = gpuManager->GetBuffer(DXBuffers::AnimationBones);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = (UINT)buffer.count;

		

		HRESULT result = DX11::Device->CreateShaderResourceView(buffer.buffer.Get(), &srvDesc, &myInstanceSRV);
		if (FAILED(result))
		{
			MessageBox(nullptr, L"Failed to create SRV", L"Error", MB_ICONERROR);
		}


	}

	auto& buffer = gpuManager->GetBuffer(DXBuffers::AnimationBones);

	//gpuManager->SetBuffer(DXBuffers::AnimationBones, myBonesList.data(), sizeof(CU::Matrix4x4f[MAX_BONES_ALLOWED]) * size, "Bind Animations");
	//Separate Update
	DX11::UpdateBuffer(buffer.buffer.Get(), myBonesList.data(), sizeof(AnimationMatrix) * myBonesList.size());
	DX11::Context->VSSetShaderResources((int)TextureSlot::AnimatonBones, 1, &myInstanceSRV);








	//TODO
	// 
	//Create CBuffer for animator offset Index 
	//Update buffer Index and Bones on GPU
	//Fix Shader

	//Create Function to SetOffset

	//int amount = std::min((int)myBonesList.size(), MAX_ANIMATED_INSTANCES);
	//UpdateAnimations(amount);

	//GraphicsEngine::ourGPUBufferManager->SetBuffer(DXBuffers::Animation, myBonesList.data(), sizeof(CU::Matrix4x4f[MAX_BONES_ALLOWED]) * myBoneBindIndex, "Bind Animations");
}

void RenderBatchManager::UpdateTransforms_RendererModel()
{
	//Right now Update all
	for (size_t i = 0; i < myComponents_RendererModel.size(); ++i)
	{
		myInstanceData_RendererModel[i].transform = myComponents_RendererModel[i].transform->GetWorldMatrix();
	}
}

void RenderBatchManager::UpdateAnimations(int aComponentOffset, int aStartIndex, int someAmount)
{
	aComponentOffset;
	aStartIndex;
	someAmount;
	//Animator** animatorList = new Animator* [someAmount];

	////Get Animators
	//for (size_t i = 0; i < someAmount; ++i)
	//{
	//	animatorList[i] = myComponents_RendererModel.at(aComponentOffset + i).animator.Get();
	//}

	////Update buffer with each animator
	//for (size_t i = 0; i < someAmount; ++i)
	//{
	//	animatorList[i]->UpdateAnimator(0, TGA::FBX::Matrix(), (TGA::FBX::Matrix*)myAnimatorJointsList[aStartIndex + i]);
	//}

	////Update buffer on GPU side
	//GraphicsEngine::ourGPUBufferManager->SetBuffer(DXBuffers::Animation, myAnimatorJointsList, sizeof(myAnimatorJointsList), "Pelle");

	//delete animatorList;
}

int RenderBatchManager::UpdateAnimations(int aAmount)
{
	//Return remaining
	
	int remainingOnBuffer = myBoneBindIndex - myBoneListIndex;

	if (0 < remainingOnBuffer)
	{
		myBoneListIndex += remainingOnBuffer;
		return remainingOnBuffer;
	}

	int bindAmount = std::min(aAmount, MAX_ANIMATED_INSTANCES);

	myBoneBindIndex += bindAmount;

	assert(myBoneBindIndex <= myBonesList.size());

	//GraphicsEngine::ourGPUBufferManager->SetBuffer(DXBuffers::Animation, myBonesList.data() + myBoneListIndex, sizeof(CU::Matrix4x4f[MAX_BONES_ALLOWED]) * bindAmount, "Bind Animations");
	
	return bindAmount;
}

void RenderBatchManager::ResetAnimationListIndex()
{
	myBoneListIndex = 0;
	myBoneBindIndex = 0;
}

unsigned int RenderBatchManager::Add(const GamePointer<RendererModel>& aRenderModel)
{
	int sharedDataIndex				= 0;
	unsigned int sharedDataBatchID	= 0;
	RendererModel* component = aRenderModel.Get();
	ComponentContainer container = { aRenderModel, aRenderModel->GetComponent<Animator>(), aRenderModel->GetTransform() };
	

	//Add to sharedData
	{
		SharedData_RendererModel data;
		data.isForward = component->IsRenderedForward();
		data.isAnimated = container.animator;


		data.mesh	= component->GetMesh();
		data.coolVS = component->GetShader().vs;
		data.coolps = component->GetShader().ps;

		std::copy_n(component->GetTexturesData(),
			MAX_DIFFERENT_TEXTURES * MAX_MESHES_PER_MODEL,
			&data.textures[0][0]);

		int size = static_cast<int>(mySharedData_RendererModel.size());
		sharedDataIndex = AddToList(mySharedData_RendererModel, data);

		if (size < mySharedData_RendererModel.size())	//New SharedBatch
		{
			mySharedData_RendererModel.at(sharedDataIndex).id = ++myBatchIDCount;
		}

		sharedDataBatchID = mySharedData_RendererModel.at(sharedDataIndex).id;
		++mySharedData_RendererModel.at(sharedDataIndex).amount;
	}

	//Add to componentList
	int startIndex = 0;
	for (size_t i = 0; i < sharedDataIndex; ++i)
	{
		startIndex += mySharedData_RendererModel.at(i).amount;
	}

	int endIndex = mySharedData_RendererModel.at(sharedDataIndex).amount - 1;

	auto begin = myComponents_RendererModel.begin();
	std::advance(begin, startIndex);

	auto end = begin;
	std::advance(end, endIndex);

	InsertIntoList(myComponents_RendererModel, begin, end, container,
		[&](const ComponentContainer& aLhs, const ComponentContainer& aRhs)
		{
			int lhsIndex = aLhs.component.GetKeyIndex();
			int rhsIndex = aRhs.component.GetKeyIndex();

			return lhsIndex < rhsIndex;
		});

	myInstanceData_RendererModel.push_back({});
	return sharedDataBatchID;
}

void RenderBatchManager::Erase(unsigned int aBatchID, const GamePointer<RendererModel>& aRenderModel)
{
	int sharedDataIndex = 0;
	//int sharedDataAmount;
	//RendererModel* component = aRenderModel.Get();

	SharedData_RendererModel* sharedData = nullptr;
	for (size_t i = 0; i < mySharedData_RendererModel.size(); ++i)
	{
		if (mySharedData_RendererModel[i].id == aBatchID)
		{
			sharedDataIndex	= static_cast<int>(i);
			sharedData		= &mySharedData_RendererModel[i];
			break;
		}
		
	}

	if (sharedData == nullptr) throw;

	//ComponentContainer container = { aRenderModel, aRenderModel->GetComponent<Animator>(), aRenderModel->GetTransform() };

	////Erase to sharedData
	//{
	//	SharedData_RendererModel data;
	//	data.isForward = component->IsRenderedForward();
	//	data.isAnimated = container.animator;

	//	ModelInstance& modelInstance = component->GetModel();
	//	Model& model = modelInstance.myModel;

	//	data.mesh = model.mesh;
	//	data.coolVS = model.shader.vs;
	//	data.coolps = model.shader.ps;

	//	std::copy_n(&modelInstance.myTextures[0][0],
	//		MAX_DIFFERENT_TEXTURES * MAX_MESHES_PER_MODEL,
	//		&data.textures[0][0]);


	//	auto find = std::lower_bound(mySharedData_RendererModel.begin(), mySharedData_RendererModel.end(), data);

	//	bool meshCheck	= data.mesh.GetID() == find->mesh.GetID();		meshCheck	;
	//	bool VsCheck	= data.coolVS.GetID() == find->coolVS.GetID();	VsCheck		 ;
	//	bool PsCheck	= data.coolps.GetID() == find->coolps.GetID();	PsCheck		 ;
	//																				 
	//	bool textureCheck = true;										textureCheck ;
	//	for (int i = 0; i < 4; ++i)
	//	{
	//		for (int x = 0; x < 4; ++x)
	//		{
	//			if (data.textures[i][x].GetID() != find->textures[i][x].GetID())
	//			{
	//				textureCheck = false;
	//				break;
	//			}
	//		}
	//		if (!textureCheck) break;
	//	}
	//	if (find == mySharedData_RendererModel.end() || *find < data || data < *find)
	//	{
	//		throw;
	//	}
	//	sharedDataIndex = static_cast<int>(std::distance(mySharedData_RendererModel.begin(), find));

	//	sharedDataAmount = --find->amount;
	//	if (find->amount == 0)
	//	{
	//		mySharedData_RendererModel.erase(find);
	//		//--sharedDataIndex;
	//	}
	//}

	//Erase from componentList
	int startIndex = 0;
	for (size_t i = 0; i < sharedDataIndex; ++i)
	{
		startIndex += mySharedData_RendererModel.at(i).amount;
	}

	int endIndex = sharedData->amount;

	auto begin = myComponents_RendererModel.begin();
	std::advance(begin, startIndex);

	auto end = begin;
	std::advance(end, endIndex);

	EraseFromList(myComponents_RendererModel, begin, end, { aRenderModel },
		[&](const ComponentContainer& aLhs, const ComponentContainer& aRhs)
		{

			int lhsIndex = aLhs.component.GetKeyIndex();
			int rhsIndex = aRhs.component.GetKeyIndex();

			return lhsIndex < rhsIndex;
		});

	myInstanceData_RendererModel.pop_back();

	//Remove from shared data
	--sharedData->amount;
	if (sharedData->amount < 1)
	{
		mySharedData_RendererModel.erase(mySharedData_RendererModel.begin() + sharedDataIndex);
	}
}

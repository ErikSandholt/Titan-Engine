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

	//Separate Update
	DX11::UpdateBuffer(buffer.buffer.Get(), myBonesList.data(), sizeof(AnimationMatrix) * myBonesList.size());
	DX11::Context->VSSetShaderResources((int)TextureSlot::AnimatonBones, 1, &myInstanceSRV);
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

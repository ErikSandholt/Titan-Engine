#pragma once
//#include "Engine\Utils_Misc\IManager.h"
#include <vector>
#include <CommonUtilities/Matrix4x4.hpp>
#include "Engine/ComponentSystem/GamePointer.hpp"
#include "Engine/AssetManagement/AssetPointer.hpp"
#include "Engine/ComponentSystem/Components/RendererModel.h"
#include "Engine/ComponentSystem/Components/Animator.h"
// DOESN'T DRAW, ONLY MANAGES!

//class RendererModel;
class Transform;
struct Mesh;
struct VertexShaderAndInputLayout;
struct ID3D11PixelShader;

struct SharedData_RendererModel
{
	//Model model;
	AssetPointer<Mesh*>							mesh;
	AssetPointer<ID3D11PixelShader*>			coolps; // todo- please rename these and fix the linker errors that occur when they're not called this
	AssetPointer<VertexShaderAndInputLayout>	coolVS; // todo- maybe replace with model struct from model.h?

	AssetPointer<ShaderResource>				textures[MAX_MESHES_PER_MODEL][MAX_DIFFERENT_TEXTURES] = {};
	
	bool isForward	= false;
	bool isAnimated = false;

	int amount		= 0;
	unsigned int id	= 0;

	// this is what decides what comes first in the list, just change the mouths or other params etc.
	bool operator <(const SharedData_RendererModel& aRhs) const
	{
		if (isForward		!=		aRhs.isForward)			return isForward	< aRhs.isForward;	//False First
		if (mesh.GetID()	!=		aRhs.mesh.GetID())		return mesh.GetID()	< aRhs.mesh.GetID();
		if (coolps.GetID()		!=		aRhs.coolps.GetID())		return coolps.GetID()	< aRhs.coolps.GetID();
		if (coolVS.GetID()		!=		aRhs.coolVS.GetID())		return coolVS.GetID()	< aRhs.coolVS.GetID();

		for (size_t m = 0; m < MAX_MESHES_PER_MODEL; ++m)
		{
			for (size_t t = 0; t < MAX_DIFFERENT_TEXTURES; ++t)
			{
				if (textures[m][t].GetID() != aRhs.textures[m][t].GetID())	return textures[m][t].GetID() < aRhs.textures[m][t].GetID();
			}
		}

		if (isAnimated		!=		aRhs.isAnimated)			return isAnimated < aRhs.isAnimated;	//False First

		return false;
	}
};

namespace Titan
{
	class Engine;
}

class RenderBatchManager //: public IManager
{
public:
	struct ComponentContainer
	{
		GamePointer<RendererModel>	component;
		GamePointer<Animator>		animator;
		GamePointer<Transform>		transform;
	};

	struct InstanceData
	{
		CU::Matrix4x4f transform	= {};
		int animationIndex			= -1;

		int trash0;
		int trash1;
		int trash2;
	};

	struct AnimationMatrix
	{
		CU::Matrix4x4f bones[MAX_BONES_ALLOWED];
	};
private:

	static RenderBatchManager* Instance;

	std::vector<SharedData_RendererModel>			mySharedData_RendererModel	= {};
	std::vector<ComponentContainer>					myComponents_RendererModel	= {};
	std::vector<InstanceData>						myInstanceData_RendererModel= {};
	//CU::Matrix4x4f									myAnimatorJointsList[MAX_ANIMATED_INSTANCES][MAX_BONES_ALLOWED] = {};

	std::vector<AnimationMatrix>					myBonesList					= {};
	ID3D11ShaderResourceView*						myInstanceSRV				= nullptr;

	int myBoneListIndex			= 0;
	int myBoneBindIndex			= 0;

	unsigned int myBatchIDCount				= 0;

public:

	friend class Titan::Engine;
	RenderBatchManager() = default;
	RenderBatchManager(const RenderBatchManager&) = delete;

	static RenderBatchManager& GetInstance() { return *Instance; }

	//If we need culling its best if done inside here
	const	SharedData_RendererModel*			SharedData_RendererModelData()			const { return mySharedData_RendererModel.data(); }
			size_t								SharedData_RendererModelSize()			const { return mySharedData_RendererModel.size(); }

	const	RenderBatchManager::InstanceData*	Transform_RendererModelData()			const { return myInstanceData_RendererModel.data(); }
			size_t								Transform_RendererModelSize()			const { return myInstanceData_RendererModel.size(); }

	const	ComponentContainer*					SharedData_RenderModelComponentData()	const { return myComponents_RendererModel.data(); }
	size_t										SharedData_RenderModelComponentSize()	const { return myComponents_RendererModel.size(); }

	//Only once per frame
	void Update_RendererModel();
	void UpdateTransforms_RendererModel();
	void UpdateAnimations(int aComponentOffset, int aStartIndex, int someAmount);

	/// <summary>
	/// Keeps track how many instances are on the GPU buffer
	/// </summary>
	/// <param name="aAmount"> A amount to bind </param>
	/// <returns> Amount bound to GPU </returns>
	int UpdateAnimations(int aAmount);
	void ResetAnimationListIndex();

	unsigned int Add(const GamePointer<RendererModel>& aRenderModel);
	void Erase(unsigned int aBatchID, const GamePointer<RendererModel>& aRenderModel);

	//// Inherited via IManager
	//bool Init() override;
	//bool Clear() override;
	//void BeginOfFrame() override;
	//void EndOfFrame() override;

private:
};
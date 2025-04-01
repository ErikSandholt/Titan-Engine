#pragma once
#include <string>
#include <future>
#include <CommonUtilities/JsonFile.hpp>
#include "Engine\ComponentSystem\GamePointer.hpp"
#include "Engine\Utils_Misc\BitMask.h"
#include "Engine\ComponentSystem\Base\ComponentList.h"
#include "Engine\ComponentSystem\Base\Component.h"
#include "Engine\Utils_Misc\BinaryFileReader.h"

// Scene loader takes a Json file as stored data.
class BinaryFileReader;
namespace GameSystem { class Object; }

class Component;

class SceneLoader
{
#pragma region Structs


public:
	enum class State
	{
		Waiting,
		ReadingFile,
		InitObjects,
		Finished,
		Opening,
		Discarding
	};

private:
	enum class ComponentTypes
	{
		GameObject,
		Transform,
		RectTransform,
		MeshFilter,
		Camera,
		//Light,
		SpotLight,
		PointLight,
		DirectionLight,
		Player,
		PlayerCamera,
		AudioSourceComp,
		AudioListener,
		Enemy,
		Canvas,
		Image,
		Text,
		Navmesh,
		UIButton,
		UIMainMenu,
		UIMenu,
		UISettings,
		BoxCollider,
		SphereCollider,
		PlayerUI,
		TMP_Text,
		SpriteRenderer,
		NextSceneTrigger,
		ScriptComponent,
		ShotgunPickup,
		Count
	};

	static const std::unordered_map<std::wstring, ComponentTypes> TypeConverter;

	struct Base_Init
	{
		int GameObjectID	= 0;
		int TransformID		= 0;
		int ComponentID		= 0;
		int Enable			= 0;

		//Read
		void Phase0(const BinaryFileReader& aFile)
		{
			aFile.Copy<Base_Init>(this);
		}
	};
		

	struct ObjectInitData
	{
		int								GameObjectID;
		int								TransformID;
		bool							Enable;
		GamePointer<GameSystem::Object> Pointer;
	};

	struct ComponentListInfo
	{
		std::unordered_map<int, ObjectInitData> PtrMap		= {};
		std::atomic<bool> Continue							= false;

		ComponentListInfo() = default;

	};

	using InitilizerInfo = std::unordered_map<std::wstring, ComponentListInfo>;
	//using InitFunc = void(*)(std::vector<std::function<void()>>&, InitilizerInfo&, const BinaryFileReader&, int);

	//std::Map of objects
	//Binary file copy
	//Finalize func
	//ComponentListInfo myInfoLists[(size_t)ComponentTypes::Count];
	

#pragma endregion
	std::vector<std::function<void()>> myLateInstantiate;
	std::vector<std::future<void>> myThreads = { };
	InitilizerInfo myInitilizerInfo;
	std::atomic<State> myState			= State::Waiting;
	std::atomic<int> myActiveWorkers	= 0;
	SpinLock myAddComponentToGameObjectLock;


public:
	static std::string GetStartUpScene();
	static bool LoadScene(const std::string& aSceneName);
	//Return a object
	// SceneManager holds this object and adds it to the list
	// Object can:
	// Check if done
	// Discard	//Destroy all objects
	// Open		//Activates components and gameObjects
	//static bool AsyncLoadScene(const std::string& aSceneName);
	static void CreateEmptyScene();

	State GetState() const { return myState; }

	void Discard();
	void Open(bool whenAssetsLoaded = false);

private:
	SceneLoader() = default;

	void InitScene(const BinaryFileReader& aFile);

	//Phase 1: Create all InfoLists with binaryFile readers
	// ENTITY_ID, GO_ID, TRANS_ID, ENABLE, GamePointer<Object>
	// EntityList<T>
	// Start working on Creating objects
	// 
	//Phase 2: Wait for Open?
	// Open:
	//	Send List to fill GamePointer
	//  Finilize
	//  Awake
	// Discard:
	//	Deconstruct

	static void PushLateInstance(std::vector<std::function<void()>>& aLateInstantiate, const std::function<void()>& aFunc);
	void AddCompToGO(const GamePointer<GameObject>& aGameObject, const GamePointer<Component>& aComponent);
	
	void ContinueWorkerThread(ComponentListInfo& aInfo);
	void ContinueAllWorkerAndWait();

	void WaitForWorkers() const;
	void WaitForContinue(std::atomic<bool>& aContinue);


	void InitUnused(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitGameObjects(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitTransforms(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitRectTransforms(	ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitGO_TF(				ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitMeshFilter(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitAudioSource(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitAudioListener(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitLightDirection(	ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitLightPoint(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitLightSpot(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitCameras(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitPlayerCamera(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitEnemies(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitPlayers(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitNavmesh(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitBoxCollider(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitSphereCollider(	ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitColliders(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitSpriteRenderers(	ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitTriggerAreas(	ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitGameManager(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitPowerup(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitCinematic(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitDialogue(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitGrid(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitAnimations(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitUICanvas(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitUIImage(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitUIText(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitUIButton(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitUIMainMenu(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitUIMenu(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitUISettings(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitUIPlayer(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitUIInGame(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitWater(			ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	//void InitUIManager(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitScriptComponent(	ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitNextSceneTrigger(	ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);
	void InitShotgunPickup(		ComponentListInfo& Info, const BinaryFileReader& aFile, int aSize);

	

#pragma region [Initialization Phases]
	//Phase 0:
	// Reading File
	// Wait
	template<typename ContainerT = struct Base_Init>
	std::vector<ContainerT> InitPhase0(ComponentListInfo& aInfo, const BinaryFileReader& aFile, int aSize);

	//Phase 1:
	// Set values
	template<typename T, typename ContainerT = struct Base_Init>
	std::vector<T> InitPhase1(ComponentListInfo& aInfo, const std::vector<ContainerT>& someData, int aSize);

	//Phase 2:
	// Communicate with other objects Ex set Objects Hierarchy
	template<typename T, typename ContainerT = struct Base_Init>
	void InitPhase2(ComponentListInfo& aInfo, T* outList, int aSize);

	//Phase 3:
	// Finalize
	template<typename T, typename ContainerT = struct Base_Init>
	void InitPhase3(ComponentListInfo& aInfo, T* aList, int aSize);

	//Phase 4:
	// Awake
	template<typename T>
	void InitPhase4(ComponentListInfo& aInfo);
#pragma endregion
};

template<typename ContainerT>
inline std::vector<ContainerT> SceneLoader::InitPhase0(ComponentListInfo& aInfo, const BinaryFileReader& aFile, int aSize)
{
	std::vector<ContainerT> objects(aSize);
	for (size_t i = 0; i < aSize; ++i)
	{
		ContainerT& data = objects[i];
		data.Phase0(aFile);

		auto& obj			= aInfo.PtrMap[data.ComponentID];
		obj.GameObjectID	= data.GameObjectID;
		obj.TransformID		= data.TransformID;
		obj.Enable			= data.Enable;

	}

	return objects;
}

template<typename T, typename ContainerT>
inline std::vector<T> SceneLoader::InitPhase1(ComponentListInfo& aInfo, const std::vector<ContainerT>& someData, int aSize)
{
	std::vector<T> objects(aSize);
	
	const ContainerT*	dataList	= someData.data();
	T*					objList		= objects.data();
	for (size_t i = 0; i < aSize; ++i)
	{
		const ContainerT&	data	= dataList[i];
		T&					obj		= objList[i];

		obj.Initialize();

		obj.SetID(data.ComponentID);
		BitMask::SetBit(obj.myComponentState, Component::C_State::Enabled, data.Enable);

		if constexpr (!std::is_same<ContainerT, Base_Init>().value) { data.Phase1(obj); }
	}

	myActiveWorkers.fetch_sub(1, std::memory_order_relaxed);
	WaitForContinue(aInfo.Continue);

	return objects;
}

template<typename T, typename ContainerT>
inline void SceneLoader::InitPhase2(ComponentListInfo& aInfo, T* outList, int aSize)
{ 
	auto& transformMap = myInitilizerInfo.at(L"Transform").PtrMap;
	//int size = static_cast<int>(outObjects.size());

	for (size_t i = 0; i < aSize; ++i)
	{
		T& obj = outList[i];
		int compID	= obj.GetID();
		int transformID = aInfo.PtrMap.at(compID).TransformID;

		BitMask::SetBit(obj.myComponentState, Component::C_State::Hierarchy, transformMap.at(transformID).Enable);

		if constexpr (!std::is_same<ContainerT, Base_Init>().value) { ContainerT::Phase2(aInfo, obj); }
	}

	myActiveWorkers.fetch_sub(1, std::memory_order_relaxed);
	WaitForContinue(aInfo.Continue);
}

template<typename T, typename ContainerT>
inline void SceneLoader::InitPhase3(ComponentListInfo& aInfo, T* aList, int aSize)
{
	if (myState == State::Discarding) {
		return;
	}

	ComponentList<T>::GrowToContain(aSize);
	std::vector<GamePointer<T>> pointerList = ComponentList<T>::Append(aList, aSize);

	for (size_t i = 0; i < aSize; ++i)
	{
		auto& ptr	= aInfo.PtrMap.at(aList[i].myID);
		ptr.Pointer = std::move(pointerList[i]);
	}

	--myActiveWorkers;
	WaitForContinue(aInfo.Continue);

	auto& gameObjectList	= myInitilizerInfo.at(L"GameObject");
	auto& transformList		= myInitilizerInfo.at(L"Transform");
	auto it					= aInfo.PtrMap.begin();
	while (it != aInfo.PtrMap.end())
	{
		T* object						= reinterpret_cast<T*>(		it->second.Pointer.Get());
		ObjectInitData& gameObjectData	= gameObjectList.PtrMap.at(	it->second.GameObjectID);
		ObjectInitData& transformData	= transformList.PtrMap.at(	it->second.TransformID);
		
		BitMask::SetBit(object->myComponentState, Component::C_State::Hierarchy, transformData.Enable);

		object->SetTransform(transformData.Pointer);
		object->SetGameObject(gameObjectData.Pointer);

		AddCompToGO(gameObjectData.Pointer, it->second.Pointer);

		if constexpr (!std::is_same<ContainerT, Base_Init>().value) { ContainerT::Phase3(aInfo, *object); }
		++it;
	}

	myActiveWorkers.fetch_sub(1, std::memory_order_relaxed);
	WaitForContinue(aInfo.Continue);
}

template<typename T>
inline void SceneLoader::InitPhase4(ComponentListInfo& aInfo)
{
	if (myState == State::Discarding) {
		return;
	}


	//Phase 4:
	//Awake

	auto it = aInfo.PtrMap.begin();
	while (it != aInfo.PtrMap.end())
	{
		Component* object = reinterpret_cast<Component*>(it->second.Pointer.Get());
		object->BaseAwake();

		++it;
	}

	myActiveWorkers.fetch_sub(1, std::memory_order_relaxed);
	//WaitForContinue(aInfo.Continue);
}

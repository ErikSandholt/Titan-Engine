#include "SceneLoader.h"
#include <unordered_map>
#include <Engine/Debug/Debug_Log.h>
#include <CommonUtilities/Vector.h>
#include <GraphicsEngine/Mesh_Model/ModelInstanceFactory.h>
#include <Engine\AssetManagement\AM_Mesh.h>
#include <Engine\AssetManagement\AM_Animation.h>
#include "Engine\Utils_Misc\WorkingThreadManager.h"

#include <Engine/Utils_Misc\BinaryFileReader.h>
#include <Engine/Utils_Misc/StringCast.hpp>
#include <Engine\ComponentSystem\GameObject.h>

#include "Engine/EngineSettings.h"

#define USE_UNITY_LOADER


const std::unordered_map<std::wstring, SceneLoader::ComponentTypes> SceneLoader::TypeConverter =
	{
		{ L"GameObject",			ComponentTypes::GameObject },
		{ L"Transform",				ComponentTypes::Transform},
		{ L"RectTransform",			ComponentTypes::RectTransform},
		{ L"MeshFilter",			ComponentTypes::MeshFilter},
		{ L"Camera",				ComponentTypes::Camera},
		{ L"SpotLight",				ComponentTypes::SpotLight},
		{ L"PointLight",			ComponentTypes::PointLight},
		{ L"DirectionLight",		ComponentTypes::DirectionLight},
		{ L"Player",				ComponentTypes::Player},
		{ L"PlayerCamera",			ComponentTypes::PlayerCamera},
		{ L"AudioSourceComp",		ComponentTypes::AudioSourceComp},
		{ L"AudioListener",			ComponentTypes::AudioListener},
		{ L"Enemy",					ComponentTypes::Enemy},
		{ L"Canvas",				ComponentTypes::Canvas},
		{ L"Image",					ComponentTypes::Image},
		{ L"Text",					ComponentTypes::Text},
		{ L"Navmesh",				ComponentTypes::Navmesh},
		{ L"UIButton",				ComponentTypes::UIButton},
		{ L"UIMainMenu",			ComponentTypes::UIMainMenu},
		{ L"UIMenu",				ComponentTypes::UIMenu},
		{ L"UISettings",			ComponentTypes::UISettings},
		{ L"BoxCollider",			ComponentTypes::BoxCollider},
		{ L"SphereCollider",		ComponentTypes::SphereCollider},
		{ L"PlayerUI",				ComponentTypes::PlayerUI},
		{ L"NextSceneTrigger",		ComponentTypes::NextSceneTrigger},
		{ L"ShotgunPickup",			ComponentTypes::ShotgunPickup},
		
		{ L"TMP_Text",				ComponentTypes::TMP_Text},
		{ L"SpriteRenderer",		ComponentTypes::SpriteRenderer},

		{ L"ScriptComponent",		ComponentTypes::ScriptComponent},
	};

std::string SceneLoader::GetStartUpScene()
{
	nlohmann::json json;
	ES_Game::JsonFile::GetJsonFile(json, "Startup.json");

	return json["Startup_Scene"];
}

bool SceneLoader::LoadScene(const std::string& aSceneName)
{
	const std::string sceneFolderPath = TITAN_ASSETS_PATH "Scenes/";

	BinaryFileReader sceneData; 
	if (sceneData.OpenFile((sceneFolderPath + aSceneName + ".scene").c_str()))
	{
		LOG_MESSAGE("Loading Scene: \"%s\"", aSceneName.c_str());

		SceneLoader sl;
		sl.InitScene(sceneData);
		sl.Open(EngineSettings::global_WaitForSceneToFullyLoad);
		return true;
	}

	LOG_WARNING("Couldn't find Scene: \"%s\"", aSceneName.c_str());
	return false;
}

#define INIT_TYPE(initFunc)										\
{													\
	BinaryFileReader file	= aFile;								\
	int size				= file.Read<int>();					\
	auto& info = myInitilizerInfo[typeName];							\
	info.PtrMap.reserve(size);									\
	myActiveWorkers.fetch_add(1, std::memory_order_relaxed);					\
	myThreads.push_back(WorkingThreadManager::Enqueue([this, &info, file = std::move(file), size]()	\
		{											\
			initFunc(info, file, size);							\
													\
		}											\
	));												\
	break;												\
}													\

void SceneLoader::InitScene(const BinaryFileReader& aFile)
{
	std::vector<std::function<void()>> lateInstantiate;
	lateInstantiate.reserve(100);
	std::unordered_map<std::wstring, ComponentListInfo> initializers;

	initializers.reserve((size_t)ComponentTypes::Count);


	std::wstring typeName;
	
	//Phase 0-1
	myState = State::ReadingFile;

	while (aFile.HasData())
	{

		typeName = aFile.Read<wchar_t*>();
		int dataSize = aFile.Read<int>(); dataSize;
		try
		{

			switch (TypeConverter.at(typeName))
			{
				case ComponentTypes::GameObject:		INIT_TYPE(InitGameObjects)
				case ComponentTypes::Transform:			INIT_TYPE(InitTransforms)
				case ComponentTypes::RectTransform:		INIT_TYPE(InitRectTransforms)
				case ComponentTypes::MeshFilter:		INIT_TYPE(InitMeshFilter)
				case ComponentTypes::Camera:			INIT_TYPE(InitCameras)
				case ComponentTypes::SpotLight:			INIT_TYPE(InitLightSpot)
				case ComponentTypes::PointLight:		INIT_TYPE(InitLightPoint)
				case ComponentTypes::DirectionLight:		INIT_TYPE(InitLightDirection)
				case ComponentTypes::Player:			INIT_TYPE(InitPlayers)
				case ComponentTypes::PlayerCamera:		INIT_TYPE(InitPlayerCamera)
				case ComponentTypes::AudioSourceComp:		INIT_TYPE(InitAudioSource)
				case ComponentTypes::AudioListener:		INIT_TYPE(InitAudioListener)
				case ComponentTypes::Enemy:			INIT_TYPE(InitEnemies)
				case ComponentTypes::Canvas:			INIT_TYPE(InitUICanvas)
				case ComponentTypes::Image:			INIT_TYPE(InitUIImage)
				case ComponentTypes::Text:			INIT_TYPE(InitUIText)
				case ComponentTypes::Navmesh:			INIT_TYPE(InitNavmesh)
				case ComponentTypes::UIButton:			INIT_TYPE(InitUIButton)
				case ComponentTypes::UIMainMenu:		INIT_TYPE(InitUIMainMenu)
				case ComponentTypes::UIMenu:			INIT_TYPE(InitUIMenu)
				case ComponentTypes::UISettings:		INIT_TYPE(InitUISettings)
				case ComponentTypes::BoxCollider:		INIT_TYPE(InitBoxCollider)
				case ComponentTypes::SphereCollider:		INIT_TYPE(InitSphereCollider)
				case ComponentTypes::PlayerUI:			INIT_TYPE(InitUnused)
				case ComponentTypes::ScriptComponent:		INIT_TYPE(InitScriptComponent)
				case ComponentTypes::NextSceneTrigger:		INIT_TYPE(InitNextSceneTrigger)
				case ComponentTypes::ShotgunPickup:		INIT_TYPE(InitShotgunPickup)
			}

		} catch (const std::out_of_range& e)
		{
			OutputDebugStringA(e.what());
			LOG_ERROR("Error, typeName \"%s\" was not found!", Titan::string_cast(typeName).c_str());
		}

		aFile.Transverse(dataSize);
	}



	WaitForWorkers();	

	myState = State::InitObjects;

	ContinueAllWorkerAndWait(); //Phase 2

	myState = State::Finished;

}

void SceneLoader::CreateEmptyScene()
{
	LOG_MESSAGE("Loading Empty scene...");
}

void SceneLoader::Discard()
{
	if (myState == State::Opening || myState == State::Discarding) return;


	while (myState != State::Finished)
	{

	}

	myState = State::Discarding;

	ContinueAllWorkerAndWait(); //Phase 3 
	//Skip Phase 4

	for (auto& thread : myThreads)
	{
		thread.wait();
	}


}

void SceneLoader::Open(bool whenAssetsLoaded)
{
	if (myState == State::Opening || myState == State::Discarding) return;

	while (myState != State::Finished)
	{

	}

	myState = State::Opening;

	ContinueAllWorkerAndWait(); //Phase 3 1/2
	ContinueAllWorkerAndWait(); //Phase 3 2/2
	
	
	ContinueAllWorkerAndWait(); //Phase 4
	

	for (auto& thread : myThreads)
	{
		thread.wait();
	}

	for (auto& func : myLateInstantiate)
	{
		func();
	}

	if (whenAssetsLoaded)
	{
		while (!AM_Mesh::GetInstance().IsAssetsReady())
		{
			//Nothing
		}
		while (!AM_Animation::GetInstance().IsAssetsReady())
		{
			//Nothing
		}

	}
}

void SceneLoader::PushLateInstance(std::vector<std::function<void()>>& aLateInstantiate, const std::function<void()>& aFunc)
{
	static SpinLock lock;
	lock.lock();

	aLateInstantiate.push_back(aFunc);

	lock.unlock();
}

void SceneLoader::AddCompToGO(const GamePointer<GameObject>& aGameObject, const GamePointer<Component>& aComponent)
{
	myAddComponentToGameObjectLock.lock();
	aGameObject->myComponents.push_back(aComponent);
	myAddComponentToGameObjectLock.unlock();
}

void SceneLoader::ContinueWorkerThread(ComponentListInfo& aInfo)
{
	myActiveWorkers.fetch_add(1, std::memory_order_relaxed);
	aInfo.Continue.store(true, std::memory_order_relaxed);
}

void SceneLoader::ContinueAllWorkerAndWait()
{
	auto it = myInitilizerInfo.begin();
	while (it != myInitilizerInfo.end())
	{
		ContinueWorkerThread(it->second);
		++it;
	}

	WaitForWorkers();
}

void SceneLoader::WaitForWorkers() const
{
	while (0 < myActiveWorkers.load(std::memory_order_relaxed)) {}
}

void SceneLoader::WaitForContinue(std::atomic<bool>& aContinue)
{
	while (!aContinue.load(std::memory_order_relaxed)) {}

	aContinue.store(false, std::memory_order_relaxed);
	//aContinue = false;
}

void SceneLoader::InitUnused(ComponentListInfo& aInfo, const BinaryFileReader& /*aFile*/, int /*aSize*/)
{
	myActiveWorkers.fetch_sub(1, std::memory_order_acquire);
	WaitForContinue(aInfo.Continue);

	myActiveWorkers.fetch_sub(1, std::memory_order_acquire);
	WaitForContinue(aInfo.Continue);


	switch (myState)
	{
		default: TITAN_ASSERT("This option is not available so something wrong happened"); break;
	case State::Discarding: break;
		case State::Opening:
		{

			myActiveWorkers.fetch_sub(1, std::memory_order_acquire);
			WaitForContinue(aInfo.Continue);


			myActiveWorkers.fetch_sub(1, std::memory_order_acquire);
			WaitForContinue(aInfo.Continue);
		}
	}

	myActiveWorkers.fetch_sub(1, std::memory_order_relaxed);
}

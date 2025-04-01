#pragma once
#include <vector>
#include <unordered_map>
#include "IGameDataList.h"
#include <Engine/Utils_Misc/Slot_Map.h>
#include "Engine/ComponentSystem/Slot_Manager.hpp"
#include "GameObject.h"
#include "GamePointer.hpp"

namespace Titan
{
	class Engine;
}

class GameObjectManager : public IGameDataList, protected Slot_Manager<GameObject>
{
public:
	friend class Titan::Engine;

	/// <summary>
	/// Assumes that each component's state and ID has been set. 
	/// Adds to start list.
	/// </summary>
	/// <param name="aSource"	> The source of all the components	</param>
	/// <param name="aAmount"	> The amount of the components		</param>
	/// <returns				> Return components					</returns>
	static std::vector<GamePointer<GameObject>> Append(GameObject* aSource, int aAmount)			{ return Instance->Internal_Append(aSource, aAmount); }
	static GamePointer<GameObject> Create()															{ return Instance->Internal_Create(); }
	static GamePointer<GameObject> Create(int aID)													{ return Instance->Internal_Create(aID); }
	static void Destroy(const GamePointer<GameObject>& aGameObject)									{ Instance->Internal_Destroy( Key{ aGameObject.GetKeyIndex(), aGameObject.GetKeyGeneration() }); }
	static void Destroy(int aID)																	{ Instance->Internal_Destroy(Instance->Internal_GetKey(aID)); }

	static GamePointer<GameObject> GetGameObject(int aID)											{ return Instance->GetGamePointer(aID); }
	static GamePointer<GameObject> GetGameObject(const std::string& aName)							{ return Instance->GetGamePointer(aName); }
	static std::vector< GamePointer<GameObject> >FindGameObjects(float aRange)						{ return Instance->Internal_FindGameObjects(aRange); }
	static std::vector< GamePointer<GameObject> >FindGameObjectsByTag(const std::string& aTag)		{ return Instance->Internal_FindGameObjectsByTag(aTag); }
	static std::vector< GamePointer<GameObject> >GetList()											{ return Instance->Internal_GetList(); }
	static void GrowToContain(int aSize)															{ return Instance->Internal_GrowToContain(aSize); }

	static GamePointer<GameSystem::Object> FindObject(const std::string& aType, int aID);

private:
	static GameObjectManager*				Instance;
	GameObject								myTypeChecker;


	GameObjectManager() = default;
	~GameObjectManager() = default;

	static void CleanupManager() { Instance->Cleanup(); }
	static void MergeManager() { Instance->Merge(); }

	static GameObject* Get(unsigned int aIndex, int aGeneration)					{ return Instance->Internal_Get(aIndex, aGeneration); }
	static Key GetKey(int aID)														{ return Instance->Internal_GetKey(aID); }

	GameObject* Internal_Get(unsigned int aIndex, int aGeneration);
	Key Internal_GetKey(int aID);
	inline int RandomID() const;

	GamePointer<GameObject> GetGamePointer(const std::string& aName);
	void Internal_GrowToContain(int aSize);
	GamePointer<GameObject> Internal_Create();
	GamePointer<GameObject> Internal_Create(int aID);
	std::vector<GamePointer<GameObject>> Internal_Append(GameObject* aSource, int aAmount);
	void Internal_Destroy(const Key& aKey);

	std::vector< GamePointer<GameObject> >  Internal_FindGameObjectsByTag(const std::string& aTag);
	std::vector< GamePointer<GameObject> >  Internal_FindGameObjects(float aRange);
	std::vector< GamePointer<GameObject> > Internal_GetList();

	static void DestroyGameObject(GameObject& aGameObject);


	// Inherited via IGameDataList
	std::vector< GamePointer<GameSystem::Object> > GetDataList() override;
	GameObject* GetObject(int aID) override;
	GameObject* GetObject(int aIndex, int aGeneration) override;
	GamePointer<GameSystem::Object> GetGamePointer(int aID) override;
	GamePointer<GameSystem::Object> GetGamePointer(int aIndex, int aGeneration) override;
	const GameSystem::Object* GetTypeChecker() const override;
	bool HasKey(int aIndex, int aGeneration) override;
	void Merge() override;
	void Cleanup() override;

	// Inherited via Slot_Manager
	void EntityDeconstruction(GameObject& aEntity) override;
};


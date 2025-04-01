#include "GameObjectManager.h"
#include <random>
#include <iostream>


GameObjectManager* GameObjectManager::Instance = nullptr;

GamePointer<GameSystem::Object> GameObjectManager::FindObject(const std::string& aType, int aID)
{
	if (aType != typeid(GameObject).name()) return {};

	return FindGameObjectByID(aID);
}

GameObject* GameObjectManager::Internal_Get(unsigned int aIndex, int aGeneration)
{
	return GetObject(aIndex, aGeneration);
}

Key GameObjectManager::Internal_GetKey(int aID)
{

	struct FindByID
	{
		int ID;

		bool operator()(const GameObject& aEntity) const
		{
			if(aEntity.GetID() != ID)														return false;
			if (BitMask::GetBit(aEntity.myGameObjectState, GameObject::GO_State::Destroy))	return false;

			return true;
		}

	};

	return FindEntity(FindByID(aID));
}

inline int GameObjectManager::RandomID() const
{
	std::random_device randEngine;
	std::uniform_int_distribution<int> uniformInt(0, INT_MAX);
	return uniformInt(randEngine);
}

void GameObjectManager::Cleanup()
{
	EntityCleanup();
}

void GameObjectManager::EntityDeconstruction(GameObject& aEntity)
{
	aEntity;
	
}

void GameObjectManager::Merge()
{
	EntityMerge();
}

GamePointer<GameObject> GameObjectManager::GetGamePointer(const std::string& aName)
{

	struct FindByName
	{
		const std::string& Name;

		bool operator()(const GameObject& aEntity) const
		{

			if (BitMask::GetBit(aEntity.myGameObjectState, GameObject::GO_State::Destroy)) return false;
			return aEntity.GetName() == Name;
		}
	};

	Key key = FindEntity(FindByName(aName));

	if (key.Index == -1) return {};

	return GamePointer<GameObject>(const_cast<GameObjectManager*>(this), key.Index, key.Generation);
}

void GameObjectManager::Internal_GrowToContain(int aSize)
{
	GrowToContainEntitys(aSize);
}

GamePointer<GameObject> GameObjectManager::Internal_Create()
{
	return Internal_Create( RandomID() );
}

GamePointer<GameObject> GameObjectManager::Internal_Create(int aID)
{
	Key key				= CreateEntity();
	GameObject* entity	= GetEntity(key);

	entity->SetID(aID);
	entity->myPointer = GamePointer<GameObject>(this, key.Index, key.Generation);

	BitMask::ClearMask(	entity->myGameObjectState);
	BitMask::SetBit(	entity->myGameObjectState, GameObject::GO_State::Enable,			true);
	BitMask::SetBit(	entity->myGameObjectState, GameObject::GO_State::ParentHierarchy,	true);

	return entity->myPointer;
}

std::vector<GamePointer<GameObject>> GameObjectManager::Internal_Append(GameObject* aSource, int aAmount)
{
	std::vector<GamePointer<GameObject>>	list(aAmount);
	std::vector<Key>						keys = Slot_Manager<GameObject>::AppendEntitys(aSource, aAmount);


	for (size_t i = 0; i < aAmount; ++i)
	{
		const Key& key = keys[i];
		list[i] = GamePointer<GameObject>(this, key.Index, key.Generation);
		GetEntity(key)->myPointer = list[i];
	}

	return list;
}

void GameObjectManager::Internal_Destroy(const Key& aKey)
{
	if (aKey.Index == -1) return;

	auto entity = GetEntity(aKey);

	if (BitMask::GetBit(entity->myGameObjectState, GameObject::GO_State::Destroy)) return;

	//Destroy children first
	auto transform = entity->GetTransform().Get();
	int childCount = transform->GetChildrenAmount();

	for (int i = 0; i < childCount; ++i)
	{
		auto child = transform->GetChild(0);
		child->GetGameObject()->Destroy();
	}

	for (int i = 0; i < entity->myComponents.size(); ++i)
	{
		entity->myComponents[i]->SetEnable(false);
	}

	while (entity->myComponents.empty() == false)
	{
		auto& comp = entity->myComponents.back();
		comp->Destroy();

	}

	//Mark for delete

	BitMask::SetBit(entity->myGameObjectState, GameObject::GO_State::Destroy, true);
	DestroyEntity(aKey);
}

std::vector<GamePointer<GameObject>> GameObjectManager::Internal_FindGameObjectsByTag(const std::string& aTag)
{
	struct FindByTag
	{
		const std::string& Tag;

		bool operator()(const GameObject& aEntity) const
		{
			return aEntity.GetTag() == Tag;
		}
	};

	auto keys = FindEntitys( FindByTag(aTag) );
	std::vector< GamePointer<GameObject> > gameObjects;
	gameObjects.reserve(keys.size());

	for (int i = 0; i < keys.size(); ++i)
	{
		Key& key		= keys[i];

		if (BitMask::GetBit(GetObject(key.Index, key.Generation)->myGameObjectState, GameObject::GO_State::Destroy)) continue;

		gameObjects.push_back(GamePointer<GameObject>(this, key.Index, key.Generation));
	}

	return gameObjects;
}

std::vector<GamePointer<GameObject>> GameObjectManager::Internal_FindGameObjects(float aRange)
{
	aRange;
	/*struct FindByRange
	{
		float range;

		bool operator()(const GameObject& aEntity) const
		{
			Transform& transform = *aEntity.GetTransform().Get();


			return aEntity.GetTag() == Tag;
		}
	};

	auto keys = FindEntitys(FindByTag(aTag));
	std::vector< GamePointer<GameObject> > gameObjects;
	gameObjects.resize(keys.size());

	for (int i = 0; i < keys.size(); ++i)
	{
		Key& key = keys[i];
		gameObjects[i] = GamePointer<GameObject>(this, key.Index, key.Generation);
	}*/

	return std::vector<GamePointer<GameObject>>();
}

std::vector<GamePointer<GameObject>> GameObjectManager::Internal_GetList()
{

	int size = GetTotalCount();

	std::vector< GamePointer<GameObject> > list;
	list.reserve(static_cast<size_t>(size));

	for (int i = 0; i < size; ++i)
	{
		Key key = GetEntityKey(i);

		if (BitMask::GetBit(GetObject(key.Index, key.Generation)->myGameObjectState, GameObject::GO_State::Destroy)) continue;

		list.push_back({ this, key.Index, key.Generation });
	}

	return list;
}

void GameObjectManager::DestroyGameObject(GameObject& aGameObject)
{
	aGameObject;

}

std::vector<GamePointer<GameSystem::Object>> GameObjectManager::GetDataList()
{
	int size					= GetTotalCount();

	std::vector< GamePointer<GameSystem::Object> > list;
	list.reserve(static_cast<size_t>(size));

	for (int i = 0; i < size; ++i)
	{
		Key key = GetEntityKey(i);

		if (BitMask::GetBit(GetObject(key.Index, key.Generation)->myGameObjectState, GameObject::GO_State::Destroy)) continue;

		list.push_back({ this, key.Index, key.Generation });
	}

	return list;
}

GameObject* GameObjectManager::GetObject(int aID)
{

	return GetEntity( GetKey(aID) );
}

GameObject* GameObjectManager::GetObject(int aIndex, int aGeneration)
{

	return GetEntity( Key(aIndex, aGeneration) );
}

GamePointer<GameSystem::Object> GameObjectManager::GetGamePointer(int aID)
{
	Key key = Internal_GetKey(aID);

	if (key.Index == -1) return {};


	if (BitMask::GetBit(GetObject(key.Index, key.Generation)->myGameObjectState, GameObject::GO_State::Destroy)) return {};

	key = Internal_GetKey(aID);

	return GamePointer<GameSystem::Object>(this, key.Index, key.Generation);
}

GamePointer<GameSystem::Object> GameObjectManager::GetGamePointer(int aIndex, int aGeneration)
{
	return GamePointer<GameSystem::Object>(this, aIndex, aGeneration);
}

const GameSystem::Object* GameObjectManager::GetTypeChecker() const
{
	return &myTypeChecker;
}

bool GameObjectManager::HasKey(int aIndex, int aGeneration)
{
	GameObject* entity = GetEntity(Key(aIndex, aGeneration));

	if (entity == nullptr) return false;

	return BitMask::GetBit(entity->myGameObjectState, GameObject::GO_State::Destroy) == false;

}

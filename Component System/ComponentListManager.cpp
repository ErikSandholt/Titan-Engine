#include "ComponentListManager.h"
#include <Engine/ComponentSystem/Components/Transform.h>


ComponentListManager* ComponentListManager::Instance = nullptr;

GamePointer<GameSystem::Object> ComponentListManager::FindObject(const std::string& aType, int aID)
{
	//This is expensive
	for (auto& list : Instance->myLists)
	{
		if (list->GetTypeChecker()->GetTypeName() != aType) continue;

		return list->GetGamePointer(aID);
	}

	return GamePointer<GameSystem::Object>();
}

void ComponentListManager::FilterByRange(std::vector<GamePointer<Component>>& aOutList, const CU::Vector3f& aPosition, float aRange)
{
	int size = static_cast<int>(aOutList.size());

	if (size == 0) return;

	for (int i = size - 1; 0 <= i; --i)
	{
		auto& comp = aOutList[i];
		auto transform = comp->GetTransform();
		float absolute = (transform->GetPosition() - aPosition).LengthSq();
		
		if (absolute == 0) continue;

		if (aRange < sqrtf(absolute))
		{
			aOutList.erase(aOutList.begin() + i);
		}
	}
}

ComponentListManager::ComponentListManager()
{
	Instance = this;
}

ComponentListManager::~ComponentListManager()
{
	auto it = myLists.begin();
	while (it != myLists.end())
	{
		delete *it;
		it = myLists.erase(it);
	}

	Instance = nullptr;
}

void ComponentListManager::Cleanup()
{
	for (auto& list : myLists)
	{
		list->Cleanup();
	}
}

void ComponentListManager::Merge()
{
	for (auto& list : myLists)
	{
		list->Merge();
	}
}

void ComponentListManager::Update()
{
	int size = static_cast<int>(myLists.size());
	for (int i = size - 1; 0 <= i; --i)
	{
		myLists[i]->Start();
	}

	for (int i = size - 1; 0 <= i; --i)
	{
		myLists[i]->Update();
	}
}

void ComponentListManager::LateUpdate()
{
	int size = static_cast<int>(myLists.size());
	for (int i = size - 1; 0 <= i; --i)
	{
		myLists[i]->LateUpdate();
	}
}

void ComponentListManager::DrawGizmo()
{
	for (auto& list : myLists)
	{
		list->DrawGizmo();
	}
}

uint32_t ComponentListManager::Jenkins_one_at_a_time_hash(const uint8_t* key, size_t length)
{
	size_t i = 0;
	uint32_t hash = 0;
	while (i != length) {
		hash += key[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

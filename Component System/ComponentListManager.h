#pragma once
#include <map>
#include <string>
#include <atomic>
//#include <typeinfo>
//#include <typeindex>
#include "IComponentList.h"
#include <CommonUtilities\Vector.h>
#include "Engine\Utils_Misc\SpinLock.h"

class GameObject;
namespace Titan
{
	class Engine;
}

namespace Editor
{
	class S4Editor;
}


class ComponentListManager
{
public:
	friend class Titan::Engine;
	friend class Editor::S4Editor;

	static GamePointer<GameSystem::Object> FindObject(const std::string& aType, int aID);

	template <typename T>
	static void AddCreateList(IComponentList* aList);


	template <typename T>
	static bool FindComponents(std::vector< GamePointer<Component> >& aOutList);

	static void FilterByRange(std::vector< GamePointer<Component> >& aOutList, const CU::Vector3f& aPosition, float aRange);
	//Implement this later
	//template<typename T>
	//static std::vector< GamePointer<T> > Find() 
	//{  
	//	auto& myList = Instance->myLists;
	//	auto compList = std::vector<GamePointer<T>>();
	//	auto it = myList.begin();
	//	IComponentList* list = nullptr;

	//	//While this works if a "brach" is encountered there will be duplicates of components
	//	while (it != myList.end())
	//	{
	//		list = it->second;
	//		//This list has type
	//		if (dynamic_cast<const T*>(list->GetTypeChecker()) == nullptr)
	//		{
	//			it++;
	//			continue;
	//		}

	//		auto extraComp = list->GetDataList();
	//		compList.reserve(compList.size() + extraComp.size());

	//		for (auto& comp : extraComp)
	//		{
	//			compList.push_back(comp.Cast<T>());
	//		}

	//		it++;
	//	}

	//	if (0 < compList.size()) return compList;


	//	return std::vector<GamePointer<T>>();
	//}


private:
	static ComponentListManager* Instance;
	std::vector<IComponentList*> myLists;
	SpinLock myLock;

	ComponentListManager();
	~ComponentListManager();

	static void CleanupManager();
	static void MergeManager();
	static void UpdateManager();
	static void LateUpdateManager();
	static void DrawGizmoManager();

	void Cleanup();
	void Merge();
	void Update();
	void LateUpdate();
	void DrawGizmo();


	static uint32_t Jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);

	template <typename T>
	static uint32_t GetHash();
};

template <typename T>
void ComponentListManager::AddCreateList(IComponentList* aList) {
	//std::type_index id(typeid(T)); //GetHash<T>();
	//Instance->myLists[id.hash_code()] = aList;

	Instance->myLock.lock();
	Instance->myLists.push_back(aList);
	Instance->myLock.unlock();

}

template <typename T>
bool ComponentListManager::FindComponents(std::vector<GamePointer<Component>>& aOutList) {

	Instance->myLock.lock();
	for (auto& list : Instance->myLists)
	{
		if (dynamic_cast<const T*>(list->GetTypeChecker()) == nullptr) continue;

		auto dataList = list->GetDataList();

		aOutList.reserve(aOutList.size() + dataList.size());
		for (auto& it : dataList)
		{
			aOutList.push_back(it.ForceCast<Component>());
		}
	}
	Instance->myLock.unlock();

	return !aOutList.empty();
}

inline void ComponentListManager::CleanupManager() { Instance->Cleanup(); }
inline void ComponentListManager::MergeManager() { Instance->Merge(); }
inline void ComponentListManager::UpdateManager() { Instance->Update(); }
inline void ComponentListManager::LateUpdateManager() { Instance->LateUpdate(); }

inline void ComponentListManager::DrawGizmoManager() { Instance->DrawGizmo(); }

template <typename T>
uint32_t ComponentListManager::GetHash() {
	std::string name = typeid(T).name();
	return Jenkins_one_at_a_time_hash((uint8_t*)name.c_str(), name.size());
}

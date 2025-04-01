#pragma once
#include <vector>
#include "Engine/ComponentSystem/Slot_Manager.hpp"
#include "ComponentListManager.h"
#include "IComponentList.h"
#include <Engine/Utils_Misc/BitMask.h>

//Temp?
#include <random>
#include <iostream>

#define CHECK if(Instance == nullptr) { Instance = new ComponentList<T>(); ComponentListManager::AddCreateList<T>(Instance);  }

template<typename T>
class GamePointer;


template<typename T>
class ComponentList : public IComponentList, protected Slot_Manager<T>
{
public:
	friend class ComponentListManager;
	friend class GamePointer<T>; //Not needed?
	friend class GameObject;


	template <typename T>
	static bool FindComponents(std::vector< GamePointer<GameSystem::Object> >& aOutList)	{ return ComponentListManager::FindComponents<T>(aOutList); }

	static GamePointer<T> Create()															{ CHECK return	Instance->Internal_Create(); }
	static GamePointer<T> Create(int aID)													{ CHECK return	Instance->Internal_Create(aID); }
	/// <summary>
	/// Assumes that each component's state and ID has been set. 
	/// Adds to start list and calls awake.
	/// </summary>
	/// <param name="aSource"	> The source of all the components	</param>
	/// <param name="aAmount"	> The amount of the components		</param>
	/// <returns				> Return components					</returns>
	static std::vector<GamePointer<T>> Append(T* aSource, int aAmount)						{ CHECK return	Instance->Internal_Append(aSource, aAmount); }
	static GamePointer<T> Find(int aID)														{ CHECK return	Instance->GetGamePointer(aID); }
	static void Destroy(int aID)															{ CHECK			Instance->Internal_Destroy(aID); }
	static std::vector< GamePointer<T> > GetList()											{ CHECK return	Instance->Internal_GetList(); }

	static void GrowToContain(int aSize)													{ CHECK return	Instance->Internal_GrowToContain(aSize); }

private:
	static ComponentList*				Instance;
	std::vector<IComponentList*>		mySubLists;
	std::vector<Key>					myStart;
	T									myTypeChecker;

	ComponentList() = default;
	virtual ~ComponentList()
	{
		Instance = nullptr;
	}

	void Cleanup()		override;
	void Merge()		override;
	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;
	void DrawGizmo()	override;

	void Destroy(int aIndex, int aGeneration) override;
	static T* Get(unsigned int aIndex, int aGeneration)						{ return Instance->GetObject(aIndex, aGeneration); }
	static Key GetKey(int aID)												{ return Instance->Internal_GetKey(aID); }

	template<typename X>
	static void AddSubList(ComponentList<X>& aList)							{ Instance->Internal_AddSubList(aList); }

	//Internal
	void			Internal_GrowToContain(int aSize);
	GamePointer<T>	Internal_Create();
	GamePointer<T>	Internal_Create(int aID);
	std::vector<GamePointer<T>> Internal_Append(T* aSource, int aAmount);
	void			Internal_Destroy(T* aComponent);
	void			Internal_Destroy(Key aKey);
	void			Internal_Destroy(int aID);


	// Inherited via Slot_Manager
	void EntityDeconstruction(T& aEntity) override;

	std::vector< GamePointer<T> > Internal_GetList();
	std::vector< GamePointer<GameSystem::Object> > GetDataList() override;

	virtual std::vector< GamePointer<Component> > GetComponentList();
	void GetListFromSub(std::vector< GamePointer<Component> >& aOutList) override;

	Key Internal_GetKey(int aID);
	bool HasKey(int aIndex, int aGeneration) override;
	static int RandomID();


	template<typename X>
	void Internal_AddSubList(ComponentList<X>& aList);


	// Inherited via IComponentList
	GamePointer<GameSystem::Object> CreateObject()									override;
	const GameSystem::Object* GetTypeChecker() const								override;
	T* GetObject(int aID)															override;
	T* GetObject(int aIndex, int aGeneration)										override;
	GamePointer< GameSystem::Object> GetGamePointer(int aID)						override;
	GamePointer< GameSystem::Object> GetGamePointer(int aIndex, int aGeneration)	override;
};

template <class T>
ComponentList<T>* ComponentList<T>::Instance = nullptr;


template<typename T>
template<typename X>
void ComponentList<T>::Internal_AddSubList(ComponentList<X>& aList)
{
	mySubLists.push_back(aList);
}

template<typename T>
Key ComponentList<T>::Internal_GetKey(int aID)
{


	struct FindByID
	{
		int ID;

		bool operator()(const T& aEntity) const
		{
			if (BitMask::GetBit(aEntity.myComponentState, Component::C_State::Destroying))	return false;
			return aEntity.GetID() == ID;
		}

	};

	return Slot_Manager<T>::FindEntity(FindByID(aID));
}

template<typename T>
bool ComponentList<T>::HasKey(int aIndex, int aGeneration)
{
	T* entity = Slot_Manager<T>::GetEntity(Key(aIndex, aGeneration));

	if (entity == nullptr) return false;

	return BitMask::GetBit(entity->myComponentState, Component::C_State::Destroying) == false;
}

template<typename T>
int ComponentList<T>::RandomID()
{
	std::random_device randEngine;
	std::uniform_int_distribution uniformInt(0, INT_MAX);
	return uniformInt(randEngine);
}

template<typename T>
std::vector<GamePointer<Component>> ComponentList<T>::GetComponentList()
{
	auto list = Internal_GetList();
	std::vector<GamePointer<Component>> compList;
	compList.reserve(list.size());

	for (auto& comp : list)
	{
		compList.push_back(comp.template Cast<Component>());
	}

	return compList;
}

template<typename T>
void ComponentList<T>::GetListFromSub(std::vector< GamePointer<Component> >& aOutList)
{
	int size = 0;
	{
		size = static_cast<int>(mySubLists.size());
		for (int i = 0; i < size; i++)
		{
			mySubLists[i]->GetListFromSub(aOutList);
		}
	}

	auto list = GetComponentList();
	aOutList.reserve( aOutList.size() + list.size() );

	for (auto& comp : list)
	{
		aOutList.push_back(comp);
	}
}

template<typename T>
void ComponentList<T>::Cleanup()
{
	Slot_Manager<T>::EntityCleanup();
	
}

template<typename T>
void ComponentList<T>::Merge()
{
	Slot_Manager<T>::EntityMerge();
}

template<typename T>
void ComponentList<T>::Start()
{
	int startSize = static_cast<int>(myStart.size());

	for (size_t i = 0; i < startSize; ++i)
	{
		auto comp = Slot_Manager<T>::GetEntity( myStart[i] );
		if (comp)
		{
			comp->BaseStart();
		}
	}
	

	myStart.clear();
}

template<typename T>
void ComponentList<T>::Update()
{
	T* data = Slot_Manager<T>::GetData();

	for (int i = 0; i < Slot_Manager<T>::GetMainCount(); ++i)
	{
		if (data[i].GetActive())
		{
			data[i].Update();
		}
	}

	//Buffer wont update since they are "Inactive during their creation frame"
}

template<typename T>
void ComponentList<T>::LateUpdate()
{

	T* data = Slot_Manager<T>::GetData();

	for (int i = 0; i < Slot_Manager<T>::GetMainCount(); ++i)
	{
		if (data[i].GetActive())
		{
			data[i].LateUpdate();
		}
	}


	//Buffer wont update since they are "Inactive during their creation frame"
}

template<typename T>
inline void ComponentList<T>::DrawGizmo()
{
	T* data = Slot_Manager<T>::GetData();
	for (int i = 0; i < Slot_Manager<T>::GetMainCount(); ++i)
	{
		if (data[i].GetActive())
		{
			data[i].OnDrawGizmos();
		}
	}
}

template<typename T>
inline void ComponentList<T>::Destroy(int aIndex, int aGeneration)
{
	Internal_Destroy( Key(aIndex, aGeneration) );
}

template<typename T>
inline void ComponentList<T>::Internal_GrowToContain(int aSize)
{
	if (aSize < 1) return;

	Slot_Manager<T>::GrowToContainEntitys(aSize);
	myStart.reserve(aSize);
}

template<typename T>
GamePointer<T> ComponentList<T>::Internal_Create()
{
	return Internal_Create( RandomID() );
}

template<typename T>
GamePointer<T> ComponentList<T>::Internal_Create(int aID)
{
	Key key = Slot_Manager<T>::CreateEntity();
	T* comp = Slot_Manager<T>::GetEntity(key);
	
	comp->myPointer = GamePointer<T>(this, key.Index, key.Generation);
	//comp->myKeyIndex		= key.Index;
	//comp->myKeyGeneration	= key.Generation;
	//comp->myComponentList	= this;

	comp->SetID(aID);

	BitMask::ClearMask(comp->myComponentState);
	BitMask::SetBit(comp->myComponentState, Component::C_State::Enabled, true);
	BitMask::SetBit(comp->myComponentState, Component::C_State::Hierarchy, true);

	comp->Initialize();

	myStart.push_back(key);

	return GamePointer<T>(this, key.Index, key.Generation);
}

template<typename T>
inline std::vector<GamePointer<T>> ComponentList<T>::Internal_Append(T* aSource, int aAmount)
{
	std::vector<GamePointer<T>> list(aAmount);
	std::vector<T*>				components(aAmount);
	std::vector<Key>			keys = Slot_Manager<T>::AppendEntitys(aSource, aAmount);


	myStart.reserve(myStart.size() + aAmount);

	for (size_t i = 0; i < aAmount; ++i)
	{
		const Key& key = keys[i];
		list[i] = GamePointer<T>(this, key.Index, key.Generation);

		components[i] = Slot_Manager<T>::GetEntity(key);

		components[i]->myPointer = list[i];
		myStart.push_back(key);
	}


	return list;
}


template<typename T>
inline GamePointer<GameSystem::Object> ComponentList<T>::CreateObject()
{
	return Internal_Create();
}

template<typename T>
void ComponentList<T>::Internal_Destroy(T* aComponent)
{
	Internal_Destroy( aComponent->GetID() );
}

template<typename T>
void ComponentList<T>::Internal_Destroy(Key aKey)
{
	if (aKey.Index < 0) return;

	auto comp = Slot_Manager<T>::GetEntity(aKey);
	if (BitMask::GetBit(comp->myComponentState, Component::C_State::Destroying)) return;

	comp->BaseOnDisable();
	comp->OnDestroy();

	GameObject* go = comp->GetGameObject().Get();
	go->RemoveComponent(comp->GetID());

	BitMask::SetBit(comp->myComponentState, Component::C_State::Destroying, true);

	Slot_Manager<T>::DestroyEntity(aKey);
}

template<typename T>
void ComponentList<T>::Internal_Destroy(int aID)
{
	Internal_Destroy( Internal_GetKey(aID) );
	
}

template<typename T>
inline void ComponentList<T>::EntityDeconstruction(T& /*aEntity*/)
{
	__noop;
	//aEntity;
	/*GameObject* go = aEntity.GetGameObject().Get();
	go->RemoveComponent(aEntity.GetID());
	go = aEntity.GetGameObject().Get();*/
}

template<typename T>
std::vector< GamePointer<T> > ComponentList<T>::Internal_GetList()
{
	
	int size = Slot_Manager<T>::GetTotalCount();

	std::vector< GamePointer<T> > list;
	list.reserve(static_cast<size_t>(size));

	for (int i = 0; i < size; ++i)
	{
		Key key = Slot_Manager<T>::GetEntityKey(i);

		if (BitMask::GetBit(GetObject(key.Index, key.Generation)->myComponentState, Component::C_State::Destroying)) continue;

		list.push_back({ this, key.Index, key.Generation });
	}

	return list;
}

template<typename T>
std::vector<GamePointer<GameSystem::Object>> ComponentList<T>::GetDataList()
{
	

	int size = Slot_Manager<T>::GetTotalCount();

	std::vector< GamePointer<GameSystem::Object> > list;
	list.reserve(static_cast<size_t>(size));

	for (int i = 0; i < size; ++i)
	{
		Key key = Slot_Manager<T>::GetEntityKey(i);

		if (BitMask::GetBit(GetObject(key.Index, key.Generation)->myComponentState, Component::C_State::Destroying)) continue;

		list.push_back({ this, key.Index, key.Generation });
	}

	return list;
}



template<typename T>
const GameSystem::Object* ComponentList<T>::GetTypeChecker() const
{
	return  dynamic_cast<const T*>(&myTypeChecker);
}

template<typename T>
T* ComponentList<T>::GetObject(int aID)
{
	

	return Slot_Manager<T>::GetEntity( GetKey(aID) );
}

template<typename T>
T* ComponentList<T>::GetObject(int aIndex, int aGeneration)
{
	
	
	return Slot_Manager<T>::GetEntity( Key(aIndex, aGeneration) );
}

template<typename T>
GamePointer< GameSystem::Object> ComponentList<T>::GetGamePointer(int aID)
{
	Key key = Internal_GetKey(aID);

	if (key.Index == -1) return {};


	if (BitMask::GetBit(GetObject(key.Index, key.Generation)->myComponentState, Component::C_State::Destroying)) return {};

	return GamePointer<GameSystem::Object>(this, key.Index, key.Generation);
}

template<typename T>
GamePointer< GameSystem::Object> ComponentList<T>::GetGamePointer(int aIndex, int aGeneration)
{
	return GamePointer<Component>(this, aIndex, aGeneration);
}

#undef CHECK
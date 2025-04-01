#pragma once
#include <vector>
#include <string>
#include "GamePointer.hpp"
#include "Object.h"
#include "Engine/ComponentSystem/Base/ComponentList.h"
#include "Engine/ComponentSystem/Components/RectTransform.h"
#include <Engine/Utils_Misc/BitMask.h>
#include<Engine/ComponentSystem/Components/Collision/CollisionLayers.h>

// A gameobject contains components, name, tag, transform.

template <typename T>
class ComponentList;

class SceneLoader;
class GameObjectManager;
class Transform;
class RectTransform;


GamePointer<GameSystem::Object> FindObject(const std::string& aType, int aID);

std::vector<GamePointer<GameObject>> FindGameObjects();
std::vector<GamePointer<GameObject>> FindGameObjects(const CU::Vector3f& aPosition, float aRange);
std::vector<GamePointer<GameObject>> FindObjectsByTag(const std::string& aTag);
GamePointer<GameObject> FindGameObjectByID(const int& aID);
GamePointer<GameObject> FindGameObjectByName(const std::string& aName);


#undef SendMessage

class GameObject : public GameSystem::Object
{
public:
	template <typename T>
	friend class ComponentList;

	friend class GameObjectManager;
	friend class SceneLoader;
	friend class Transform;


	GameObject() = default;

	void Destroy() const;
	int GetID() const override;
	const std::string& GetName() const;
	virtual bool SendMessage(void* aMessage, unsigned int aChannel) override;
	void SetName(const std::string& aName);
	void SetStatic(bool aIsStatic);
	bool GetIsStatic() const;
	std::string GetTag() const;
	void SetTag(const std::string& aTag);
	ELayers GetLayer() const;
	void SetLayer(ELayers aLayer);
	void SetEnable(bool aIsActive);
	bool GetEnable() const;
	bool GetActive() const;
	const GamePointer<Transform>& GetTransform() const;
	const GamePointer<GameObject>& GetGameObject() const;

	std::string GetTypeName() const override;


	const Sunder::CallEvent GetFunction(const std::string& aFunctionName) const;
																									

	template <class Comp>
	GamePointer<Comp> GetComponent() const;

	template <class Comp>
	std::vector<GamePointer<Comp>> GetComponents();

	template <class Comp>
	GamePointer<Comp> GetComponentInChildren();

	template <class Comp>
	void GetComponentsInChildren(std::vector<GamePointer<Comp>>& aInAComponentList);

	template <class Comp>
	GamePointer<Comp> AddComponent();

	//This is heavy because of Cash miss
	static GamePointer<GameObject> Create();

private:
	enum class GO_State
	{
		Enable = 0,
		Static = 1,
		ParentHierarchy = 2,
		Destroy = 3
	};

	ELayers myLayer		= ELayers::Default;
	std::string myTag = "None";
	std::string myName = "GameObject";
	int myID = 0;
	unsigned int myGameObjectState = 0;
	std::vector<GamePointer<Component>> myComponents;
	GamePointer<Transform> myTransform;
	GamePointer<GameObject>	myPointer;

	//This is heavy because of Cash miss
	static GamePointer<GameObject> Create(int aID, int aTransformID);
	void SetHierarchyActivity(bool aIsActive);
	void SetTransform(const GamePointer<Transform>& aTransform);
	void SetID(int aID);

	void GetChildGameObjects(std::vector<GamePointer<GameObject>>& aOutList) const;

	template <class Comp>
	GamePointer<Comp> AddComponent(int aID);

	void AppendComponents(const GamePointer<Component>* someComponents, int aSize);
	void RemoveComponent(int aID);
};

///
template <class Comp>
GamePointer<Comp> GameObject::AddComponent()
{
	GamePointer<Comp> component = ComponentList<Comp>::Create();
	component->SetTransform(GetTransform());
	component->SetGameObject(GetGameObject());

	BitMask::SetBit(component->myComponentState, Component::C_State::Hierarchy, GetActive());

	myComponents.push_back(component);

	component->BaseAwake();
	/*if (BitMask::GetBit(myGameObjectState, GO_State::Enable))
	{
		component->OnEnable();
	}*/
	return component;
}

template <>
inline GamePointer<RectTransform> GameObject::AddComponent()
{
	GamePointer<RectTransform> component = ComponentList<RectTransform>::Create();
	component->SetTransform(component);
	component->SetGameObject(GetGameObject());

	for (auto& comp : GetComponents<Component>())
	{
		if (auto transform = comp.Cast<Transform>())
		{
			*component.Get() = *transform.Get();
			transform->Destroy();
			continue;
		}

		comp->SetTransform(component);
	}

	myTransform = component;
	myComponents.insert(myComponents.begin(), component);

	return component;
}

template <>
inline GamePointer<Transform> GameObject::AddComponent()
{
	GamePointer<Transform> component = ComponentList<Transform>::Create();
	component->SetTransform(component);
	component->SetGameObject(GetGameObject());

	myTransform = component;
	myComponents.insert(myComponents.begin(), component);

	return component;
}

/// <summary>
/// Create the component, attach it to this gameobject, calls void Awake() on it and return the component.
/// </summary>
/// <typeparam name="Comp"></typeparam>
/// <param name="aID"></param>
/// <returns></returns>
template <class Comp>
GamePointer<Comp> GameObject::AddComponent(int aID)
{
	GamePointer<Comp> component = ComponentList<Comp>::Create(aID);
	component->SetTransform(GetComponent<Transform>());
	component->SetGameObject(GetGameObject());

	BitMask::SetBit(component->myComponentState, Component::C_State::Hierarchy, GetActive());

	myComponents.push_back(component);
	component->BaseAwake();
	/*if (BitMask::GetBit(myGameObjectState, GO_State::Enable))
	{
		component->OnEnable();
	}*/
	return component;
}


template <>
inline GamePointer<RectTransform> GameObject::AddComponent(int aID)
{
	GamePointer<RectTransform> component = ComponentList<RectTransform>::Create(aID);
	component->SetTransform(component);
	component->SetGameObject(GetGameObject());

	for (auto& comp : GetComponents<Component>())
	{
		if (auto transform = comp.Cast<Transform>())
		{
			*component.Get() = *transform.Get();
			transform->Destroy();
			continue;
		}

		comp->SetTransform(component);
	}

	myTransform = component;
	myComponents.insert(myComponents.begin(), component);

	return component;
}

template <>
inline GamePointer<Transform> GameObject::AddComponent(int aID)
{
	GamePointer<Transform> component = ComponentList<Transform>::Create(aID);
	Transform& transform = *component.Get();
	transform.SetTransform(component);
	transform.SetGameObject(GetGameObject());

	myTransform = component;
	myComponents.insert(myComponents.begin(), component);

	return component;
}


/// <summary>
/// Get the Component attached to the gameobject. If no such component is attached it returns empty. It's possible to check if it found a component by using a if statement. It will be false if it’s empty.
/// </summary>
/// <typeparam name="Comp"></typeparam>
/// <returns></returns>
template <class Comp>
GamePointer<Comp> GameObject::GetComponent() const
{
	GamePointer<Comp> comp;
	for (auto& it : myComponents)
	{
		if (dynamic_cast<Comp*>(it.Get()))
		{
			comp = it.Cast<Comp>();
			break;
		}
	}

	return comp;
}

template <class Comp>
std::vector<GamePointer<Comp>> GameObject::GetComponents()
{
	std::vector<GamePointer<Comp>> compList;

	for (auto& it : myComponents)
	{
		if (dynamic_cast<Comp*>(it.Get()))
		{
			compList.push_back(it.Cast<Comp>());
		}
	}

	return compList;
}

/// <summary>
/// TODO- FIX, CURRENTLY BROKEN. Get the first Component attached to the gameobjects children. A child is another gameobject that’s attached to this gameobject with the Transform component.
/// </summary>
/// <typeparam name="Comp"></typeparam>
/// <returns></returns>
template <class Comp>
GamePointer<Comp> GameObject::GetComponentInChildren()
{
	GamePointer<Comp> comp;
	std::vector<GamePointer<GameObject>> children;
	GetChildGameObjects(children);

	for (auto& child : children)
	{
		comp = child->GetComponent<Comp>();
		if (comp) break;

		comp = child->GetComponentInChildren<Comp>();
		if (comp) break;
	}

	return comp;
}

/// <summary>
/// Get all Components attached to the gameobjects children. A child is another gameobject that’s attached to this gameobject with the Transform component.
/// </summary>
/// <typeparam name="Comp"></typeparam>
/// <param name="aInAComponentList"></param>
template <class Comp>
void GameObject::GetComponentsInChildren(std::vector<GamePointer<Comp>>& aInAComponentList)
{
	GamePointer<Comp> comp; // is "comp" even used? wtf
	std::vector<GamePointer<GameObject>> children;
	std::vector<GamePointer<Comp>> components;
	GetChildGameObjects(children);

	for (auto& child : children)
	{
		components = child->GetComponents<Comp>();
		for (auto& auto_ : components)
		{
			aInAComponentList.push_back(auto_);
		}

		child->GetComponentsInChildren(aInAComponentList);
	}
}

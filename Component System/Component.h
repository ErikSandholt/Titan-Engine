#pragma once
#include <vector>
#include "ComponentDef.h"
#include <CommonUtilities/Vector.h>
#include "Engine/ComponentSystem/GamePointer.hpp"
#include "Engine/ComponentSystem/IGameDataList.h"
#include "Engine/ComponentSystem/Object.h"
#include "Engine/EventSystems/CallEvent.hpp"


class SceneLoader;
class GameObject;
class Transform;
class RectTransform;
class ComponentListManager;
class Component;
class IComponentList;

template<typename T>
class ComponentList;

namespace Sunder { class CallEvent; }

namespace Editor { class InspectorWindow; }

/// <summary>
/// Find the component with the same ID.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="aID"></param>
/// <returns></returns>
template<typename T>
GamePointer<T> FindComponent(int aID)
{
	return ComponentList<T>::Find(aID);
}

/// <summary>
/// Find the components with the same or base type.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <returns></returns>
template <typename T>
std::vector< GamePointer<T> > FindComponents()
{
	std::vector< GamePointer<Component> > dataList = {};

	if (ComponentListManager::template FindComponents<T>(dataList) == false) return {};

	std::vector< GamePointer<T> > compList;
	compList.reserve(dataList.size());

	for (auto& i : dataList)
	{
		compList.push_back(i.ForceCast<T>() );
	}

	return compList;
}

template<typename T>
std::vector< GamePointer<T> > FindComponents(const CU::Vector3f& aPosition, float aRange)
{
	std::vector< GamePointer<Component> > dataList = {};

	if (ComponentListManager::template FindComponents<T>(dataList) == false) return {};

	ComponentListManager::FilterByRange(dataList, aPosition, aRange);

	std::vector< GamePointer<T> > compList;
	compList.reserve(dataList.size());

	for (auto& i : dataList)
	{
		compList.push_back(i.ForceCast<T>());
	}

	return compList;
}

class Collider;

class Component : public GameSystem::Object
{
public:
	template<typename T>
	friend class ComponentList;
	friend class CollisionCallback;
	friend class CharacterControllerCallback;
	friend class GameObject;
	friend class SceneLoader;
	friend class Editor::InspectorWindow;


	const GamePointer<GameObject>& GetGameObject() const;
	const GamePointer<Transform>&	GetTransform() const;

	/// <summary>
	/// Get the Component attached to the gameobject. If no such component is attached it returns an empty GamePointer. It's possible to check if it found a component by using an if statement. It will be false if it’s empty.
	/// </summary>
	/// <typeparam name="Comp"></typeparam>
	/// <returns></returns>
	template<typename Comp>
	inline GamePointer<Comp> GetComponent() const;

	//void GetComponents(std::vector<Comp*>& inAComponentList)
	template <class Comp>
	inline std::vector<GamePointer<Comp>> GetComponents();

	/// <summary>
	///  Get the first Component attached to the gameobjects children. A child is another gameobject that’s attached to this gameobject with the Transform component.
	/// </summary>
	/// <typeparam name="Comp"></typeparam>
	/// <returns></returns>
	template <class Comp>
	inline GamePointer<Comp> GetComponentInChildren();

	/// <summary>
	/// Get all Components attached to the gameobjects children. A child is another gameobject that’s attached to this gameobject with the Transform component.
	/// </summary>
	/// <typeparam name="Comp"></typeparam>
	/// <returns></returns>
	template <class Comp>
	//void GetComponentsInChildren(std::vector<Comp*>& inAComponentList)
	inline std::vector<GamePointer<Comp>> GetComponentsInChildren();

	int GetID() const override { return myID; }
	std::string GetTypeName() const override = 0;
	virtual bool SendMessage(void* /*aMessage*/, unsigned int /*aChannel*/) override { return true; };
	virtual const Sunder::CallEvent GetFunction(const std::string& aFunctionName) const = 0;
	void SetEnable(bool aIsEnabled);
	bool GetEnable() const;
	//Dirty is only set on some components ex Transform
	bool GetDirty() const;
	bool GetActive() const;
	void Destroy() const;

protected:
	enum class C_State
	{
		Hierarchy,
		Enabled,
		Awakened,
		Started,
		Destroying,
		Dirty,
		DrawEditor,

	};

	friend class Transform;
	friend class RectTransform;
	

	Component()							= default;
	~Component() override				= default;

	//Developer Only
	virtual void Initialize();

	//Clients
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();

	virtual void OnEnable();
	virtual void OnDisable();
	virtual void OnDestroy();
	virtual void OnDrawGizmos();
	virtual void OnDrawGizmosSelected();

	virtual void OnCollisionEnter(GamePointer<GameObject> other);
	virtual void OnCollisionStay(GamePointer<GameObject> other);
	virtual void OnCollisionExit(GamePointer<GameObject> other);

	virtual void OnTriggerEnter(GamePointer<GameObject> other);
	virtual void OnTriggerExit(GamePointer<GameObject> other);

	template<typename T>
	GamePointer<T> CreatePointer() const
	{
		return myPointer.Cast<T>(); //GamePointer<T>( reinterpret_cast<IGameDataList*>(myComponentList), myKeyIndex, myKeyGeneration);
	}

	bool GetCompState(C_State aState) const;
	void SetCompState(C_State aState, bool aFlag);

private:

	unsigned int			myComponentState	= 0;

	int						myID				= 0;
	GamePointer<GameObject> myGameObject;
	GamePointer<Transform>	myTransform;
	GamePointer<Component>	myPointer;

	void BaseAwake();
	void BaseStart();

	void SetGameObject(const GamePointer<GameObject>& aGameObject)	
	{ 
		myGameObject = aGameObject; 
	}

	void SetTransform(const GamePointer<Transform>& aTransform)		
	{ 
		myTransform = aTransform; 
	}

	void SetID(int aID);

	virtual int GetUpdatePriority() const;

	void BaseOnEnable();
	void BaseOnDisable();
	void BaseOnDrawGizmos();
};

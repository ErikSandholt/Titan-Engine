#include "Component.h"
#include <Engine/Utils_Misc/BitMask.h>
#include "Engine/ComponentSystem/Base/IComponentList.h"
#include "Engine/ComponentSystem/GameObject.h"

/// <summary>
/// Get the gameobject the component is attached to.
/// </summary>
/// <returns></returns>
const GamePointer<GameObject>& Component::GetGameObject() const
{
	return myGameObject;
}

/// <summary>
/// Get the Transform the gameobject has.
/// </summary>
/// <returns></returns>
const GamePointer<Transform>& Component::GetTransform() const
{
	return myTransform;
}

/// <summary>
///  Set components activity. It will only update when active. Components start active by default.
/// </summary>
/// <param name="aIsEnabled"></param>
void Component::SetEnable(const bool aIsEnabled)
{
	if (aIsEnabled == BitMask::GetBit(myComponentState, C_State::Enabled)) return;
	if (BitMask::GetBit(myComponentState, C_State::Hierarchy) == false)
	{
		BitMask::SetBit(myComponentState, C_State::Enabled, aIsEnabled);
		return;
	}

	if (aIsEnabled)
	{
		//Needs to be set before calling
		BitMask::SetBit(myComponentState, C_State::Enabled, aIsEnabled);
		BaseOnEnable();
	}
	else
	{
		BaseOnDisable();
	}

	BitMask::SetBit(myComponentState, C_State::Enabled, aIsEnabled);
}

/// <summary>
///  Get components state.
/// </summary>
/// <returns></returns>
bool Component::GetEnable() const
{
	return BitMask::GetBit(myComponentState, C_State::Enabled);
}

bool Component::GetDirty() const
{
	return GetCompState(C_State::Dirty);
}

/// <summary>
///  Get components active state.
/// </summary>
/// <returns></returns>
bool Component::GetActive() const
{
	return	BitMask::GetBit(myComponentState, C_State::Enabled)		&&
			BitMask::GetBit(myComponentState, C_State::Hierarchy)	&&
			BitMask::GetBit(myComponentState, C_State::Awakened)	&&
			!BitMask::GetBit(myComponentState, C_State::Destroying);
}

/// <summary>
/// To delete a component; call Destroy, and it will be destroyed at the end of the frame.
/// </summary>
void Component::Destroy() const
{
	reinterpret_cast<IComponentList*>(myPointer.GetList())->Destroy(myPointer.GetKeyIndex(), myPointer.GetKeyGeneration());
}

/// <summary>
/// OnEnable: When the component goes from inactive to active this function will be called.
/// </summary>
void Component::OnEnable() { __noop; }
/// <summary>
/// OnDisable: When the component goes from active to inactive this function will be called.
/// </summary>
void Component::OnDisable() { __noop; }
/// <summary>
/// OnDestroy: Is similar to the deconstructor. Here you can add any delete pointer the Class oversees.
/// </summary>
void Component::OnDestroy() { __noop; }
/// <summary>
/// OnDrawGizmo: While the editor is running all components will run the code OnDrawGizmo which will be used for drawing lines or shapes mostly.
/// </summary>
void Component::OnDrawGizmos() { __noop; }
/// <summary>
/// OnSelectedGizmo: When the editor selects a gameobject all its components will call OnSelectedGizmo function. Which will be used for drawing lines or shapes mostly.
/// </summary>
void Component::OnDrawGizmosSelected() { __noop; }

/// <summary>
/// Initialize will set Default values on the component
/// </summary>
void Component::Initialize() { __noop; }

/// <summary>
/// Awake: Is similar to the constructor, it will be called after the component is created. Here you can set any values in your variables.
/// </summary>
void Component::Awake() { __noop; }
/// <summary>
/// Start: Will be called once at the start of the next frame.
/// </summary>
void Component::Start() { __noop; }
/// <summary>
/// Update: Is simply update that is called once every frame per component.
/// </summary>
void Component::Update() { __noop; }
/// <summary>
/// LateUpdate: Is similar to update but is called after all update calls has been called.
/// </summary>
void Component::LateUpdate() { __noop; }

void Component::OnCollisionEnter(GamePointer<GameObject> other) { __noop; }

void Component::OnCollisionStay(GamePointer<GameObject> other) { __noop; }

void Component::OnCollisionExit(GamePointer<GameObject> other) { __noop; }

void Component::OnTriggerEnter(GamePointer<GameObject> other) { __noop; }

void Component::OnTriggerExit(GamePointer<GameObject> other) { __noop; }

bool Component::GetCompState(C_State aState) const
{
	return BitMask::GetBit(myComponentState, aState);
}

void Component::SetCompState(C_State aState, bool aFlag)
{
	BitMask::SetBit(myComponentState, aState, aFlag);
}

void Component::BaseAwake()
{
	BitMask::SetBit(myComponentState, Component::C_State::Awakened, true);
	Awake();
}

void Component::BaseStart()
{
	BitMask::SetBit(myComponentState, Component::C_State::Started, true);
	Start();

	BaseOnEnable();
}

void Component::SetID(const int aID)
{
	myID = aID;
}

int Component::GetUpdatePriority() const
{
	return 0;
}

void Component::BaseOnEnable()
{

	bool callOnEnable =		 BitMask::GetBit(myComponentState, C_State::Enabled)	&&
							 BitMask::GetBit(myComponentState, C_State::Hierarchy)	&&
							 BitMask::GetBit(myComponentState, C_State::Awakened)	&&
							!BitMask::GetBit(myComponentState, C_State::Destroying);

	if(callOnEnable) OnEnable();
}

void Component::BaseOnDisable()
{
	bool callOnDisable =	 BitMask::GetBit(myComponentState, C_State::Enabled)	&&
							 BitMask::GetBit(myComponentState, C_State::Hierarchy)	&&
							 BitMask::GetBit(myComponentState, C_State::Awakened)	&&
							!BitMask::GetBit(myComponentState, C_State::Destroying);

	if(callOnDisable) OnDisable();
}

void Component::BaseOnDrawGizmos()
{
	bool callDrawGizmos =	 BitMask::GetBit(myComponentState, C_State::Enabled)	&&
							 BitMask::GetBit(myComponentState, C_State::Hierarchy)	&&
							 BitMask::GetBit(myComponentState, C_State::Awakened)	&&
							!BitMask::GetBit(myComponentState, C_State::Destroying);

	if(callDrawGizmos) OnDrawGizmos();
}



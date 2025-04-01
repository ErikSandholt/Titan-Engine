#include "GameObject.h"
#include <typeinfo>
#include "GameObjectManager.h"
#include <Engine/Utils_Misc/BitMask.h>
#include "Engine/Debug/Debug_Log.h"

std::string GameObject::GetTypeName() const
{
     return typeid(GameObject).name(); 
}

const Sunder::CallEvent GameObject::GetFunction(const std::string& aFunctionName) const
{
    Sunder::CallEvent call = *FunctionCallManager::GetCallEvent(
        typeid(GameObject).name(),
        aFunctionName);
    call.Wrap(0, FindGameObjectByID(myID));
    return call;
}

/// <summary>
/// Will destroy the gameobject while calling destroy on every child gameobject and component attached to it.
/// </summary>
void GameObject::Destroy() const
{
    GameObjectManager::Destroy( myPointer );
}

int GameObject::GetID() const
{
    return myID;
}

const std::string& GameObject::GetName() const
{
    return myName;
}

bool GameObject::SendMessage(void* aMessage, unsigned int aChannel)
{
    
    int size    = static_cast<int>(myComponents.size());
    int passes  = 0;

    for (int i = size - 1; 0 <= i; --i)
    {
        if (myComponents[i]->SendMessage(aMessage, aChannel))
        {
            ++passes;
        }
    }

    if (size != passes) return false;

    std::vector< GamePointer<GameObject> > children;
    GetChildGameObjects(children);

    for (auto& child : children)
    {
        if ( GameObject* go = child.Get() ) go->SendMessage(aMessage, aChannel);
    }

    return true;
}

void GameObject::SetName(const std::string& aName)
{
    myName = aName;
}

void GameObject::SetStatic(bool aIsStatic)
{
    BitMask::SetBit(myGameObjectState, GO_State::Static, aIsStatic);
}

bool GameObject::GetIsStatic() const
{
    return BitMask::GetBit(myGameObjectState, GO_State::Static);
}

std::string GameObject::GetTag() const
{
    return myTag;
}

void GameObject::SetTag(const std::string& aTag)
{
    myTag = aTag;
}

ELayers GameObject::GetLayer() const
{
    return myLayer;
}

void GameObject::SetLayer(ELayers aLayer)
{
    myLayer = aLayer;
}

/// <summary>
/// Setting the gameobject activity to false will cause all gameobjects and components attached to it to also be deactivated until this is activated.
/// </summary>
/// <param name="aIsActive"></param>
void GameObject::SetEnable(const bool aIsActive)
{
    unsigned int previous = myGameObjectState;
    BitMask::SetBit(myGameObjectState, GO_State::Enable, aIsActive);

    if (previous == myGameObjectState) return; //Nothing changed


    myTransform->SetEnable(aIsActive);

    //Component* component;
    //const size_t size = myComponents.size();
    //if (aIsActive)
    //{
    //    for (size_t i = 0; i < size; i++)
    //    {
    //        component = myComponents[i].Get();

    //        BitMask::SetBit(component->myComponentState, Component::C_State::Hierarchy, true);
    //        if(component->GetEnable())
    //            component->OnEnable();
    //    }
    //}
    //else
    //{
    //    for (size_t i = 0; i < size; i++)
    //    {
    //        component = myComponents[i].Get();

    //        BitMask::SetBit(component->myComponentState, Component::C_State::Hierarchy, false);
    //        if (component->GetEnable())
    //            component->OnDisable();
    //    }
    //}

    myTransform->UpdateHierarchy();

}

bool GameObject::GetEnable() const
{
    return BitMask::GetBit(myGameObjectState, GO_State::Enable);
    
}

bool GameObject::GetActive() const
{
    return  BitMask::GetBit(myGameObjectState, GO_State::Enable) &&
            BitMask::GetBit(myGameObjectState, GO_State::ParentHierarchy) &&
            !BitMask::GetBit(myGameObjectState, GO_State::Destroy);
}

/// <summary>
/// Get Transform component that handles position, rotation, scale and any other attached gameobject.
/// </summary>
/// <returns></returns>
const GamePointer<Transform>& GameObject::GetTransform() const
{
    return myTransform;
}

const GamePointer<GameObject>& GameObject::GetGameObject() const
{
    return myPointer;//GameObjectManager::GetGameObject(myID);
}

/// <summary>
/// Creates an empty gameobject with a Transform component and returns it.
/// </summary>
/// <returns></returns>
GamePointer<GameObject> GameObject::Create()
{
    auto go = GameObjectManager::Create();
    go->AddComponent<Transform>();
    return go;
}

GamePointer<GameObject> GameObject::Create(int aID, int aTransformID)
{
    auto go = GameObjectManager::Create(aID);
    go->AddComponent<Transform>(aTransformID);
    return go;
}

void GameObject::SetHierarchyActivity(bool aIsActive)
{
    unsigned int previousState = myGameObjectState;
    BitMask::SetBit(myGameObjectState, GO_State::ParentHierarchy, aIsActive);

    if (previousState == myGameObjectState) return; //Nothing changed

    Component* component = nullptr;
    size_t size = myComponents.size();
    if (aIsActive)
    {
        for (size_t i = 0; i < size; ++i)
        {
            component = myComponents[i].Get();
            if (component->GetEnable()) component->OnEnable();
                
            BitMask::SetBit(component->myComponentState, Component::C_State::Hierarchy, true);
            component->BaseOnEnable();
        }
    }
    else
    {
        for (size_t i = 0; i < size; ++i)
        {
            component = myComponents[i].Get();
            if (component->GetEnable()) component->OnDisable();
                
            component->BaseOnDisable();
            BitMask::SetBit(component->myComponentState, Component::C_State::Hierarchy, false);
        }
    }
}

void GameObject::SetTransform(const GamePointer<Transform>& aTransform)
{
    myTransform = aTransform;
}


void GameObject::SetID(int aID)
{
    myID = aID;
}

void GameObject::GetChildGameObjects(std::vector<GamePointer<GameObject> >& aOutList) const
{
    for (int i = 0; i < myTransform->GetChildrenAmount(); ++i)
    {
        auto child = myTransform->GetChild(i)->GetGameObject();
        aOutList.push_back(child);
        child->GetChildGameObjects(aOutList);
    }
}

void GameObject::AppendComponents(const GamePointer<Component>* someComponents, int aSize)
{
    size_t size = myComponents.size();
    myComponents.resize(size + aSize);

    _memccpy(myComponents.data() + size, someComponents, 0, aSize * sizeof(GamePointer<Component>));
}

void GameObject::RemoveComponent(int aID)
{
    auto find = myComponents.begin();
    while (find != myComponents.end())
    {
        if (find->Get()->GetID() == aID)
        {

            myComponents.erase(find);
            return;
        }
        ++find;
    }
}

std::vector < GamePointer<GameObject> > FindGameObjects()
{
    return GameObjectManager::GetList();
}

std::vector < GamePointer<GameObject> > FindGameObjects(const CU::Vector3f& aPosition, float aRange)
{
    std::vector < GamePointer<GameObject> > allGO = GameObjectManager::GetList();
    std::vector < GamePointer<GameObject> > closeGO;

    for (auto& go : allGO)
    {
        auto transform = go->GetTransform();
        auto distance = transform->GetPosition() - aPosition;

        auto absolut = distance.LengthSq();

        if (absolut == 0.0f || sqrtf(absolut) <= aRange) closeGO.push_back(go);

    }

    return closeGO;
}

GamePointer<GameObject> FindGameObjectByID(const int& aID)
{
    return GameObjectManager::GetGameObject(aID);
}

GamePointer<GameObject> FindGameObjectByName(const std::string& aName)
{
    return GameObjectManager::GetGameObject(aName);
}

std::vector<GamePointer<GameObject>> FindObjectsByTag(const std::string& aTag)
{
    return GameObjectManager::FindGameObjectsByTag(aTag);
}

GamePointer<GameSystem::Object> FindObject(const std::string& aType, int aID)
{
    GamePointer<GameSystem::Object> obj;
    obj = GameObjectManager::FindObject(aType, aID);
    if (obj) return obj;
    obj = ComponentListManager::FindObject(aType, aID);
    if (obj) return obj;

    return GamePointer<GameSystem::Object>();
}
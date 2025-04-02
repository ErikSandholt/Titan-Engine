# Component System

The system is inspired by **Unity Engine**

## Component

_Inherits from Object_

Provides a custom Component a Base to integrate and communicate with the system.

## GameObject

_Inherits from Object_

Serves as a container for components allowing components to communicate with each other that are on the gameobject or it transforms children. 

The state of the gameobject will determine the state of the components. Such as enable, disable, destroy.

## Slot_Manager

As the Slot_Map will move data immediately when modified the manager will contain buffers for when the Slot_Map is full and removing entities. 

At the end of the frame the manager will replace destroyed entities with the new ones if any, grow the map if needed and add them. (See Memory Management)

## ComponentList

_Inherits from IComponentList, Slot_Manager_

Serves as a layer to manage component type in the system so as to not move memory when updating components is progress which could cause memory to be invalid. 

## ComponentListManager

A manage of ComponentLists were function calls for a type can search each component list. (Such as FindComponents<T>)

## GameObjectManager

_Inherits from IGameDataList, Slot_Manager_

Serves as a layer to manage gameobject in the system so as to not move memory when updating components is progress which could cause memory to be invalid. 

## GamePointer

Contains a Key for a Game entity such as GameObject or component in a IGameDataList. The list will use the key to retrieve an Entity.

The index of a Key inside a Slot_Map and the generation of the key. If they don’t match, the key is invalid.

## Interfaces

### Object

This allows us to type check an entity for casting purposes.

### IGameDataList

This interface allows the system to check each list for entities such as an interface and receive a list of entities from different lists.


### IComponentList

_Inherits from IGameDataList_

This interface provides functions for components such as Update that will iterate through a Slot_Map data to update all active components.



# Memory Management

## AssetPointer<T>

Reference to a Asset inside a AssetManager by ID.

For each AssetPointer that refers to an Asset the counter of that Asset will increase If this reaches 0 the asset will be destroyed.

The assetPointer gave us an easier way to manage the asset and understand which asset it’s referring to.

## GamePointer<T>

Contains a Key for a Game entity such as GameObject or component in a IGameDataList. The list will use the key to retrieve an Entity.

The index of a Key inside a Slot_Map and the generation of the key. If they don’t match, the key is invalid.

## Abstract_AssetManager<T>

This creates a base for AssetManagers to be built on allowing developers to inherit it and it will handle when an Asset is no longer being referenced to.

The decision was made to avoid SharedPointers because the AssetManager should be able to handle the cleanup of Assets allowing us to make permanent assets.

## Slot_Map<T>

Contains three arrays T data, keysToData, indexToKey. 

When data is removed from an array of T the last entity in the array is moved to its index, clear the key that refers to the index and update the last data’s key and indexToKey to the new location. 

Adding data when the Slot_Map is full it will expand by 2 * size and move the memory.

This makes sure the Keys are still valid and memory can be ordered by active entities.

## Slot_Manager<T>

As the Slot_Map will move data immediately when modified the manager will contain buffers for when the Slot_Map is full and removing entities. At the end of the frame the manager will replace destroyed entities with the new ones if any, grow the map if needed and add them.

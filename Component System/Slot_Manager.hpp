#pragma once
#include <vector>
#include <unordered_map>
#include <Engine/Utils_Misc/Slot_Map.h>

template<typename T>
class Slot_Manager
{
public:

	Key CreateEntity();
	std::vector<Key> AppendEntitys(T* aSource, int aAmount);
	void DestroyEntity(const Key& aKey);

	virtual void EntityDeconstruction(T& aEntity)	= 0;


	//Use a struct with a operator(T& aEntity) that returns bool
	template <typename X>
	Key FindEntity(const X& aFindFunction) const;

	//Use a struct with a operator(T& aEntity) that returns bool
	template <typename X>
	std::vector<Key> FindEntitys(const X& aFindFunction) const;

	T* GetEntity(const Key& aKey) const;
	Key GetEntityKey(int aDataIndex) const;
	//Including Main and buffer
	int GetTotalCount() const;


	//Return first entity in main
	T* GetData();
	int GetMainCount() const;


	void GrowToContainEntitys(int aSize);

	void EntityCleanup();
	void EntityMerge();

protected:


private:
	struct BufferData
	{
		int			Generation = 0;
		T Entity;
	};
	enum class SlotMapType
	{
		Entity,
		Buffer
	};


	Slot_Map<T>								myEntitys;
	//Key Index, { Generation, Data }
	std::unordered_map<int, BufferData>		myBuffer;
	std::vector<Key>						myErase;
	std::vector<SlotMapType>				myManagerKeySlotMap;
};

template<typename T>
inline Key Slot_Manager<T>::CreateEntity()
{
	Key key;
	T* entity = nullptr;

	//If a entity is queued for erase
	if (0 < myErase.size())
	{
		key = myErase.back();
		SlotMapType type	= myManagerKeySlotMap.at(key.Index);

		switch (type)
		{
		case SlotMapType::Entity:
		{
			entity			= &myEntitys.Get(key);
			EntityDeconstruction(*entity);

			key				= myEntitys.SetGeneration(key, key.Generation + 1);
			break;
		}
		case SlotMapType::Buffer:
		{
			auto& bufferIndex		= myBuffer.at(key.Index);
			entity					= &bufferIndex.Entity;
			EntityDeconstruction(*entity);

			bufferIndex.Generation += 1;

			//If buffer has a reserved object in components
			if (key.Index < myEntitys.GetCapacity())
			{
				myEntitys.SetGeneration(key, key.Generation + 1);
			}

			++key.Generation;
			break;
		}
		default:
			break;
		}

		entity->~T();
		new (entity) T();
		myErase.pop_back();
	}

	if (entity == nullptr &&
		myEntitys.GetSize() < myEntitys.GetCapacity())
	{
		key					= myEntitys.Create();
		entity				= &myEntitys.Get(key);
	}

	if (entity == nullptr)
	{
		int size			= myEntitys.GetCapacity() + static_cast<int>(myBuffer.size());
		key.Index			= size;
		key.Generation		= 0;
		BufferData& data	= myBuffer[key.Index];
		data.Generation		= 0;
		entity				= &data.Entity;

		myManagerKeySlotMap.resize(size + 1);
		myManagerKeySlotMap.at(size) = SlotMapType::Buffer;
	}

	return key;
}

template<typename T>
inline std::vector<Key> Slot_Manager<T>::AppendEntitys(T* aSource, int aAmount)
{
	int index			= 0;
	int remaining		= aAmount;
	std::vector<Key>	list(aAmount);
	Key*				listPtr = list.data();

	//Replace entitys marked for delete
	int containerSize	= std::min(static_cast<int>(myErase.size()), remaining);
	remaining -= containerSize;
	for (size_t i = 0; i < containerSize; ++i)
	{
		listPtr[index] = CreateEntity();
		*GetEntity(listPtr[index]) = std::move(aSource[index]);
		++index;
	}

	if (remaining < 1) return list;

	//Add to Entitys
	containerSize = std::min(myEntitys.GetCapacity() - myEntitys.GetSize(), remaining);
	remaining -= containerSize;

	std::vector<Key> newKeys = myEntitys.Append(aSource + index, containerSize);
	Key* newKeysPtr = newKeys.data();
	for (size_t i = 0; i < containerSize; ++i)
	{
		listPtr[index] = newKeysPtr[i];
		++index;
	}

	if (remaining < 1) return list;

	//Add rest to buffer
	for (size_t i = 0; i < remaining; ++i)
	{
		listPtr[index] = CreateEntity();
		*GetEntity(listPtr[index]) = std::move(aSource[index]);
		++index;
	}

	return list;
}

template<typename T>
inline void Slot_Manager<T>::DestroyEntity(const Key& aKey)
{
	if (aKey.Index < 0) return;

	//TODO: Get a faster sort
	auto insert = myErase.begin();
	Key* start = myErase.data();
	for (int i = 0; i < myErase.size(); i++)
	{
		if (aKey.Index < start[i].Index)
		{
			++insert;
			continue;
		}

		if (aKey.Index		== start[i].Index		&&
			aKey.Generation <= start[i].Generation		)
		{
			return;
		}

		break;
	}

	if (insert != myErase.end() && 
		insert->Index == aKey.Index)
	{
		insert->Generation = aKey.Generation;
		return;
	}

	myErase.insert(insert, aKey);
}

template<typename T>
inline T* Slot_Manager<T>::GetEntity(const Key& aKey) const
{
	if (myManagerKeySlotMap.size() <= aKey.Index) return nullptr;

	SlotMapType type = myManagerKeySlotMap.at(aKey.Index);

	switch (type)
	{
	case SlotMapType::Entity:
	{
		return myEntitys.HasKey(aKey) ? &myEntitys.Get(aKey) : nullptr;
	}
	case SlotMapType::Buffer:
	{
		auto find = myBuffer.find(aKey.Index);
		return find != myBuffer.end() ? const_cast<T*>(&find->second.Entity) : nullptr;
	}
	default:
		break;
	}

	return nullptr;
}

template<typename T>
inline int Slot_Manager<T>::GetTotalCount() const
{
	return myEntitys.GetSize() + static_cast<int>( myBuffer.size() );	//Buffer can be part of myEntitys
}

template<typename T>
inline Key Slot_Manager<T>::GetEntityKey(int aDataIndex) const
{
	assert(-1 < aDataIndex);

	int size = myEntitys.GetSize();

	Key key;


	if (aDataIndex < size)
	{
		key = myEntitys.GetKey(aDataIndex);

		if (myManagerKeySlotMap[key.Index] == SlotMapType::Buffer)
		{
			key.Generation = myBuffer.at(key.Index).Generation;;
		}

		return key;

	}

	int index			= aDataIndex - size;
	if (-1 < index && index < myBuffer.size())
	{
		auto bufferIT	= myBuffer.begin();

		for (int i = 0; i < index; ++i) { ++bufferIT; }

		key.Index		= bufferIT->first;
		key.Generation	= bufferIT->second.Generation;

		return key;
	}

	return Key();
}

template<typename T>
inline T* Slot_Manager<T>::GetData()
{
	return &myEntitys[0];
}

template<typename T>
inline int Slot_Manager<T>::GetMainCount() const
{
	return myEntitys.GetSize();
}

template<typename T>
inline void Slot_Manager<T>::GrowToContainEntitys(int aSize)
{
	int growSize = myEntitys.GetSize() + static_cast<int>(myBuffer.size()) + aSize;
	int oldCapacity = static_cast<int>(myEntitys.GetCapacity());

	if (aSize		<= 0			) return;
	if (growSize	<= oldCapacity	) return;

	myEntitys.GrowToContain(growSize);
	myManagerKeySlotMap.resize(myEntitys.GetCapacity());


	auto it = myBuffer.begin();
	while (it != myBuffer.end())
	{
		int index = it->first;
		if (oldCapacity <= index)
		{
			myEntitys.Insert(index);
		}

		++it;
	}
}

template<typename T>
inline void Slot_Manager<T>::EntityCleanup()
{
	if (myErase.size() == 0) return;

	//Erase should be ordered from greater to lesser

	Key tempKey;
	auto it = myErase.begin();

	while (it != myErase.end())
	{
		tempKey = *it;

		SlotMapType& type = myManagerKeySlotMap[tempKey.Index];

		switch (type)
		{
		case SlotMapType::Entity:
		{
			EntityDeconstruction(myEntitys.Get(tempKey));

			myEntitys.Erase(tempKey);
			break;
		}

		case SlotMapType::Buffer:
		{
			EntityDeconstruction(myBuffer.at(tempKey.Index).Entity);

			//The component list has grown and has reserved a object
			if (tempKey.Index < myEntitys.GetCapacity())
			{
				myEntitys.Erase(tempKey);
			}

			myBuffer.erase(tempKey.Index);
			break;
		}
		default:
			break;
		}

		type	= SlotMapType::Entity;
		it		= myErase.erase(it);
	}

	assert(myErase.size() == 0 && "Memory leak. As some entity's were not handled");
}

template<typename T>
inline void Slot_Manager<T>::EntityMerge()
{
	int bufferSize = static_cast<int>(myBuffer.size());

	if (bufferSize == 0) return;

	//int size = myEntitys.GetSize();

	int growSize = static_cast<int>(myManagerKeySlotMap.size());

	myEntitys.GrowToContain(growSize);
	if (growSize < myEntitys.GetCapacity())
	{
		myManagerKeySlotMap.resize(myEntitys.GetCapacity());
	}

	//GameObject**	temp = nullptr;
	Key	key;

	auto bufferIT = myBuffer.begin();

	while (bufferIT != myBuffer.end())
	{
		key.Index		= bufferIT->first;
		key.Generation	= bufferIT->second.Generation;

		Key tempKey = myEntitys.Insert(key.Index);
		myEntitys.SetGeneration(tempKey, key.Generation);

		T& entity = myEntitys.Get(key);
		T& data = bufferIT->second.Entity;

		entity = std::move(data);


		myManagerKeySlotMap.at(key.Index) = SlotMapType::Entity;
		++bufferIT;
	}


	myBuffer.clear();
}

template<typename T>
template<typename X>
inline Key Slot_Manager<T>::FindEntity(const X& aFindFunction) const
{
	int size = GetTotalCount();

	for (int i = 0; i < size; ++i)
	{
		Key key = GetEntityKey(i);

		if (aFindFunction( *GetEntity(key) ) ) return key;
	}

	return Key();
}

template<typename T>
template<typename X>
inline std::vector<Key> Slot_Manager<T>::FindEntitys(const X& aFindFunction) const
{
	std::vector<Key> list;

	list.reserve(5);

	int size = GetTotalCount();

	for (int i = 0; i < size; ++i)
	{
		Key key = GetEntityKey(i);

		if ( aFindFunction( *GetEntity( key ) ) )
		{
			list.push_back(key);
		}
	}

	return list;
}

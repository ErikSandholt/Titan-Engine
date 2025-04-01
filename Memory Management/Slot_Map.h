#pragma once
#include <type_traits>
#include <vector>

#ifndef TITAN_ASSERT
#include <cassert>
#define TITAN_ASSERT(expression) assert(expression)
#endif

struct Key
{
	int Index	= -1;
	int Generation	= -1;

	Key() = default;
	Key(int aIndex, int aGeneration) : Index(aIndex), Generation(aGeneration) {}

	bool operator ==(const Key& aRhs) const
	{
		if (Index		!= aRhs.Index)		return false;
		if (Generation	!= aRhs.Generation) return false;

		return true;
	}
};

template <typename T = int>
class Slot_Map
{
	using Key_Inner = Key;
	using Key_Outer = Key;

public:
	Slot_Map() = default;

	~Slot_Map();
	

	Key_Outer SetGeneration(const Key_Outer& aKey, int aGeneration);
	

	int GetCapacity() const { return myCapacity; }
	int GetSize() const { return mySize; }
	Key_Outer GetKey(int aDataIndex) const;

	T& operator [](int aIndex) const { return myData[aIndex]; }

	template<typename X>
	Key_Outer FindKey(const X& aSearch) const;

	void Clear();

	bool HasKey(const Key_Outer& aKey) const;

	T& Get(Key_Outer aKey) const;

	T& Get(int aIndex, int aGeneration) const;

	Key_Outer Insert(int aIndex);

	Key_Outer Create();

	Key_Outer Create(T& aValue);

	std::vector<Key_Outer> Append(T* aSource, int aSize);

	void Erase(Key_Outer aKey);

	void Erase(int aDataIndex);

	void Reserve(int someSize);

	void GrowToContain(int someSize);

private:
	int myFreeKeyHead		= -1;
	int myFreeKeyTail		= -1;
	int myCapacity			=  0;
	int mySize			=  0;

	Key_Inner* myKeys		= nullptr;
	T* myData				= nullptr;
	int* myDataKeyIndex		= nullptr;

	//Debug
	struct DebugData
	{
		Key_Inner InnerKey;
		Key_Outer OuterKey;
		T* Data;
	};
	std::vector<DebugData> myDebugData;
	//End Debug

	int CalculateGrowth() const
	{
		return myCapacity * 2 + 1;
	}

	template<typename TValue>
	void MoveList(TValue* aDestination, TValue* aSource)
	{
		for (int i = 0; i < myCapacity; ++i)
		{
			new (&aDestination[i]) TValue( std::move(aSource[i]) );
		}
	}

	Key_Outer CreateOKey_ByData(int aDataIndex) const
	{
		return CreateOKey_ByKey( myDataKeyIndex[aDataIndex] );
	}

	Key_Outer CreateOKey_ByKey(int aOuterKeyIndex) const
	{
		TITAN_ASSERT(-1 < aOuterKeyIndex && "OuterKeyIndex is invalid");
		return { aOuterKeyIndex, myKeys[aOuterKeyIndex].Generation };
	}

	Key_Inner CreateIKey_ByData(int aDataIndex) const
	{
		TITAN_ASSERT(-1 < aDataIndex && "DataIndex is invalid");
		int index = myDataKeyIndex[aDataIndex];

		TITAN_ASSERT(-1 < index && "Index is invalid");
		return myKeys[index];
	}

	Key_Inner CreateIKey_ByKey(int aOuterKeyIndex) const
	{
		TITAN_ASSERT(-1 < aOuterKeyIndex && "KeyIndex is invalid");
		return myKeys[aOuterKeyIndex];
	}

	Key_Inner& GetIKey_ByData(int aDataIndex) const
	{
		TITAN_ASSERT(-1 < aDataIndex && "DataIndex is invalid");

		int index = myDataKeyIndex[aDataIndex];

		TITAN_ASSERT(-1 < index && "Index is invalid");
		return myKeys[index];
	}

	Key_Inner& GetIKey_ByKey(int aOuterKeyIndex) const
	{
		TITAN_ASSERT(-1 < aOuterKeyIndex && "OuterKeyIndex is invalid");
		return myKeys[aOuterKeyIndex];
	}


	void UpdateHeadAndTail()
	{
		for (; myFreeKeyHead < myFreeKeyTail; ++myFreeKeyHead)
		{
			if (myKeys[myFreeKeyHead].Index == -1) break;
		}

		//Update Tail
		for (; myFreeKeyHead < myFreeKeyTail; --myFreeKeyTail)
		{
			if (myKeys[myFreeKeyTail].Index == -1) break;
		}

		if (myFreeKeyHead == myFreeKeyTail &&
			myFreeKeyHead == -1 ||
			-1 < myKeys[myFreeKeyHead].Index)
		{
			myFreeKeyHead = -1;
			myFreeKeyTail = -1;
		}
	}

	void CreateSpace()
	{
		TryExpandList();
		UpdateHeadAndTail();
	}

	void TryExpandList() 
	{
		if (mySize < myCapacity) return;
		int newCapacity = myCapacity + CalculateGrowth();
		GrowList(newCapacity);
	}

	void GrowList(int someSize) 
	{
		const int keySize		= sizeof(Key)	* someSize;
		const int objectSize	= sizeof(T)		* someSize;
		const int eraseSize		= sizeof(int)	* someSize;

		int allocateSize	= keySize + objectSize + eraseSize;
		void* newList		= std::malloc(allocateSize);

		if (newList == nullptr) throw std::bad_alloc();

		if(myKeys)
		{
			void* moveList = newList;
			MoveList<Key>(static_cast<Key*>(moveList), myKeys);


			moveList = static_cast<char*>(newList) + keySize;

			T* dataList = reinterpret_cast<T*>(moveList);
			for (int i = 0; i < mySize; ++i)
			{	
				//If the key generation is -1 then the data was never used
				new (&dataList[i]) T(std::move( myData[i] ));
			}


			moveList = static_cast<char*>(newList) + keySize + objectSize;
			MoveList<int>(static_cast<int*>(moveList), myDataKeyIndex);
			std::free(myKeys);
		}

		myKeys			= static_cast<Key*>	(newList);
		myData			= reinterpret_cast<T*>		(static_cast<char*>(newList) + keySize);
		myDataKeyIndex	= reinterpret_cast<int*>	(static_cast<char*>(newList) + keySize + objectSize);

		//Set default Values
		Key defaultKey = Key();
		for (int i = myCapacity; i < someSize; ++i)
		{
			myKeys[i]			= defaultKey;
			myDataKeyIndex[i]	= -1;
		}

		myCapacity		= someSize;
		myFreeKeyHead	= myFreeKeyHead < 0 ? mySize : myFreeKeyHead;
		myFreeKeyTail	= myCapacity - 1;

		Update_Debug();
	}

	void Update_Debug()
	{
		__noop;
		/*myDebugData.clear();
		for (int i = 0; i < mySize; ++i)
		{
			myDebugData.push_back(GetDebugData(i));
		}*/
	}

	DebugData GetDebugData(int aDataIndex)
	{
		Key_Inner iKey = GetIKey_ByData(aDataIndex);
		Key_Outer oKey = CreateOKey_ByData(aDataIndex);
		return { iKey, oKey, &myData[iKey.Index] };
	}

};

template<typename T>
Slot_Map<T>::~Slot_Map()
{
	if (myKeys)
	{
		Clear();
		std::free(myKeys);
	}

	myData = nullptr;
	myDataKeyIndex = nullptr;
	myKeys = nullptr;

	mySize = 0;
	myCapacity = 0;
	myFreeKeyHead = 0;
	myFreeKeyTail = 0;
}

template<typename T>
inline Key Slot_Map<T>::SetGeneration(const Key_Outer& aKey, int aGeneration)
{
	if (HasKey(aKey) == false) throw;

	Key_Inner& key = GetIKey_ByKey(aKey.Index);

	key.Generation = aGeneration;
	Update_Debug();
	return CreateOKey_ByKey(aKey.Index);
}

template<typename T>
 Key Slot_Map<T>::GetKey(int aDataIndex) const
{
	if (aDataIndex < 0 || mySize <= aDataIndex) return {};

	return CreateOKey_ByData(aDataIndex);
}

template<typename T>
 void Slot_Map<T>::Clear()
{
	auto defaultKey = Key();
	for (size_t i = 0; i < mySize; i++)
	{
		myData[i].~T();
		myDataKeyIndex[i]	= -1;
		myKeys[i]		= defaultKey;
	}

	mySize			= 0;
	myFreeKeyHead		= 0;
	myFreeKeyTail		= myCapacity;

	Update_Debug();
}

template<typename T>
 bool Slot_Map<T>::HasKey(const Key_Outer& aKey) const
{
	if (aKey.Index < 0)							return false;
	if (myCapacity <= aKey.Index)						return false;

	const Key_Inner& innerKey = GetIKey_ByKey(aKey.Index);
	if (innerKey.Index == -1 || aKey.Generation != innerKey.Generation)	return false;

	return true;
}

template<typename T>
 T& Slot_Map<T>::Get(Key_Outer aKey) const
{
	if (HasKey(aKey) == false) throw;

	int dataIndex = myKeys[aKey.Index].Index;

	return myData[dataIndex];
}

template<typename T>
 T& Slot_Map<T>::Get(int aIndex, int aGeneration) const
{
	if (HasKey({ aIndex, aGeneration }) == false) throw;

	int dataIndex = myKeys[aIndex].Index;

	return myData[dataIndex];
}

template<typename T>
inline Key Slot_Map<T>::Insert(int aIndex)
{
	//This is in our best intrest to not call
	TITAN_ASSERT(aIndex < myCapacity);

	Key_Inner& indices = GetIKey_ByKey(aIndex);


	++indices.Generation;

	if (indices.Index == -1)
	{
		myDataKeyIndex[mySize] = aIndex;
		indices.Index = mySize;
		new (&myData[mySize]) T();
		++mySize;
	}
	else
	{
		myData[indices.Index].~T();
		new (&myData[indices.Index]) T();
	}


	UpdateHeadAndTail();

	Update_Debug();

	return CreateOKey_ByKey(aIndex);
}

template<typename T>
inline Key Slot_Map<T>::Create()
{
	CreateSpace();
	int index = myFreeKeyHead;

	Key_Inner& indices		= GetIKey_ByKey(index); 
	indices.Index			= mySize;
	++indices.Generation;
	myDataKeyIndex[mySize]	= index;

	new (&myData[mySize]) T();

	UpdateHeadAndTail();

	++mySize;

	Update_Debug();

	return CreateOKey_ByKey(index);
}

template<typename T>
 Key Slot_Map<T>::Create(T& aValue)
{
	CreateSpace();
	int index = myFreeKeyHead;


	Key_Inner& indices = GetIKey_ByKey(index);
	indices.Index = mySize;
	++indices.Generation;
	myDataKeyIndex[mySize] = index;

	auto& data			= myData[indices.Index]; data;
	auto dataKeyIndex	= myDataKeyIndex[indices.Index]; dataKeyIndex;

	new (&myData[mySize]) T(aValue);

	UpdateHeadAndTail();

	++mySize;

	Update_Debug();
	
	return CreateOKey_ByKey(index);
}

 template<typename T>
 inline std::vector<Key> Slot_Map<T>::Append(T* aSource, int aSize)
 {
	 std::vector<Key_Outer> list(aSize);
	 for (size_t i = 0; i < aSize; ++i)
	 {
		 list[i] = Create(aSource[i]);
	 }

	 return list;
 }

template<typename T>
void Slot_Map<T>::Erase(Key_Outer aKey)
{
	if (HasKey(aKey) == false) return;

	Key_Inner& indices = GetIKey_ByKey(aKey.Index);


	int dataIndex = indices.Index;

	indices.Index			= -1; //Set Key to -1 to indicate unused
	myDataKeyIndex[dataIndex]	= -1;

	//if less then head Update
	myFreeKeyHead = aKey.Index < myFreeKeyHead ? aKey.Index : myFreeKeyHead;
	//if greater than tail update
	myFreeKeyTail = myFreeKeyTail < aKey.Index ? aKey.Index : myFreeKeyTail;

	--mySize;
	//Move last element to empty

	//If last Dont swap
	if (dataIndex != mySize)
	{
		Key_Inner& swapKey = GetIKey_ByData(mySize);
		swapKey.Index = dataIndex;

		std::swap(myData[dataIndex], myData[mySize]);
		std::swap(myDataKeyIndex[dataIndex], myDataKeyIndex[mySize]);
	}

	myData[mySize].~T();
	

	UpdateHeadAndTail();

	Update_Debug();
}

template<typename T>
 void Slot_Map<T>::Erase(int aDataIndex)
{
	if (aDataIndex < 0 || mySize <= aDataIndex) return;

	Erase(CreateOKey_ByData(aDataIndex));
}

template<typename T>
 void Slot_Map<T>::Reserve(int someSize)
{
	if (someSize <= myCapacity) return;

	GrowList(someSize);
}

template<typename T>
 void Slot_Map<T>::GrowToContain(int someSize)
{
	if (someSize <= myCapacity) return;
	if (someSize <= 0)			return;

	int size = myCapacity == 0 ? 1 : myCapacity;
	do
	{
		size *= 2;
	} while (size < someSize);

	GrowList(size);
}

template<typename T>
template<typename X>
 Key Slot_Map<T>::FindKey(const X& aSearch) const
{
	for (int i = 0; i < mySize; i++)
	{
		if (aSearch(myData[i])) return CreateOKey_ByData(i);

	}

	return {};
}

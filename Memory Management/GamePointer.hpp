#pragma once

#undef GetObject

class IGameDataList;

// GamePointer<T> refers to a list with a two int’s a key and generation to get the type it’s referring to.

template <typename T>
class GamePointer
{
public:
private:
#ifdef _DEBUG
	mutable T* myDebugValue = nullptr;
#endif // _DEBUG


	IGameDataList*	myList			= nullptr;
	int				myKeyIndex		= 0;
	int				myKeyGeneration = 0;

public:
	GamePointer() : myList(nullptr), myKeyIndex(-1), myKeyGeneration(-1)
	{ }

	GamePointer(IGameDataList* aList, int aIndex, int aGeneration) :
		myList(aList),
		myKeyIndex(aIndex),
		myKeyGeneration(aGeneration)
	{
#ifdef _DEBUG
		myDebugValue = myList ? dynamic_cast<T*>(myList->GetObject(myKeyIndex, myKeyGeneration)) : nullptr;
#endif
	}

	GamePointer(const GamePointer<T>& aRhs)
	{
		myList = aRhs.myList;
		myKeyIndex = aRhs.myKeyIndex;
		myKeyGeneration = aRhs.myKeyGeneration;

#ifdef _DEBUG
		myDebugValue = aRhs.myDebugValue;
#endif
	}

	int				GetKeyIndex() const			{ return myKeyIndex; }
	int				GetKeyGeneration() const	{ return myKeyGeneration; }
	IGameDataList*	GetList() const				{ return myList; }

	template<typename Y>
	GamePointer<Y> Cast() const
	{
		if(myList && dynamic_cast<const Y*>( myList->GetTypeChecker() ))
		{
			return GamePointer<Y>(myList, myKeyIndex, myKeyGeneration);
		}

		return GamePointer<Y>();
	}

	template<typename Y>
	inline GamePointer<Y> ForceCast() const
	{
		return (GamePointer<Y>)*this;//GamePointer<Y>(myList, myKeyIndex, myKeyGeneration);
	}


	template<typename Y>
	operator GamePointer<Y>() const
	{
		return Cast<Y>();
	}

	
	~GamePointer()
	{
		myList			= nullptr;
		myKeyIndex		= -1;
		myKeyGeneration = -1;

#ifdef _DEBUG
		myDebugValue	= nullptr;
#endif
	}

	/// <summary>
	/// Get a type from a list
	/// </summary>
	/// <returns></returns>
	T* Get() const
	{
		if (myList == nullptr) return nullptr;

#ifdef _DEBUG
		myDebugValue = dynamic_cast<T*>(myList->GetObject(myKeyIndex, myKeyGeneration));
		return myDebugValue;
#else

		return dynamic_cast<T*>(myList->GetObject(myKeyIndex, myKeyGeneration));
#endif // _DEBUG
	}

	T* operator ->() const
	{
		if (myList == nullptr) return nullptr;

#ifdef _DEBUG
		myDebugValue = dynamic_cast<T*>(myList->GetObject(myKeyIndex, myKeyGeneration));
		return myDebugValue;
#else

		return dynamic_cast<T*>(myList->GetObject(myKeyIndex, myKeyGeneration));
#endif // _DEBUG
	}

	/// <summary>
	/// Check if the pointer is empty.
	/// </summary>
	operator bool() const
	{
		if (myList == nullptr) return false;

		return myList->HasKey(myKeyIndex, myKeyGeneration);
	}

	template<typename X>
	bool operator ==(const GamePointer<X>& aRhs) const
	{
		//If this fails
		if (*this			== false				)	return false;
		if (aRhs			== false				)	return false;
		
		if (myList			!= aRhs.myList			)	return false;
		if (myKeyIndex		!= aRhs.myKeyIndex		)	return false;
		if (myKeyGeneration != aRhs.myKeyGeneration	)	return false;

		return true;
		//if (auto ptr = aRhs.Get()	)
		//{
		//	//Check if same
		//	return ptr == Get();
		//}

		////If aRhs fails
		//return false;
	}

	void operator =(const GamePointer<T>& aRhs)
	{
		myList = aRhs.myList;
		myKeyIndex = aRhs.myKeyIndex;
		myKeyGeneration = aRhs.myKeyGeneration;
	}

	bool operator <(const GamePointer<T>& aRhs) const
	{
		if (myKeyIndex		< aRhs.myKeyIndex		) return true;
		if (myKeyGeneration < aRhs.myKeyGeneration	) return true;
		if (myList			!= aRhs.myList			) return true;

		return false;
	}

	/*void operator =(const int& aObjectID)
	{
		this* =
		myID = aObjectID;
	}

	void operator =(const T& aObject)
	{
		myID = aObject->GetID();
	}

	void operator =(const T* aObject)
	{
		myID = aObject ? aObject->GetID() : 0;
	}*/


};

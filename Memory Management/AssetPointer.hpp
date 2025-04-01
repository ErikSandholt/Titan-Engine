#pragma once
#include <string>
#include "IAssetManager.hpp"

template<typename T>
concept IsPointer = std::is_pointer_v<T>;

template<typename T>
concept IsNotPointer = !std::is_pointer_v<T>;

template <typename T>
class AssetPointer
{
public:
	AssetPointer();

	AssetPointer(const std::string_view& aAssetName, unsigned int aID, IAssetManager<T>& aManager, T& aAsset);

	AssetPointer(const AssetPointer& aAssetPointer);

	~AssetPointer();

	T& Get() const  requires IsPointer<T>;
	T* Get() const requires IsNotPointer<T>;
	[[nodiscard]] const std::string_view& GetAssetName() const;
	[[nodiscard]] unsigned int GetID() const;

	// ReSharper disable once CppNonExplicitConversionOperator
	explicit operator bool() const;
	// this should probably be amrked explicit but breaks something because of that. todo fix, maybe. - momo
	T& operator ->() const requires IsPointer<T>;
	T* operator ->() const requires IsNotPointer<T>;

	bool operator ==(const AssetPointer& aRhs) const;

	AssetPointer& operator =(const AssetPointer& aLeftValue);

private:
	IAssetManager<T>*	myManager	= nullptr;
	T*					myAsset		= nullptr;
	unsigned int		myID		= 0;
	std::string_view	myAssetName;

	void Clear();
};

//Non pointer specific
template <typename T>
AssetPointer<T>::AssetPointer() = default;

template <typename T>
AssetPointer<
	T>::AssetPointer(const std::string_view& aAssetName, const unsigned int aID, IAssetManager<T>& aManager, T& aAsset):
	myID(aID), myAssetName(std::move(aAssetName)), myManager(&aManager), myAsset(&aAsset) {}

template <typename T>
AssetPointer<T>::AssetPointer(const AssetPointer<T>& aAssetPointer)
{
	*this = aAssetPointer;
}

template <typename T>
AssetPointer<T>::~AssetPointer()
{
	if (myManager)
	{
		myManager->Destroy(*this);
	}

	Clear();
}

template <typename T>
 T& AssetPointer<T>::Get() const requires IsPointer<T>
{ 
	return *myAsset; 
}

template <typename T>
 T* AssetPointer<T>::Get() const requires IsNotPointer<T>
{
	return myAsset;
}

template <typename T>
const std::string_view& AssetPointer<T>::GetAssetName() const { return myAssetName; }

template <typename T>
unsigned int AssetPointer<T>::GetID() const { return myID; }

/// <summary>
/// Can check if the Pointer is empty.
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
AssetPointer<T>::operator bool() const { return myManager; }

template<typename T>
 T& AssetPointer<T>::operator->() const requires IsPointer<T>
{

	return *myAsset;
}

template<typename T>
 T* AssetPointer<T>::operator->() const requires IsNotPointer<T>
{ 

	return myAsset; 
}

template <typename T>
bool AssetPointer<T>::operator==(const AssetPointer& aRhs) const
{
	return myID == aRhs.myID;
}

template <typename T>
AssetPointer<T>& AssetPointer<T>::operator=(const AssetPointer<T>& aLeftValue)
{
	if (this == &aLeftValue) return *this;

	if (myManager)
	{
		myManager->Destroy(*this);
	}

	if (aLeftValue)
	{
		aLeftValue.myManager->Copy(aLeftValue);
		myAsset		= aLeftValue.myAsset;
		myManager	= aLeftValue.myManager;
		myID		= aLeftValue.myID;
		myAssetName	= aLeftValue.myAssetName;

	}
	else
	{
		Clear();
	}

	return *this;
}

template <typename T>
void AssetPointer<T>::Clear()
{
	myAsset = nullptr;
	myManager = nullptr;
	myID = 0;
	//myAssetName = "";
}

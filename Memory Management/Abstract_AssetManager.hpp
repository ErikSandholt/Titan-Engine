#pragma once
#include <unordered_map>
#include <string>
#include <assert.h>
#include "AssetPointer.hpp"

//TKey can only be std::string or unsigned int
template <
	typename TValue,
	typename TKey = std::string,
	typename Container = struct IAssetManager<TValue>::AssetData >
class Abstract_AssetManager : protected IAssetManager<TValue>
{
public:
	Abstract_AssetManager() = default;
	virtual ~Abstract_AssetManager() = default;

protected:
	//This will create if no object currently exists
	Container& GetAsset(const TKey& aKey) const { return myAssetData[aKey]; }
	Container& GetFirst() const { return myAssetData.begin()->second; }

	unsigned int Size() const { return static_cast<unsigned int>(myAssetData.size()); }
	bool Empty() const { return myAssetData.empty(); }

	bool HasAsset(const TKey& aKey) const
	{
		auto layer = myAssetData.find(aKey);
		return layer != myAssetData.end();
	}

	Container& GetIndex(int aIndex) const
	{
		TITAN_ASSERT(aIndex < (int)Size());

		auto asset = myAssetData.begin();
		std::advance(asset, aIndex);
		return asset.second;
	}

	std::pair<const TKey, Container>& GetAssetPair(const TKey& aKey) const
	{
		return *myAssetData.find(aKey);
	}

	std::pair<const TKey, Container>& GetAssetPair(int aIndex) const
	{
		TITAN_ASSERT(aIndex < (int)Size());

		auto asset = myAssetData.begin();
		std::advance(asset, aIndex);
		return *asset;
	}

	virtual AssetPointer<TValue> CreateAssetPointer(const std::string& aAssetName, Container& aOutAssetData) const
	{
		++aOutAssetData.Count;

		IAssetManager<TValue>& manager = *const_cast<IAssetManager<TValue>*>(dynamic_cast<const IAssetManager<TValue>*>(this));

		return AssetPointer<TValue>(aAssetName, aOutAssetData.ID, manager, aOutAssetData.Asset);
	}

	void SetAssetPermanent(const TKey& aKey, bool aIsPermanent)
	{
		Container& data = GetAsset(aKey);
		data.IsPermanent = aIsPermanent;

		if (!aIsPermanent && data.Count == 0)
		{
			Erase(aKey);
		}
	}

	void Erase(const TKey& aKey)
	{
		if (!HasAsset(aKey)) return;

		auto find = myAssetData.find(aKey);
		Destroy(find->second);
		myAssetData.erase(find);
	}

	virtual void Destroy(Container& aOutAssetData) = 0;

	

	virtual void Cleanup()
	{
		auto it = myAssetData.begin();
		while (it != myAssetData.end())
		{

			if (1 < it->second.Count)
			{
				//Error message: Too high count something wasn't unregistered correctly
			}

			Destroy(it->second);
			it = myAssetData.erase(it);
		}
	}

private:
	mutable std::unordered_map<TKey, Container> myAssetData;

	// Inherited via IAssetManager
	void Copy(const AssetPointer<TValue>& aAssetPointer) override
	{
		typename std::unordered_map<TKey, Container>::iterator find;

		if constexpr (std::is_same_v<TKey, std::string>)
		{
			find = myAssetData.find(aAssetPointer.GetAssetName().data());
			//return CreateAssetPointer(find->first, find->second);
		}
		else
		{
			find = myAssetData.find(aAssetPointer.GetID());
			//return CreateAssetPointer(aAssetPointer.GetAssetName().data(), find->second);
		}

		++find->second.Count;
	}

	void Destroy(const AssetPointer<TValue>& aAssetPointer) override
	{
		typename std::unordered_map<TKey, Container>::iterator find;

		if constexpr (std::is_same_v<TKey, std::string>)
		{
			find = myAssetData.find(aAssetPointer.GetAssetName().data());
		}
		else
		{
			find = myAssetData.find(aAssetPointer.GetID());
		}

		auto& asset = find->second;

		--asset.Count;
		if (!asset.IsPermanent && asset.Count == 0)
		{
			Destroy(asset);
			myAssetData.erase(find);
		}
	}

};

//When TKey1 is a unsigned int the two first digits in ID will be the "Layer" up to 100 (0 - 99) and the second key is the id 
template <
	typename TValue,
	typename TKey1 = std::string, //OR unsigned int Only
	typename TKey2 = unsigned int,
	typename Container = struct IAssetManager<TValue>::AssetData >
class Abstract_AssetManager2D : protected IAssetManager<TValue>
{
public:
	Abstract_AssetManager2D() = default;
	virtual ~Abstract_AssetManager2D() = default;

protected:
	unsigned int Size() const { return static_cast<unsigned int>(myAssetData.size()); }
	unsigned int Size(const TKey1& aKey) const
	{
		auto layer = myAssetData.find(aKey);
		return static_cast<unsigned int>(layer != myAssetData.end() ? myAssetData.at(aKey).size() : 0);
	}

	bool Empty() const { return myAssetData.empty(); }
	//This will create if no object currently exists
	Container& GetAsset(const TKey1& aKey1, const TKey2& aKey2)	const { return myAssetData[aKey1][aKey2]; }
	Container& GetFirst(const TKey1& aKey) { return myAssetData.at(aKey).begin()->second; }
	std::pair<TKey1, std::unordered_map<TKey2, Container> >& GetIndex(int aIndex) const
	{
		TITAN_ASSERT(aIndex < Size());

		auto asset = myAssetData.begin();
		std::advance(asset, aIndex);
		return asset.second;
	}

	std::pair<const TKey2, Container>& GetAssetPair(const TKey1& aKey1, const TKey2& aKey2) const
	{
		TITAN_ASSERT(HasLayer(aKey1));

		auto find = myAssetData.at(aKey1).find(aKey2);
		return *find;
	}

	std::pair<const TKey2, Container>& GetAssetPair(const TKey1& aKey, int aIndex) const
	{
		TITAN_ASSERT(HasLayer(aKey));
		TITAN_ASSERT(aIndex < (int)Size(aKey));

		auto asset = myAssetData.at(aKey).begin();
		std::advance(asset, aIndex);
		return *asset;
	}

	Container& GetIndex(const TKey1& aKey, int aIndex) const
	{
		TITAN_ASSERT(HasLayer(aKey));
		TITAN_ASSERT(aIndex < static_cast<int>(Size(aKey)) );

		auto asset = myAssetData.at(aKey).begin();
		std::advance(asset, aIndex);
		return asset->second;
	}

	void SetAssetPermanent(const TKey1& aKey1, const TKey2& aKey2, bool aIsPermanent)
	{
		Container& data = GetAsset(aKey1, aKey2);
		data.IsPermanent = aIsPermanent;

		if(!aIsPermanent && data.Count == 0)
		{
			Erase(aKey1, aKey2);
		}
	}

	bool Empty(const TKey1& aKey) const
	{
		if (!HasLayer(aKey)) return true;

		return myAssetData.at(aKey).empty();
	}

	bool HasLayer(const TKey1& aKey) const
	{
		auto layer = myAssetData.find(aKey);
		return layer != myAssetData.end();
	}

	bool HasAsset(const TKey1& aKey1, const TKey2& aKey2) const
	{
		auto layer = myAssetData.find(aKey1);
		if (layer == myAssetData.end()) return false;

		auto asset = layer->second.find(aKey2);
		return asset != layer->second.end();
	}


	virtual AssetPointer<TValue> CreateAssetPointer(const std::string& aAssetName, Container& aOutAssetData) const
	{
		if constexpr (std::is_arithmetic_v<TKey1>)
		{
			TITAN_ASSERT(GetLayerFromID(aOutAssetData.ID) < 100);
		}

		IAssetManager<TValue>& manager = *const_cast<IAssetManager<TValue>*>(dynamic_cast<const IAssetManager<TValue>*>(this));

		++aOutAssetData.Count;
		return AssetPointer<TValue>(aAssetName, aOutAssetData.ID, manager, aOutAssetData.Asset);
	}

	void Erase(const TKey1& aKey1, const TKey2& aKey2)
	{
		if (!HasAsset(aKey1, aKey2)) return;

		auto findLayer = myAssetData.find(aKey1);
		auto findAsset = findLayer->second;
		Destroy(findAsset->second);
		findLayer.erase(findAsset);
	}

	virtual void Destroy(Container& aOutAssetData) = 0;

	unsigned int SetLayerInID(unsigned int aID, unsigned int aLayerType) const
	{
		unsigned int newID = aID - GetLayerFromID(aID);
		newID += aLayerType;

		return newID;
	}

	unsigned int GetLayerFromID(unsigned int aID) const
	{
		unsigned int layer = aID % 100;
		return layer;
	}

	virtual void Cleanup()
	{
		auto itLayer = myAssetData.begin();
		while (itLayer != myAssetData.end())
		{
			auto& layer = itLayer->second;
			auto itAsset = layer.begin();
			while (itAsset != layer.end())
			{
				auto& asset = itAsset->second;
				if (1 < asset.Count)
				{
					//Error message: Too high count something wasn't unregistered correctly
				}

				Destroy(asset);
				itAsset = layer.erase(itAsset);
			}

			itLayer = myAssetData.erase(itLayer);
		}
	}

private:
	mutable std::unordered_map<TKey1, std::unordered_map<TKey2, Container> > myAssetData;


	// Inherited via IAssetManager
	void Copy(const AssetPointer<TValue>& aAssetPointer) override
	{
		typename std::unordered_map<TKey1, std::unordered_map<TKey2, Container> >::iterator findLayer;
		typename std::unordered_map<TKey2, Container>::iterator findData;

		if constexpr (std::is_same_v<TKey1, std::string>)
		{
			findLayer = myAssetData.find(aAssetPointer.GetAssetName().data());
			findData = findLayer->second.find(aAssetPointer.GetID());
		}
		else if constexpr (std::is_arithmetic_v<TKey1> && std::is_same_v<TKey2, std::string>)
		{
			findLayer = myAssetData.find(GetLayerFromID(aAssetPointer.GetID()));
			findData = findLayer->second.find(aAssetPointer.GetAssetName().data());
		}
		else if constexpr (std::is_arithmetic_v<TKey1> && std::is_arithmetic_v<TKey2>)
		{
			findLayer = myAssetData.find(GetLayerFromID(aAssetPointer.GetID()));
			findData = findLayer->second.find(aAssetPointer.GetID());
		}

		++findData->second.Count;
		//return CreateAssetPointer(aAssetPointer.GetAssetName().data(), findData->second);
	}

	void Destroy(const AssetPointer<TValue>& aAssetPointer) override
	{
		typename std::unordered_map<TKey1, std::unordered_map<TKey2, Container> >::iterator findLayer;
		typename std::unordered_map<TKey2, Container>::iterator findData;


		if constexpr (std::is_same_v<TKey1, std::string>)
		{
			findLayer = myAssetData.find(aAssetPointer.GetAssetName().data());
			findData = findLayer->second.find(aAssetPointer.GetID());
		}
		else if constexpr (std::is_arithmetic_v<TKey1> && std::is_same_v<TKey2, std::string>)
		{
			findLayer = myAssetData.find(GetLayerFromID(aAssetPointer.GetID()));
			findData = findLayer->second.find(aAssetPointer.GetAssetName().data());
		}
		else if constexpr (std::is_arithmetic_v<TKey1> && std::is_arithmetic_v<TKey2>)
		{
			findLayer = myAssetData.find(GetLayerFromID(aAssetPointer.GetID()));
			findData = findLayer->second.find(aAssetPointer.GetID());
		}

		auto& asset = findData->second;
		--asset.Count;
		if (!asset.IsPermanent && asset.Count == 0)
		{
			Destroy(asset);
			findLayer->second.erase(findData);
		}
	}


};

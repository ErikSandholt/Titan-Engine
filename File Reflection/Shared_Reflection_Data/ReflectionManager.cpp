#include "ReflectionManager.h"


ReflectionManager::ReflectionManager()
{
	InitReflection();
}

ReflectionManager& ReflectionManager::GetInstance()
{
	static ReflectionManager instance;
	return instance;
}


const ReflectionBase* ReflectionManager::GetReflection(unsigned int aID) const
{
	auto find = myReflections.find(aID);
	if (find == myReflections.end()) return nullptr;

	return find->second;
}
const ReflectionBase* ReflectionManager::GetReflection(const std::string& aName) const
{
	return GetReflection(Fnv1a_Hash_Lower(aName));
}

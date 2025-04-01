#pragma once
#include <unordered_map>
#include <vector>
#include "CallEvent.hpp"
#include "ReflectionClass.h"
#include "Hash.h"


/*#define ADD_METHOD_TO_STRING_CALL(Class, Method)										\
{																						\
	std::string methodName(#Method);													\
	FunctionCallManager::ConvertFunctionName(methodName);								\
																						\
	auto func = FunctionCallManager::Create_Function(&Class::Method);					\
	FunctionCallManager::Register(typeid(Class).name(), methodName, func);				\
}																						\*/	

class ReflectionManager
{
private:
	std::unordered_map<unsigned int, ReflectionClass>		myClasses	= {};
	std::unordered_map<unsigned int, ReflectionFunction>	myFunctions = {};
	std::unordered_map<unsigned int, ReflectionVariable>	myVariables = {};


	std::vector<ReflectionFunction*> myGlobalFunctionReflections	= {};
	std::vector<ReflectionVariable*> myGlobalVariablesReflections	= {};
	std::unordered_map<unsigned int, ReflectionBase*> myReflections = {};


public:
	ReflectionManager();

	static ReflectionManager& GetInstance();

	std::vector<ReflectionFunction*>& GetGlobalFunctions() const { return GetInstance().myGlobalFunctionReflections; }
	std::vector<ReflectionVariable*>& GetGlobalVariables() const { return GetInstance().myGlobalVariablesReflections; }

	//Hashed names
	//Generate one by using Fnv1a_Hash_Lower
	//Class needs "class " at start.
	const ReflectionBase* GetReflection(unsigned int aID) const;

	//Use TextToLower method before passing
	//Class needs "class " at start. Recommend using GetReflection<T> Or typeid(T).name() 
	const ReflectionBase* GetReflection(const std::string& aName) const;

	template<typename T>
	const ReflectionBase* GetReflection() const;

private:
	void InitReflection();	//Leave undefined in cpp file. The reflect program will define it

	// Helper to extract the method signature and create std::function
	template<typename ClassType, typename ReturnType, typename... Args>
	static std::function<ReturnType(ClassType* aInstance, Args...)> Create_Function(ReturnType(ClassType::* method)(Args...))
	{
		return [=](ClassType* aInstance, Args... args) -> ReturnType
			{
				return (aInstance->*method)(args...);
			};
	}

	template<typename ClassType, typename ReturnType, typename... Args>
	static std::function<ReturnType(ClassType* aInstance, Args...)> Create_Function(ReturnType(ClassType::* method)(Args...) const)
	{
		return [=](ClassType* aInstance, Args... args) -> ReturnType
			{
				return (aInstance->*method)(args...);
			};
	}

	template<typename ReturnType, typename... Args>
	static std::function<ReturnType(Args...)> Create_Function(ReturnType(*function)(Args...))
	{
		return [=](Args... args) -> ReturnType
			{
				return function(args...);
			};
	}

	//Variable
	template<typename ClassType, typename ReturnType>
	static std::function<ReturnType(ClassType*)> Create_Variable(ReturnType ClassType::* aVariable)
	{
		return [=](ClassType* aInstance) -> ReturnType
			{
				return (aInstance->*aVariable);
			};
	}
};


template<typename T>
inline const ReflectionBase* ReflectionManager::GetReflection() const
{
	return GetReflection(typeid(T).name());
}
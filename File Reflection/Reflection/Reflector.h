#pragma once
#include <stack>
#include <queue>
#include "Reflection_File.h"
//#include "Reflection_Class.h"
//#include "Reflection_Function.h"
#include <clang-c\Index.h>
//#include "BitMask.hpp"

enum ReflectionClassMask
{
	Reflected	,
	BaseClasses	,
	AllVariables,
	AllFunctions,

};

struct ReflectClassData
{
	//Reflection Specific only
	BitMask								reflectionSettings = {};
	CXSourceRange						range;

	ReflectionClass						reflectClass = {};
};

class Reflector
{
public:
	std::queue<CXSourceLocation>							myMacroStack				= {};
	std::stack<ReflectClassData*>							myClassStack				= {};


	std::vector<Reflection_File>							myReflectedFiles			= {};
	std::unordered_map<std::string, ReflectClassData>		myReflectedClasses			= {};
	std::unordered_map<std::string, ReflectionVariable>		myReflectedVariables		= {};
	std::unordered_map<std::string, ReflectionFunction>		myReflectedFunctions		= {};

	std::unordered_map<unsigned int, ReflectionBase*>		myReflections				= {};

public:
	bool ReflectHeaderFile(const char* aPath, int someArgAmount, const char** someArgs);

	void Clear();

private:
	static CXChildVisitResult HeaderVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data);

	CXChildVisitResult ReflectMacro(		CXCursor aCursor, CXCursor aParent);
	CXChildVisitResult ReflectClass(		CXCursor aCursor, CXCursor aParent, ReflectionClass*    aClassData);
	CXChildVisitResult ReflectFunction(		CXCursor aCursor, CXCursor aParent, ReflectionFunction* aFunctionData);
	CXChildVisitResult ReflectVariable(		CXCursor aCursor, CXCursor aParent, ReflectionVariable* aFunctionData);

	bool TryPopMacro(CXCursor& aCursor);
	bool TryPopMacro(CXSourceLocation& aCursorLocation);
	bool GetClassBit(ReflectionClassMask aReflectInfo) const;

	static std::vector<std::string> GetNamespace(CXCursor cursor);

	ReflectionFunction CreateFunction(CXCursor aCursor);
	ReflectionVariable CreateVariable(CXCursor aCursor, bool isArgument = false);
	ReflectionType CreateType(CXType aType);
	static CXVisitorResult GetSubClasses(CXCursor aCursor, CXClientData aList);

	static void PrintReflection(const ReflectionBase* aReflection);
};

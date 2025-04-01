#include "Reflector.h"
#include "Hash.h"
#include <iostream>
#include <fstream>
#include <filesystem>

#define COMPONENT_CLASS "AbstractComponent"


bool Reflector::ReflectHeaderFile(const char* aPath, int someArgAmount, const char** someArgs)
{
	while (!myMacroStack.empty()) { myMacroStack.pop(); }	//Clear
	while (!myClassStack.empty()) { myClassStack.pop(); }	//Clear

    CXIndex index			= clang_createIndex(0, 0);
    CXTranslationUnit unit	= clang_parseTranslationUnit(index, aPath, someArgs, someArgAmount, nullptr, 0,
        CXTranslationUnit_DetailedPreprocessingRecord /*CXTranslationUnit_PrecompiledPreamble | CXTranslationUnit_Incomplete*/);

    if (!unit) {
        std::cerr << "Failed to parse file: " << aPath << std::endl;
        return false;
    }

	std::filesystem::path path = aPath;
	
	myReflectedFiles.push_back({});
	myReflectedFiles.back().path = aPath;
	myReflectedFiles.back().name = path.filename().replace_extension().string();

    CXCursor rootCursor = clang_getTranslationUnitCursor(unit);

	std::cout << "\nReading File: " << myReflectedFiles.back().name << std::endl;

    // Traverse AST
    clang_visitChildren(rootCursor, &HeaderVisitor, this);

	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
    return true;
}

void Reflector::Clear()
{
    while (!myMacroStack.empty()) { myMacroStack.pop(); }
    while (!myClassStack.empty()) { myClassStack.pop(); }
    
    
    myReflectedClasses.clear();
    myReflectedFunctions.clear();
    myReflectedVariables.clear();
}

CXChildVisitResult Reflector::HeaderVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
	CXCursorKind		kind				= clang_getCursorKind(cursor);
	CXString			cursorName			= clang_getCursorSpelling(cursor);
    Reflector*			reflector           = static_cast<Reflector*>(client_data);
	CXChildVisitResult	result				= CXChildVisit_Continue;


    if (!reflector->myClassStack.empty())
    {
        CXSourceLocation end = clang_getRangeEnd(reflector->myClassStack.top()->range);
        CXSourceLocation current = clang_getCursorLocation(cursor);

        //Past end
        if (clang_isBeforeInTranslationUnit(end, current))
        {
            reflector->myClassStack.pop();
        }
    }

	switch (kind)
	{
	case CXCursor_Namespace:		{ result = CXChildVisit_Recurse;									break; }
	case CXCursor_ClassDecl:		{ result = reflector->ReflectClass(		cursor, parent, nullptr);	break; }
	case CXCursor_CXXMethod:		{ result = reflector->ReflectFunction(	cursor, parent, nullptr);	break; }
	//case CXCursor_FunctionDecl:		{ result = reflector->ReflectFunction(	cursor, parent, nullptr);	break; } It might not be a good idea to reflect globals	//Global
	case CXCursor_FieldDecl:		{ result = reflector->ReflectVariable(	cursor, parent, nullptr);	break; }
	//case CXCursor_VarDecl:			{ result = reflector->ReflectVariable(	cursor, parent, nullptr);	break; } It might not be a good idea to reflect globals	//Global
	case CXCursor_MacroExpansion:	{ result = reflector->ReflectMacro(		cursor, parent);			break; }

	default:
		break;
	}
	clang_disposeString(cursorName);
	return result;
}

CXChildVisitResult Reflector::ReflectMacro(CXCursor cursor, CXCursor parent)
{
	CXString				cursorName	= clang_getCursorSpelling(cursor);

	std::string macroName = clang_getCString(cursorName);
	if (macroName == "REFLECT_FUNCTION" ||
		macroName == "REFLECT_CLASS"	||
		macroName == "REFLECT_VARIABLE"		)
	{
		myMacroStack.push(clang_getCursorLocation(cursor));
	}

	clang_disposeString(cursorName);
	return CXChildVisitResult::CXChildVisit_Continue;
}

CXChildVisitResult Reflector::ReflectClass(CXCursor aCursor, CXCursor aParent, ReflectionClass* aClassData)
{
	CXString					cursorName		= clang_getTypeSpelling(clang_getCursorType(aCursor));//clang_getCursorSpelling(aCursor);
	std::string					className		= clang_getCString(cursorName);
	std::vector<std::string>	nameSpaceName	= GetNamespace(aCursor);

	//std::string className = "class ";
	//className += clang_getCString(clang_getTypeSpelling(type));
	
	//Remove Namespace
	{
		size_t point = className.rfind(':');
		if (point != std::string::npos)
		{
			className = className.substr(point + 1);
		}
	}

	std::string					fullName		= "class ";
	for (auto& name : nameSpaceName)
	{
		fullName += name + "::";
	}

	nameSpaceName = GetNamespace(aCursor);


	fullName += className;

	auto find = myReflectedClasses.find(fullName);
	if (find == myReflectedClasses.end())
	{
		ReflectClassData& info = myReflectedClasses[fullName];	//Forward declare
		myReflectedFiles.back().reflectedClasses[fullName] = &info.reflectClass;


		info.reflectClass.name		= className;
		info.reflectClass.nameSpace = std::move(nameSpaceName);

		std::string fullName = info.reflectClass.GetFullName();

		info.reflectClass.id		= Fnv1a_Hash_Lower(info.reflectClass.name);
		info.reflectClass.globalID	= Fnv1a_Hash_Lower(fullName);
		

		myReflections[info.reflectClass.globalID] = &info.reflectClass;

		info.reflectClass.type = CreateType(clang_getCursorType(aCursor));
		info.range = clang_getCursorExtent(aCursor);
		

		myClassStack.push(&info);

		clang_visitCXXBaseClasses(clang_getCursorType(aCursor), GetSubClasses, this);

		bool reflect = TryPopMacro(aCursor);

		if (!reflect) reflect = info.reflectClass.name == COMPONENT_CLASS;;

		auto& baseClasses = info.reflectClass.baseClasses;
		for (size_t i = 0; i < baseClasses.size(); ++i)
		{
			if (reflect) break;

			reflect = baseClasses.at(i)->name == COMPONENT_CLASS;
		}

		info.reflectionSettings.SetBit(ReflectionClassMask::AllFunctions, reflect);
		info.reflectionSettings.SetBit(ReflectionClassMask::AllVariables, reflect);
		info.reflectionSettings.SetBit(ReflectionClassMask::Reflected, true);

		PrintReflection(&info.reflectClass);
	}
	else if (!find->second.reflectionSettings.GetBit(ReflectionClassMask::Reflected))
	{

		find->second.range = clang_getCursorExtent(aCursor);

		find->second.reflectClass.type = CreateType(clang_getCursorType(aCursor));

		myClassStack.push(&find->second);

		clang_visitCXXBaseClasses(clang_getCursorType(aCursor), GetSubClasses, this);

		bool reflect = TryPopMacro(aCursor);

		if (!reflect) reflect = find->second.reflectClass.name == COMPONENT_CLASS;;

		auto& baseClasses = find->second.reflectClass.baseClasses;
		for (size_t i = 0; i < baseClasses.size(); ++i)
		{
			if (reflect) break;

			reflect = baseClasses.at(i)->name == COMPONENT_CLASS;
		}

		find->second.reflectionSettings.SetBit(ReflectionClassMask::AllFunctions, reflect);
		find->second.reflectionSettings.SetBit(ReflectionClassMask::AllVariables, reflect);
		find->second.reflectionSettings.SetBit(ReflectionClassMask::Reflected, true);
		PrintReflection(&find->second.reflectClass);
	}
	else
	{
		CXSourceLocation end = clang_getRangeEnd(clang_getCursorExtent(aCursor));
		while (TryPopMacro(end)) { }	//Pop all macros

		clang_disposeString(cursorName);
		return CXChildVisitResult::CXChildVisit_Continue;
	}



	clang_disposeString(cursorName);
	return CXChildVisitResult::CXChildVisit_Recurse;
}

CXChildVisitResult Reflector::ReflectFunction(CXCursor aCursor, CXCursor aParent, ReflectionFunction* aFunctionData)
{
	CXString	cursorName	= clang_getCursorSpelling(aCursor);
	std::string name = clang_getCString(cursorName);

	bool reflect = TryPopMacro(aCursor)								|| 
		//!myClassStack.empty() && myClassStack.top()->isComponent	||
		GetClassBit(ReflectionClassMask::AllFunctions);

	CX_CXXAccessSpecifier specifier = clang_getCXXAccessSpecifier(aCursor);

	if (reflect && specifier == CX_CXXAccessSpecifier::CX_CXXPublic)
	{
		if (!myClassStack.empty())	//Global
		{
			name.insert(0, myClassStack.top()->reflectClass.name + "::");
		}

		auto find = myReflectedFunctions.find(name);
		if (find == myReflectedFunctions.end())
		{
			ReflectionFunction func			= CreateFunction(aCursor);
			myReflectedFunctions[name]		= func;
			myReflections[func.globalID]	= &myReflectedFunctions.at(name);

			PrintReflection(&func);
		}

		if (!myClassStack.empty())	//Global
		{
			myClassStack.top()->reflectClass.functions.push_back(&myReflectedFunctions.at(name));
		}
		else
		{
			myReflectedFiles.back().reflectedGlobalFunctions[name] = &myReflectedFunctions.at(name);
		}

	}

	clang_disposeString(cursorName);
	return CXChildVisitResult::CXChildVisit_Continue;
}

CXChildVisitResult Reflector::ReflectVariable(CXCursor aCursor, CXCursor aParent, ReflectionVariable* aFunctionData)
{
	CXString cursorName			= clang_getCursorSpelling(aCursor);
	std::string name = clang_getCString(cursorName);

	bool reflect = TryPopMacro(aCursor)								||
		//!myClassStack.empty() && myClassStack.top()->isComponent	||
		GetClassBit(ReflectionClassMask::AllVariables);

	CX_CXXAccessSpecifier specifier = clang_getCXXAccessSpecifier(aCursor);

	if (reflect && specifier == CX_CXXAccessSpecifier::CX_CXXPublic)
	{
		

		if (!myClassStack.empty())	//Global
		{
			name.insert(0, myClassStack.top()->reflectClass.name + "::");
		}

		auto find = myReflectedVariables.find(name);
		if (find == myReflectedVariables.end())
		{
			//Register before creating
			ReflectionVariable variable = CreateVariable(aCursor);
			myReflectedVariables[name] = variable;
			myReflections[variable.globalID] = &myReflectedVariables[name];

			PrintReflection(&variable);
		}

		if (!myClassStack.empty())	//Global
		{
			myClassStack.top()->reflectClass.variables.push_back(&myReflectedVariables.at(name));
		}
		else
		{
			myReflectedFiles.back().reflectedGlobalVariables[name] = &myReflectedVariables.at(name);
		}
	}

	clang_disposeString(cursorName);
	return CXChildVisitResult::CXChildVisit_Continue;
}

bool Reflector::TryPopMacro(CXCursor& aCursor)
{
	if (myMacroStack.empty()) return false;

	CXSourceLocation location = clang_getCursorLocation(aCursor);

	return TryPopMacro(location);
}

bool Reflector::TryPopMacro(CXSourceLocation& aCursorLocation)
{
	if (myMacroStack.empty()) return false;

	bool popMacro = clang_isBeforeInTranslationUnit(myMacroStack.front(), aCursorLocation);

	if (popMacro)
	{
		myMacroStack.pop();
	}

	return popMacro;
}

bool Reflector::GetClassBit(ReflectionClassMask aReflectInfo) const
{
	if (myClassStack.empty()) return false;

	return myClassStack.top()->reflectionSettings.GetBit(aReflectInfo);
}

std::vector<std::string> Reflector::GetNamespace(CXCursor cursor)
{
	CXCursor parent = clang_getCursorSemanticParent(cursor);
	std::vector<std::string> namespaceText = {};

	while (!clang_Cursor_isNull(parent)) {
		if (clang_getCursorKind(parent) == CXCursor_Namespace) {
			CXString nsName = clang_getCursorSpelling(parent);
			std::string namespaceStr = clang_getCString(nsName);
			clang_disposeString(nsName);

			namespaceText.insert(namespaceText.begin(), namespaceStr);
			//return namespaceStr; // Return the first found namespace
		}

		parent = clang_getCursorSemanticParent(parent);
	}

	return namespaceText;
}

ReflectionFunction Reflector::CreateFunction(CXCursor aCursor)
{
	CXString cursorName = clang_getCursorSpelling(aCursor);

	ReflectionFunction func;
	func.name = clang_getCString(cursorName);
	//func.nameSpace namespace and Class Name
	if (myClassStack.empty())
	{
		func.nameSpace = GetNamespace(aCursor);
	}
	else
	{
		func.nameSpace = myClassStack.top()->reflectClass.nameSpace;
		func.nameSpace.push_back(myClassStack.top()->reflectClass.name);
	}
	
	std::string fullName = func.GetFullName();

	func.globalID	= Fnv1a_Hash_Lower(fullName);
	func.id			= Fnv1a_Hash_Lower(func.name);

	CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(aCursor);
	int accessID = static_cast<int>(access);
	assert(accessID != 0);	//Invalid Could index 0 be Global?

	func.decal.SetDigits(0, 1, accessID);

	if (clang_CXXMethod_isStatic(aCursor))
	{
		func.decal.SetDigits(1, 1, static_cast<int>(DecalType::Static));
	}


	func.returnValue = CreateType(clang_getCursorResultType(aCursor));



	int argAmount = clang_Cursor_getNumArguments(aCursor);

	func.arguments.reserve(argAmount);

	for (size_t i = 0; i < argAmount; ++i)
	{
		CXCursor argCursor	= clang_Cursor_getArgument(aCursor, i);
		func.arguments.push_back(CreateVariable(argCursor, true));
	}

	func.type = CreateType(clang_getCursorType(aCursor));
	clang_disposeString(cursorName);
	return func;
}


template<typename T>
void ToByte(const T& aValue, std::string& outContainer)
{
	const char* data	= reinterpret_cast<const char*>(&aValue);
	const size_t size	= sizeof(T);
	outContainer.resize(size);

	for (size_t i = 0; i < size; ++i)
	{
		outContainer[i] = data[i];
	}
}

ReflectionVariable Reflector::CreateVariable(CXCursor aCursor, bool isArgument)
{
	CXString cursorName = clang_getCursorSpelling(aCursor);

	ReflectionVariable variable;
	variable.name = clang_getCString(cursorName);

	if (myClassStack.empty())
	{
		variable.nameSpace = GetNamespace(aCursor);
	}
	else
	{
		variable.nameSpace = myClassStack.top()->reflectClass.nameSpace;
		variable.nameSpace.push_back(myClassStack.top()->reflectClass.name);
	}

	std::string fullName = variable.GetFullName();

	variable.globalID	= Fnv1a_Hash_Lower(fullName);
	variable.id			= Fnv1a_Hash_Lower(variable.name);

	variable.valueType = CreateType(clang_getCursorType(aCursor));

	
	variable.type.type	= isArgument ? R_Type::Argument : R_Type::Variable;

	if (variable.type.type == R_Type::Variable)
	{
		CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(aCursor);
		int accessID = static_cast<int>(access);
		assert(accessID != 0);	//Invalid Could index 0 be Global?

		variable.decal.SetDigits(0, 1, accessID);
	}
	

	clang_disposeString(cursorName);
	return variable;
}

ReflectionType Reflector::CreateType(CXType aType)
{
	ReflectionType reflectType = {};

	bool isRef =	aType.kind == CXType_LValueReference ||
					aType.kind == CXType_RValueReference;

	CXType type =  clang_getCanonicalType(aType);

	//Remove Reference
	if (isRef) type = clang_getPointeeType(type);

	int digitIndex = 0;
	//outAType.type = clang_getCanonicalType(aType);

	if (clang_isConstQualifiedType(aType)) reflectType.decal.SetDigits(digitIndex++, 1, static_cast<int>(DecalType::Const));//outAType.decal.push_back(ReflectInfoMask::Const);
	

	{
		while (type.kind == CXType_Pointer)
		{
			//Arrays?
			if (clang_isConstQualifiedType(type)) reflectType.decal.SetDigits(digitIndex++, 1, static_cast<int>(DecalType::Const));
			reflectType.decal.SetDigits(digitIndex++, 1, static_cast<int>(DecalType::Pointer));

			assert(digitIndex < 9 && "Too many Digits");

			type = clang_getPointeeType(type);
		}
	}

	if (isRef)
	{
		reflectType.decal.SetDigits(digitIndex++, 1, static_cast<int>(DecalType::Reference));
	}

	//Type
	switch (type.kind)
	{
	case CXTypeKind::CXType_Bool:			{ reflectType.type = R_Type::Bool;		break; }
	case CXTypeKind::CXType_Float:			{ reflectType.type = R_Type::Float;		break; }
	case CXTypeKind::CXType_Int:			{ reflectType.type = R_Type::Int;		break; }
	case CXTypeKind::CXType_Void:			{ reflectType.type = R_Type::Void;		break; }
	case CXTypeKind::CXType_Char_S:			{ reflectType.type = R_Type::Char;		break; }
	case CXTypeKind::CXType_FunctionNoProto:{ reflectType.type = R_Type::Function;	break; }
	case CXTypeKind::CXType_FunctionProto:	{ reflectType.type = R_Type::Function;	break; }
	case CXTypeKind::CXType_ObjCClass:		{ reflectType.type = R_Type::Class;		break; }
	case CXTypeKind::CXType_Record:			{ reflectType.type = R_Type::Class;		break; }
	default:								{ reflectType.type = R_Type::NONE;		break; }
	}


	//If Type refers to a class get the pointer to it from reflection
	if (reflectType.type == R_Type::Class		)//|| reflectType.type == R_Type::Function)
	{
		CXString typeName = clang_getTypeSpelling(aType);
		std::string classname = clang_getCString(typeName);
		classname.insert(0, "class ");
		reflectType.data = myReflections.at(Fnv1a_Hash_Lower(classname));
		clang_disposeString(typeName);
	}


	return reflectType;
}

CXVisitorResult Reflector::GetSubClasses(CXCursor aCursor, CXClientData aList)
{
	Reflector&	reflector						= *static_cast<Reflector*>(aList);
	CXString					cursorName		= clang_getCursorSpelling(aCursor);
	std::string					className		= clang_getCString(cursorName);
	std::vector<std::string>	nameSpaceName	= GetNamespace(aCursor);


	cursorName = clang_getCursorDisplayName(aCursor);
	className = clang_getCString(cursorName);

	std::string					fullName		= "class ";
	for (auto& name : nameSpaceName)
	{
		fullName += name + "::";
	}

	fullName += className;

	auto find = reflector.myReflectedClasses.find(fullName);
	if (find != reflector.myReflectedClasses.end())
	{
		reflector.myClassStack.top()->reflectClass.baseClasses.push_back(&find->second.reflectClass);

		clang_disposeString(cursorName);
		return CXVisitorResult::CXVisit_Continue;
	}

	//Forward declare
	auto& info = reflector.myReflectedClasses[fullName];	
	reflector.myReflectedFiles.back().reflectedClasses[fullName] = &info.reflectClass;
	
	info.reflectClass.name		= className;
	info.reflectClass.nameSpace = std::move(nameSpaceName);

	info.reflectClass.globalID	= Fnv1a_Hash_Lower(info.reflectClass.GetFullName());
	info.reflectClass.id		= Fnv1a_Hash_Lower(info.reflectClass.name);


	reflector.myReflections[info.reflectClass.globalID] = &info.reflectClass;

	info.reflectClass.type = reflector.CreateType(clang_getCursorType(aCursor));

	reflector.myClassStack.top()->reflectClass.baseClasses.push_back(&info.reflectClass);


	clang_disposeString(cursorName);
	return CXVisitorResult::CXVisit_Continue;
}

void Reflector::PrintReflection(const ReflectionBase* aReflection)
{
	std::string message = "";
	
	switch (aReflection->type.type)
	{
	case R_Type::Class:		message = "\n\n\t Reflecting Class[ ";		break;
	case R_Type::Function:	message = "\t\t Reflecting Function[ ";		break;
	case R_Type::Variable:	message = "\t\t Reflecting Variable[ ";		break;
	default:
		break;
	}

	message += "\tName: " + aReflection->GetFullName();
	message += "\t\tGlobalID: " + std::to_string(aReflection->globalID);
	
	switch (aReflection->type.type)
	{
	case R_Type::Class:		message += " ]";	break;
	case R_Type::Function:	message += " ]";	break;
	case R_Type::Variable:	message += " ]";	break;
	default:
		break;
	}

	std::cout << message << std::endl;
}

#pragma once
#include <fstream>


#define RAW_TO_STRING(arg)				#arg
#define TO_STRING(arg)					RAW_TO_STRING(arg)
#define CONST_TEXT(first, seconed, arg) constexpr const char* first ##_ ##seconed	= TO_STRING(arg)
#define REFLECT_TEXT(name, arg)			CONST_TEXT(ReflectName, name, arg)

//Class Using for Reflection
#define CUFR_TYPE			ReflectionType
#define CUFR_VARIABLE		ReflectionVariable
#define CUFR_FUNCTION		ReflectionFunction
#define CUFR_CLASS			ReflectionClass
//#define CUFR_FILE			Reflection_File

#define CUFR_TYPE_TEXT		TO_STRING(CUFR_TYPE)
#define CUFR_VARIABLE_TEXT	TO_STRING(CUFR_VARIABLE)
#define CUFR_FUNCTION_TEXT	TO_STRING(CUFR_FUNCTION)
#define CUFR_CLASS_TEXT		TO_STRING(CUFR_CLASS)

//Type
REFLECT_TEXT(Type,			type);
REFLECT_TEXT(Variable,		variable);
REFLECT_TEXT(Function,		function);
REFLECT_TEXT(Class,			rClass);

class CUFR_TYPE;
class CUFR_VARIABLE;
class CUFR_FUNCTION;
class CUFR_CLASS;

class CodeGenerator
{
public:
	static const char* GenerateType(		std::ofstream& aStream, const CUFR_TYPE*		inType);
	static const char* GenerateVariable(	std::ofstream& aStream, const CUFR_VARIABLE*	inVariable);
	static const char* GenerateFunction(	std::ofstream& aStream, const CUFR_FUNCTION*	inFunction);
	static const char* GenerateClass(		std::ofstream& aStream, const CUFR_CLASS*		inClass);
};

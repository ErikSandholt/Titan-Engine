#include "CodeGenerator.h"
#include "../Shared_Reflection_Data/ReflectionClass.h"

const char* CodeGenerator::GenerateType(std::ofstream& aStream, const CUFR_TYPE* inType)
{
	aStream << CUFR_TYPE_TEXT << " " << ReflectName_Type << ";\n";

	aStream << ReflectName_Type << ".decal = " << inType->decal.GetMask() << "; \n";
	aStream << ReflectName_Type << ".type = static_cast<R_Type>(" << (int)inType->type << "); \n";

	if (inType->data)
	{
		aStream << ReflectName_Type << ".data = myReflections.at(" << inType->data->globalID << "); \n";
	}
    return ReflectName_Type;
}

const char* CodeGenerator::GenerateVariable(std::ofstream& aStream, const CUFR_VARIABLE* inVariable)
{
	aStream << "//Variable\n";

	
	aStream << CUFR_VARIABLE_TEXT << " " << ReflectName_Variable << " = {}; \n";
	aStream << "{\n"; //Start Scope
	aStream << ReflectName_Variable << ".id			= "		<< inVariable->id					<< ";\n";
	aStream << ReflectName_Variable << ".globalID	= "		<< inVariable->globalID			<< ";\n";
	aStream << ReflectName_Variable << ".decal		= "			<< inVariable->decal.GetMask()	<< ";\n";
	aStream << ReflectName_Variable << ".name		= \""	<< inVariable->name				<< "\";\n";

	for (size_t i = 0; i < inVariable->nameSpace.size(); ++i)
	{
		aStream << ReflectName_Variable << ".nameSpace.push_back( \"" << inVariable->nameSpace.at(i) << "\" );\n";
	}

	if (inVariable->type.type == R_Type::Variable)
	{
		aStream << ReflectName_Variable << ".invoke	= ReflectionManager::Create_Variable(&" << inVariable->GetFullName() << ");\n";
	}

	//Was a variable before
	aStream << "\t{\n"; //Start Scope
	GenerateType(aStream, &inVariable->type);
	aStream << ReflectName_Variable << ".type		= std::move( " << ReflectName_Type << " );\n";
	aStream << "\t}\n"; //End Scope

	aStream << "\t{\n"; //Start Scope
	GenerateType(aStream, &inVariable->valueType);
	aStream << ReflectName_Variable << ".valueType = std::move(" << ReflectName_Type << ");\n";
	aStream << "\t}\n"; //End Scope


	aStream << "}\n"; //End Scope
    return ReflectName_Variable;
}

const char* CodeGenerator::GenerateFunction(std::ofstream& aStream, const CUFR_FUNCTION* inFunction)
{
	aStream << "//Function\n";

	aStream << CUFR_FUNCTION_TEXT << " " << ReflectName_Function << " = {}; \n";
	aStream << "{\n"; //Start Scope
	aStream << ReflectName_Function << ".id			= " << inFunction->id << ";\n";
	aStream << ReflectName_Function << ".globalID	= " << inFunction->globalID << ";\n";
	aStream << ReflectName_Function << ".name		= \"" << inFunction->name << "\";\n";


	for (size_t i = 0; i < inFunction->nameSpace.size(); ++i)
	{
		aStream << ReflectName_Function << ".nameSpace.push_back( \"" << inFunction->nameSpace.at(i) << "\" );\n";
	}

	aStream << ReflectName_Function << ".decal		= " << inFunction->decal.GetMask() << ";\n";
	aStream << ReflectName_Function << ".invoke	= ReflectionManager::Create_Function(&" << inFunction->GetFullName() << ");\n";

	//Return Value
	{
		
		aStream << "\t{\n"; //Start Scope
		GenerateType(aStream, &inFunction->returnValue);
		aStream << ReflectName_Function << ".returnValue = std::move( " << ReflectName_Type << " );\n";
		aStream << "\t}\n"; //End Scope
	}

	//Arguments
	for (size_t i = 0; i < inFunction->arguments.size(); ++i)
	{
		
		const CUFR_VARIABLE& variable = inFunction->arguments.at(i);

		aStream << "\t{\n"; //Start Scope
		GenerateVariable(aStream, &variable);
		aStream << ReflectName_Function << ".arguments.push_back(std::move(" << ReflectName_Variable << ")); \n";
		aStream << "\t}\n"; //End Scope
	}

	aStream << "}\n"; //End Scope
    return ReflectName_Function;
}

const char* CodeGenerator::GenerateClass(std::ofstream& aStream, const CUFR_CLASS* inClass)
{
	aStream << "//Function\n";

	
	aStream << CUFR_CLASS_TEXT << " " << ReflectName_Class << " = {}; \n";
	aStream << "{\n"; //Start Scope
	aStream << ReflectName_Class << ".id		= " << inClass->id << ";\n";
	aStream << ReflectName_Class << ".globalID	= " << inClass->globalID << ";\n";
	aStream << ReflectName_Class << ".decal		= " << inClass->decal.GetMask() << ";\n";
	aStream << ReflectName_Class << ".name		= \"" << inClass->name << "\";\n";
	//aStream << reflectName << "->invoke			= ReflectionManager::Create_Function(&" << aFunction.GetFullName() << ");\n";

	for (size_t i = 0; i < inClass->nameSpace.size(); ++i)
	{
		aStream << ReflectName_Class << ".nameSpace.push_back( \"" << inClass->nameSpace.at(i) << "\" );\n";
	}

	aStream << "//Functions \n";
	for (size_t i = 0; i < inClass->functions.size(); ++i)
	{
		aStream << ReflectName_Class << ".functions.push_back(static_cast<" << CUFR_FUNCTION_TEXT << "*>(myReflections.at(" << inClass->functions.at(i)->globalID <<"))	); //" << inClass->functions.at(i)->GetFullName() << "\n";
	}

	aStream << "//Variables \n";
	for (size_t i = 0; i < inClass->variables.size(); ++i)
	{
		aStream << ReflectName_Class << ".variables.push_back(static_cast<" << CUFR_VARIABLE_TEXT << "*>(myReflections.at(" << inClass->variables.at(i)->globalID <<"))	); //" << inClass->variables.at(i)->GetFullName() << "\n";
	}

	aStream << "}\n"; //End Scope
    return ReflectName_Class;
}

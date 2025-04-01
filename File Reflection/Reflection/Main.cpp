#pragma once
#include <clang-c\Index.h>
//#include "ReflectionDef.h"
//#include "Hash.h"
//#include "BitMask.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include "Reflector.h"
#include "CodeGenerator.h"

#define REFLECT_EXCLUDE_INVOKE


void WriteAddReflection(std::ofstream& aStream, const ReflectionBase* aReflection)
{
	const char* reflectionName = nullptr;
	std::string containerName = "";

	switch (aReflection->type.type)
	{
	case R_Type::Class:
	{
		reflectionName = ReflectName_Class;
		containerName = "myClasses";
		break;
	}
	case R_Type::Function:
	{
		reflectionName = ReflectName_Function;
		containerName = "myFunctions";
		break;
	}
	case R_Type::Variable:
	{
		reflectionName = ReflectName_Variable;
		containerName = "myVariables";
		break;
	}
	default:
		break;
	}

	aStream << containerName << "[" << aReflection->globalID << "] = std::move(" << reflectionName << ");\n";
	aStream << "myReflections[" << aReflection->globalID << "] = &" << containerName << ".at(" << aReflection->globalID << ");\n";

	if (aReflection->decal.GetDigits(0, 1) == 0 && 
		(aReflection->type.type == R_Type::Function || aReflection->type.type == R_Type::Variable))//(int)DecalType::Global
	{
		std::string gloabalName = aReflection->type.type == R_Type::Function ? 
			"myGlobalFunctionReflections" : "myGlobalVariablesReflections";

		std::string cast = aReflection->type.type == R_Type::Function ?
			CUFR_FUNCTION_TEXT : CUFR_VARIABLE_TEXT;

		aStream << gloabalName << ".push_back( " << "static_cast<" << cast << "*>(myReflections.at(" << aReflection->globalID << ")) );\n";
	}
}

void WriteGlobals(std::ofstream& aStream, const Reflection_File& aFile)
{
	//Globals
	aStream << "//Globals\n";
	aStream << "\t//Functions\n";
	{
		auto it = aFile.reflectedGlobalFunctions.begin();
		while (it != aFile.reflectedGlobalFunctions.end())
		{
			aStream << "{\n";	//Scope Start
			CodeGenerator::GenerateFunction(aStream, it->second);
			WriteAddReflection(aStream, it->second);
			aStream << "}\n";	//Scope end
			++it;
		}
	}

	aStream << "\n";

	//Variables
	aStream << "\t//Variables\n";
	{
		auto it = aFile.reflectedGlobalVariables.begin();
		while (it != aFile.reflectedGlobalVariables.end())
		{
			aStream << "{\n";	//Scope Start
			CodeGenerator::GenerateVariable(aStream, it->second);
			WriteAddReflection(aStream, it->second);
			aStream << "}\n";	//Scope end
			++it;
		}
	}

	aStream << "\n";
}

void WriteClass(std::ofstream& aStream, const ReflectionClass& aClass)
{
	std::cout << "\tGenerating Code for Class: " << aClass.name << std::endl;

	aStream << "//" << aClass.name << "\n";

	//Functions
	aStream << "//Functions\n";
	for (int i = 0; i < aClass.functions.size(); ++i)
	{
		aStream << "{\n";	//Scope start
		CodeGenerator::GenerateFunction(aStream, aClass.functions.at(i));
		WriteAddReflection(aStream, aClass.functions.at(i));
		aStream << "}\n";	//Scope end
	}

	aStream << "\n";

	//Variables
	aStream << "//Variables\n";
	for (int i = 0; i < aClass.variables.size(); ++i)
	{
		aStream << "{\n";	//Scope start
		CodeGenerator::GenerateVariable(aStream, aClass.variables.at(i));
		WriteAddReflection(aStream, aClass.variables.at(i));
		aStream << "}\n";	//Scope end
	}

	aStream << "{\n";	//Scope start
	CodeGenerator::GenerateClass(aStream, &aClass);
	WriteAddReflection(aStream, &aClass);
	aStream << "}\n";	//Scope end
}


void GenerateReflection(const std::string& inReflectionFolderPath, const Reflector& inReflector)
{

	{
		std::ofstream output(inReflectionFolderPath + "/ReflectComponentData.cpp");

		std::cout << "\nGenerating cpp file at: " << inReflectionFolderPath + "/ReflectComponentData.cpp" << std::endl;

		//Calculate the Diff path between output folder and Includes
		output << "#include \"" << "../../Shared_Reflection_Data/ReflectionManager.h" << "\"\n";

		auto& fileList = inReflector.myReflectedFiles;
		for (size_t i = 0; i < fileList.size(); ++i)
		{
			auto& file = fileList[i];
			//If no information skip
			if (file.IsEmpty()) continue;


			//Calculate the Diff path between output folder and Includes
			output << "#include \"" << file.path << "\"\n";
		}

		output << "\n\n";

		output << "void ReflectionManager::InitReflection() \n{\n";
		for (size_t i = 0; i < fileList.size(); ++i)
		{
			auto& file = fileList[i];


			std::cout << "\nGenerating Code From: " << file.name << std::endl;
			
			output << "//\t" << file.name << "\n";

			WriteGlobals(output, file);

			//Class
			output << "//Class\n";
			auto classIt = file.reflectedClasses.begin();
			while (classIt != file.reflectedClasses.end())
			{

				WriteClass(output, *classIt->second);
				++classIt;
			}
			

			//Bind to base classes
			//Functions, variables
			classIt = file.reflectedClasses.begin();
			while (classIt != file.reflectedClasses.end())
			{
				ReflectionClass& rClass = *classIt->second;

				std::vector<ReflectionClass*>& list = rClass.baseClasses;
				//if (!rClass.isComponent)
				if (list.empty())
				{
					++classIt;
					continue;
				}
				
				std::unordered_set<unsigned int> subReflect = {};
				{
					for (size_t i = 0; i < rClass.functions.size(); i++)
					{
						subReflect.insert(rClass.functions.at(i)->id);
					}
					for (size_t i = 0; i < rClass.variables.size(); i++)
					{
						subReflect.insert(rClass.variables.at(i)->id);
					}
				}

				output << "// " << rClass.name << "\n";
				output << "{\n";	//Scope Start
				
				output << CUFR_CLASS_TEXT << "* " << ReflectName_Class <<  " = static_cast<" << CUFR_CLASS_TEXT <<"*>(myReflections.at(" << rClass.globalID << ")); \n";
				output << ReflectName_Class << ";\n";

				//This is awful but I need more time to clean this up
				for (size_t i = 0; i < list.size(); ++i)
				{
					std::string baseClassName = list.at(i)->GetFullName();
					auto findClass = inReflector.myReflectedClasses.find(baseClassName);

					if (findClass == inReflector.myReflectedClasses.end()) continue;


					const ReflectionClass& subClass = findClass->second.reflectClass;
					for (size_t i = 0; i < subClass.functions.size(); ++i)
					{
						unsigned int id = subClass.functions.at(i)->id;
						auto find = subReflect.find(id);

						if (find != subReflect.end()) continue;

						subReflect.insert(id);
						output << ReflectName_Class << "->functions.push_back(static_cast<" << CUFR_FUNCTION_TEXT <<"*>(myReflections.at(" << subClass.functions.at(i)->globalID << "))	); //" << subClass.functions.at(i)->GetFullName() << "\n";


					}
					for (size_t i = 0; i < subClass.variables.size(); ++i)
					{
						unsigned int id = subClass.variables.at(i)->id;
						auto find = subReflect.find(id);

						if (find != subReflect.end()) continue;

						subReflect.insert(id);
						output << ReflectName_Class << "->variables.push_back(static_cast<" << CUFR_VARIABLE_TEXT <<"*>(myReflections.at(" << subClass.variables.at(i)->globalID << "))	); //" << subClass.variables.at(i)->GetFullName() << "\n";
					}
				}

				output << "}\n";	//Scope end
				++classIt;
			}


			output << "\n";
		}

		output << "}";

	}
}

int main()
{
	const char* reflectHeaderfolderPath = "C:/Program Files/Game/source/Special/Components/"; // Folder file to analyze
	const char* args[]					= { "-x", "c++", "-std=c++17" };
	const char* reflectFileTypes[]		= { ".h", ".hpp" };

	std::filesystem::path path = reflectHeaderfolderPath;


	if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
	{
		std::cout << "Path didn't exist: " << path.string().c_str() << std::endl;
		return 0;
	}

	Reflector reflector;
	
	for (auto& file : std::filesystem::directory_iterator(path))
	{
		if (!file.is_regular_file()) continue;

		bool reflect = false;
		std::string extension = file.path().extension().string();
		for (size_t i = 0; i < sizeof(reflectFileTypes) / sizeof(const char*); ++i)
		{
			if (extension == reflectFileTypes[i])
			{
				reflect = true;
				break;
			}
		}

		if (!reflect) continue;

		reflector.ReflectHeaderFile(file.path().string().c_str(), sizeof(args) / sizeof(const char*), args);
	}
	//Create Header files

	//Create reflection folder
	std::filesystem::path reflectFolderPath = path.string() + "/Reflect";
	std::filesystem::create_directory(reflectFolderPath);

	GenerateReflection(reflectFolderPath.string(), reflector);

	return 0;

}

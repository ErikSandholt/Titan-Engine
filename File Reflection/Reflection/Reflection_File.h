#pragma once
#include <string>
#include <unordered_map>
//#include "Reflection_Class.h"
#include "../Shared_Reflection_Data/ReflectionClass.h"


struct Reflection_File
	{
		std::string												path						= "";
		std::string												name						= "";
		std::unordered_map<std::string, ReflectionClass*>		reflectedClasses			= {};
		std::unordered_map<std::string, ReflectionFunction*>	reflectedGlobalFunctions	= {};
		std::unordered_map<std::string, ReflectionVariable*>	reflectedGlobalVariables	= {};

		bool IsEmpty() const
		{
			if(	reflectedClasses.empty()			&&
				reflectedGlobalFunctions.empty()	&&
				reflectedGlobalVariables.empty()		)
			{
				return true;
			}
			else
			{
				bool hasNoData = true;
				for (auto& classData : reflectedClasses)
				{
					if (!classData.second->functions.empty() ||
						!classData.second->variables.empty())
					{
						hasNoData = false;
						break;
					}
				}

				return hasNoData;
			}
		}
	};

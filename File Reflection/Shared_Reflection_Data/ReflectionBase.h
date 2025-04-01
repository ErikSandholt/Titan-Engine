#pragma once
#include <string>
#include <vector>
#include "BitMask.hpp"
#include "ReflectionType.h"


struct ReflectionBase
{
	std::vector<std::string>		nameSpace	= {};
	std::string						name		= "";

	//DecalType to get digit 
	//0 = end of decal
	BitMask							decal		= {};

	//ID is the name Hashed with Fnv1a_Hash
	unsigned int					globalID	= 0;
	unsigned int					id			= 0;

	ReflectionType					type		= {};

	virtual std::string GetFullName() const
	{

		std::string fullName = GetNameSpace();

		if (!fullName.empty()) fullName += "::";

		fullName += name;
		return fullName;
	}

	std::string GetNameSpace() const
	{
		std::string nameSpaceText;

		const size_t size = nameSpace.size();
		for (size_t i = 0; i < size; ++i)
		{
			nameSpaceText += nameSpace.at(i);

			if (i < size - 1)
			{
				nameSpaceText += "::";
			}
		}

		return nameSpaceText;
	}
};
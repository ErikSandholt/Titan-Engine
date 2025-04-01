#pragma once
#include "ReflectionFunction.h"


struct ReflectionClass : public ReflectionBase
{
public:
	//std::vector<std::string>			subClasses = {};

	virtual std::string GetFullName() const override
	{

		std::string fullName = GetNameSpace();

		if (!fullName.empty()) fullName += "::";

		fullName.insert(0, "class ");

		fullName += name;
		return fullName;
	}

	std::vector<ReflectionVariable*>	variables	= {};
	std::vector<ReflectionFunction*>	functions	= {};
	std::vector<ReflectionClass*>		baseClasses	= {};

};
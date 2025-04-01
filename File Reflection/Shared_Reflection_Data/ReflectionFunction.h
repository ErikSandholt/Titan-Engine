#pragma once
#include "ReflectionVariable.h"
#include "CallEvent.hpp"


struct ReflectionFunction : public ReflectionBase
{
public:
	ReflectionType						returnValue = {};
	std::vector<ReflectionVariable>		arguments	= {};

#ifndef REFLECT_EXCLUDE_INVOKE
	Sunder::CallEvent					invoke = {};
#endif // REFLECT_EXCLUDE_INVOKE

};
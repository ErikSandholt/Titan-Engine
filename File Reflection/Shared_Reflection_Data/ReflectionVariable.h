#pragma once
#include "ReflectionBase.h"
#include "CallEvent.hpp",

struct ReflectionVariable : public ReflectionBase
{
public:
	ReflectionType					valueType = {};

#ifndef REFLECT_EXCLUDE_INVOKE
	Sunder::CallEvent				invoke = {};
#endif // REFLECT_EXCLUDE_INVOKE
};
#pragma once
#include "BitMask.hpp"


enum class R_Type
{
	NONE		,
	Void		,
	Bool		,
	Char		,
	WChar		,
	Int			,
	UInt		,
	Float		,
	Double		,
	Long		,
	Enum		,

	Class		,
	Struct		,
	Variable	,
	Argument	,
	Function	,
	Lambda		,

};

constexpr const char* R_Type_S[]
{
	"NONE"		,
	"Void"		,
	"Bool"		,
	"Char"		,
	"WChar"		,
	"Int"		,
	"UInt"		,
	"Float"		,
	"Double"	,
	"Long"		,
	"Enum"		,
		
	"Class"		,
	"Struct"	,
	"Lambda"	,
	"Function"	,
};

//9 Digits MAX
enum class DecalType
{
	NONE		,
	Public		,
	Protected	,
	Private		,
	Static		,
	Global		,
	Const		,
	Array		,
	Pointer		,
	Reference
};

constexpr const char* DecalType_S[]
{
	"NONE"		,
	"Public"	,
	"Protected"	,
	"Private"	,
	"Static"	,
	"Global"	,
	"Const"		,
	"Array"		,
	"Pointer"	,
	"Reference"
};

struct ReflectionBase;

struct ReflectionType
{
	//DecalType to get digit 
	//0 = end
	BitMask							decal		= {};	
	R_Type							type		= R_Type::NONE;


	ReflectionBase*					data		= nullptr;
};
#pragma once
#include <vector>
#include "GamePointer.hpp"
#include "Object.h"

//#pragma push_macro("GetObject")
#undef GetObject

class IGameDataList
{
public:
	IGameDataList()		= default;
	~IGameDataList()	= default;


	virtual std::vector< GamePointer<GameSystem::Object> > GetDataList()					= 0;
	virtual GameSystem::Object* GetObject(int aID)											= 0;
	virtual GameSystem::Object* GetObject(int aIndex, int aGeneration)						= 0;
	virtual GamePointer< GameSystem::Object> GetGamePointer(int aID)						= 0;
	virtual GamePointer< GameSystem::Object> GetGamePointer(int aIndex, int aGeneration)	= 0;
	virtual const GameSystem::Object* GetTypeChecker() const								= 0;
	virtual bool HasKey(int aIndex, int aGeneration)										= 0;

protected:
	virtual void Merge()																	= 0; //Add The Buffer to components
	virtual void Cleanup()																	= 0; //Clean up the components
private:

};

//#pragma pop_macro("GetObject")
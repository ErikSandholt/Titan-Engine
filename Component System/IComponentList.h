#pragma once
#include "../IGameDataList.h"
#include "Component.h"

class ComponentListManager;

class IComponentList : public IGameDataList
{
public:
	friend class ComponentListManager;

	virtual ~IComponentList() = default;

	virtual GamePointer<GameSystem::Object> CreateObject()									= 0;
	virtual void Destroy(int aIndex, int aGeneration)										= 0;
	virtual void GetListFromSub(std::vector< GamePointer<Component> >& aOutList)			= 0;
	std::vector< GamePointer<GameSystem::Object> > GetDataList() override					= 0;
	GameSystem::Object* GetObject(int aID) override											= 0;
	GameSystem::Object* GetObject(int aIndex, int aGeneration) override						= 0;
	GamePointer< GameSystem::Object> GetGamePointer(int aID) override						= 0;
	GamePointer< GameSystem::Object> GetGamePointer(int aIndex, int aGeneration) override	= 0;
	const GameSystem::Object* GetTypeChecker() const override								= 0;

protected:
	bool HasKey(int aIndex, int aGeneration) override										= 0;
	void Merge() override																	= 0; //Add The Buffer to components
	void Cleanup() override																	= 0; //Clean up the components
	virtual void Start()																	= 0;
	virtual void Update()																	= 0;
	virtual void LateUpdate()																= 0;
	virtual void DrawGizmo()																= 0;
};
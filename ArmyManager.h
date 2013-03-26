#pragma once
#include "source\ExampleAIModule.h"

class ArmyManager
{
protected:
	std::vector<BWAPI::Unit*> army;
	ExampleAIModule* ai_module;
	BWAPI::Position current_attack;
public:
	ArmyManager(void);
	~ArmyManager(void);
	ArmyManager(ExampleAIModule*);
	virtual void addArmyUnit(BWAPI::Unit*);
	virtual void heartbeat();
};

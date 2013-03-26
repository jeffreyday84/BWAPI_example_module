#pragma once
#include "source\BaseAIModule.h"

class ArmyManager
{
protected:
	std::vector<BWAPI::Unit*> army;
	BaseAIModule* ai_module;
	BWAPI::Position current_attack;
public:
	ArmyManager(void);
	~ArmyManager(void);
	ArmyManager(BaseAIModule*);
	virtual void addArmyUnit(BWAPI::Unit*);
	virtual void heartbeat();
};

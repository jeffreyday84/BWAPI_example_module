#pragma once
#include "BaseAIModule.h"

class ArmyManager
{
protected:
	std::vector<BWAPI::Unit*> army;
	BaseAIModule* ai_module;
	BWAPI::Position current_attack;
	BWAPI::Unit* next_attack;
	void setNextAttack();
public:
	ArmyManager(void);
	~ArmyManager(void);
	ArmyManager(BaseAIModule*);
	virtual void addArmyUnit(BWAPI::Unit*);
	virtual void heartbeat();
};

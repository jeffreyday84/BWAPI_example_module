#pragma once
#include "BaseAIModule.h"

class ArmyManager : public HeartbeatEventHandler
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
	void onStart();
	virtual void addArmyUnit(BWAPI::Unit*);
	virtual void onHeartbeat();
};

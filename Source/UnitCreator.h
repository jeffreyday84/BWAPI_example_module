#pragma once
#include "BaseAIModule.h"

class UnitCreator
{
protected:
	std::vector<BWAPI::Unit*> hatcheries;
	BaseAIModule* ai_module;
public:
	UnitCreator(void);
	~UnitCreator(void);
	UnitCreator(BaseAIModule* module);
	bool hasLarva();
	bool hasFullLarva();
	int makeUnit(BWAPI::UnitType type, int num = 1);
	int makeWorker(int num = 1);
	int makeOverlord(int num = 1);
	void addProducer(BWAPI::Unit*);
	bool buildHatchery();
	bool buildSpawningPool();
	bool buildGeyser();
};

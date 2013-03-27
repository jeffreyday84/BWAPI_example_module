#pragma once
#include "Source\BaseAIModule.h"

class StrategyManager
{
protected:
	BaseAIModule* ai_module;
	std::queue<BWAPI::UnitType> build_queue;
	std::set<int> attacked_overlords;
	int workers_morphing;
	bool need_spawning_pool;
	bool starting_pool;
	bool spawning_pool_finished;
	bool slow_workers;
	bool stop_workers;
	bool overlord_building;
	bool hatch_built;
	bool hatch_started;
public:
	StrategyManager(void);
	~StrategyManager(void);
	StrategyManager(BaseAIModule*);
	void onStart();
	void onUnitReady(BWAPI::Unit*);
	void onUnitAttacked(BWAPI::Unit*);
	void onFrame();
};

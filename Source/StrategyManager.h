#pragma once
#include "BaseAIModule.h"

class FrameEventHandler;
class UnitReadyEventHandler;
class StrategyManager : public FrameEventHandler, UnitReadyEventHandler
{
protected:
	BaseAIModule* ai_module;
	std::queue<BWAPI::UnitType> build_queue;
	std::set<int> attacked_overlords;
	int workers_morphing; 
	int workers_since_pool;
	bool need_spawning_pool;
	bool starting_pool;
	bool spawning_pool_finished;
	bool slow_workers;
	bool stop_workers;
	bool overlord_building;
	bool hatch_built;
	bool hatch_started;
	bool starting_geyser;
public:
	StrategyManager(void);
	~StrategyManager(void);
	StrategyManager(BaseAIModule*);
	void onStart();
	void onUnitReady(BWAPI::Unit*);
	void onUnitAttacked(BWAPI::Unit*);
	void onFrame();
};

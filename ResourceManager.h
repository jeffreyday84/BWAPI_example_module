#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
//#include "source\BaseAIModule.h"

class BaseAIModule;
class ResourceManager
{
	friend bool sortByDistanceFromBase(BWAPI::Unit *unit1, BWAPI::Unit *unit2);
protected:
	BaseAIModule* ai_module;
	std::vector<BWAPI::Unit*> minerals_to_work;
	std::vector<BWAPI::Unit*> geysers_to_work;
	std::vector<BWAPI::Unit*> finished_geysers;
	std::vector<BWAPI::Unit*> workers;
	static std::vector<BWAPI::Unit*> bases;
	int min_min;
	BWAPI::Unit* default_mineral;
	bool gather_gas;
	std::map<BWAPI::Unit*, BWAPI::Unit*> gas_workers;
public:
	const unsigned static int FIND_MINERAL_DISTANCE = 220;
	ResourceManager();
	ResourceManager(BaseAIModule* uc);
	void addBase(BWAPI::Unit* unit);
	void addFinishedGeyser(BWAPI::Unit*);
	void setGatherGas(bool);
	void init();
	void addWorker(BWAPI::Unit *unit);
	void heartbeat();
	BWAPI::Unit* getWorker();
	BWAPI::Unit* getGeyser();
	int getNumMinsToWork();
	int getTargetMinsToWork();
	int getNumWorkers();
};

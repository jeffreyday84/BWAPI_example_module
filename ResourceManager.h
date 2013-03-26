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
	std::vector<BWAPI::Unit*> workers;
	static std::vector<BWAPI::Unit*> bases;
	int min_min;
public:
	const unsigned static int FIND_MINERAL_DISTANCE = 220;
	ResourceManager();
	ResourceManager(BaseAIModule* uc);
	void addBase(BWAPI::Unit* unit);
	void init();
	void addWorker(BWAPI::Unit *unit);
	BWAPI::Unit* getWorker();
	int getNumMinsToWork();
	int getTargetMinsToWork();
};

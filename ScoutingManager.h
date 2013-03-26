#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include "source\ExampleAIModule.h"

class ScoutingManager
{
protected:
	std::vector<BWAPI::Unit*> overlords;
	std::vector<BWAPI::Unit*> scouts;
	ExampleAIModule* ai_module;
	void sendOverlords();
public:
	ScoutingManager(void);
	ScoutingManager(ExampleAIModule*);
	void addScout(BWAPI::Unit*);
	void heartbeat();
};

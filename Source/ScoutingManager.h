#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include "BaseAIModule.h"

class ScoutingManager : public HeartbeatEventHandler
{
protected:
	std::vector<BWAPI::Unit*> overlords;
	std::vector<BWAPI::Unit*> scouts;
	BaseAIModule* ai_module;
	void sendOverlords();
public:
	ScoutingManager(void);
	ScoutingManager(BaseAIModule*);
	void onStart();
	void addScout(BWAPI::Unit*);
	void onHeartbeat();
};

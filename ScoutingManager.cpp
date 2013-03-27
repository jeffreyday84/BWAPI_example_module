#pragma once
#include "ScoutingManager.h"
//#include "source\BaseAIModule.h"
using namespace BWAPI;

ScoutingManager::ScoutingManager(void)
{
}

ScoutingManager::ScoutingManager(BaseAIModule* parent_module)
{
	ai_module = parent_module;
}

void ScoutingManager::addScout(Unit* new_scout)
{
	if(new_scout->getType().getID() == UnitTypes::Zerg_Overlord)
	{
		overlords.push_back(new_scout);
		sendOverlords();
	}
	else
		Broodwar->printf("No overlord here! I don't know what to do with this.");
}

void ScoutingManager::sendOverlords()
{
	Unit* overlord = overlords.back();
	if(overlords.size() <= ai_module->getMapLocations()->getOverlordPositions().size())
	{
		overlord->rightClick(*ai_module->getMapLocations()->getOverlordPositions()[overlords.size() - 1]);
	}
	else
	{
		overlord->rightClick(ai_module->getMapLocations()->getDefaultOverlordPosition());
	}
}

void ScoutingManager::heartbeat()
{
	for(int i = overlords.size() - 1; i >= 0; i--)
	{
		if(overlords[i]->getHitPoints() < 199)
		{
			if(overlords[i]->getHitPoints() > 0)
			{
				Broodwar->printf("Overlord is damaged");
				ai_module->onUnitAttacked(overlords[i]);
			}
			else
			{
				overlords.erase(overlords.begin() + i);
			}
		}
	}
}
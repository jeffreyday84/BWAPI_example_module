#pragma once
#include "source\ExampleAIModule.h"

class MapLocations
{
protected:
	ExampleAIModule* ai_module;
	std::vector<BWAPI::TilePosition*> hatchery_positions;
	BWAPI::TilePosition pool_position;
	BWAPI::Position default_attack_position;
	BWAPI::Position default_overlord_position;
	std::vector<BWAPI::Position*> overlord_positions;
public:
	MapLocations(void);
	~MapLocations(void);
	MapLocations(ExampleAIModule*);
	std::vector<BWAPI::TilePosition*>& getHatcheryPositions();
	BWAPI::TilePosition* popHatcheryPosition();
	BWAPI::TilePosition getPoolPosition();
	BWAPI::Position getDefaultAttackPosition();
	BWAPI::Position getDefaultOverlordPosition();
	std::vector<BWAPI::Position*>& getOverlordPositions();
};

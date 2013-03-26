#include "MapLocations.h"
using namespace BWAPI;

MapLocations::MapLocations(void)
{
}

MapLocations::~MapLocations(void)
{
}

MapLocations::MapLocations(BaseAIModule* module)
{
	ai_module = module;
	TilePosition main_base = ai_module->getMainBase()->getTilePosition();

	for(std::set< TilePosition >::iterator i = Broodwar->getStartLocations().begin(); i != Broodwar->getStartLocations().end(); i++)
	{
		if(*i != Broodwar->self()->getStartLocation())
		{
			overlord_positions.push_back(new Position(*i));
			default_attack_position = Position(*i);
		}
	}

	if(main_base.x() < 20)
	{
		hatchery_positions.push_back(new TilePosition(main_base.x() + 6, main_base.y() + 6));
		hatchery_positions.push_back(new TilePosition(main_base.x() + 12, main_base.y()));
		hatchery_positions.push_back(new TilePosition(main_base.x() + 6, main_base.y()));
		pool_position = TilePosition(main_base.x(), main_base.y() - 2);
		default_overlord_position = Position(TilePosition(3,7));
	} 
	else 
	{
		hatchery_positions.push_back(new TilePosition(main_base.x() - 18, main_base.y()));
		hatchery_positions.push_back(new TilePosition(main_base.x() - 12, main_base.y()));
		hatchery_positions.push_back(new TilePosition(main_base.x() - 6, main_base.y()));
		pool_position = TilePosition(main_base.x(), main_base.y() - 2);
		default_overlord_position = Position(TilePosition(89,83));
	}
}

std::vector<BWAPI::TilePosition*>& MapLocations::getHatcheryPositions()
{
	return hatchery_positions;
}

TilePosition* MapLocations::popHatcheryPosition()
{
	TilePosition* retval = hatchery_positions.back();
	hatchery_positions.pop_back();
	return retval;
}

BWAPI::TilePosition MapLocations::getPoolPosition()
{
	return pool_position;
}

BWAPI::Position MapLocations::getDefaultAttackPosition()
{
	return default_attack_position;
}

BWAPI::Position MapLocations::getDefaultOverlordPosition()
{
	return default_overlord_position;
}

std::vector<BWAPI::Position*>& MapLocations::getOverlordPositions()
{
	return overlord_positions;
}
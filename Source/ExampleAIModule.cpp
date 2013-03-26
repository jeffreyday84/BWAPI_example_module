#include "BaseAIModule.h"
#include "..\ResourceManager.h"
#include "..\ScoutingManager.h"
#include "..\MapLocations.h"
using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
std::vector<BWAPI::Unit*> minerals_to_work;
std::vector<BWAPI::Unit*> hatcheries;
std::map<BWAPI::Unit*,BWAPI::Unit*> worker_mineral_orders;
bool overlord_building = false;

void BaseAIModule::onStart()
{
	workers_morphing = 0;
	need_spawning_pool = true;
	spawning_pool_finished = slow_workers = stop_workers = starting_pool = false;
	spawning_pool = 0;
	Broodwar->sendText("Hello world! Jeff here!");
	// Enable some cheat flags
	Broodwar->enableFlag(Flag::UserInput);
	// Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	if (Broodwar->isReplay())
	{
		throw "This is a replay jackass";
	}
	else
	{
		resource_manager = new ResourceManager(this);
		scouting_manager = new ScoutingManager(this);

		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if ((*i)->getType().isResourceDepot())
			{
				resource_manager->addBase(*i);
				hatcheries.push_back(*i);
				main_base = *i;
			}
		}

		map_locations = new MapLocations(this);
		army_manager = new ArmyManager(this);

		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if ((*i)->getType().isWorker())
			{
				resource_manager->addWorker(*i);
			}
			else if ((*i)->getType().getID() == UnitTypes::Zerg_Overlord)
			{
				scouting_manager->addScout(*i);
			}
		}
	}
}

bool BaseAIModule::hasFullLarva()
{
	bool retval = false;
	for(unsigned int i = 0; i < hatcheries.size() && !retval; ++i)
	{
		std::set<Unit*> larva = hatcheries[i]->getLarva();
		if(larva.size() == 3) {
			retval = true;
		}
	}
	return retval;
}

bool BaseAIModule::hasLarva()
{
	bool retval = false;
	for(unsigned int i = 0; i < hatcheries.size() && !retval; ++i)
	{
		std::set<Unit*> larva = hatcheries[i]->getLarva();
		if(larva.size() > 0) {
			retval = true;
		}
	}
	return retval;
}

int BaseAIModule::make_unit(UnitType type, int num) 
{
	int retval = 0;
	for(unsigned int i = 0; i < hatcheries.size() && num > 0; ++i)
	{
		std::set<Unit*> larva = hatcheries[i]->getLarva();
		if(larva.size() > 0) {
			for(std::set<Unit*>::const_iterator l=larva.begin();l!=larva.end() && num > 0;l++)
			{
				BWAPI::Unit *this_larva = *l;
				if(this_larva->morph(type))
				{
					retval++;
					num--;
				}
			}
		}
	}
	return retval;
}

int BaseAIModule::make_overlord(int num)
{
	return make_unit(UnitTypes::Zerg_Overlord, num);
}

int BaseAIModule::make_worker(int num)
{
	return make_unit(UnitTypes::Zerg_Drone, num);
}

void BaseAIModule::onUnitReady(Unit* unit)
{
	Broodwar->printf("%s ready",unit->getType().getName().c_str());
	if(unit->getType().getID() == UnitTypes::Zerg_Zergling)
	{
		army_manager->addArmyUnit(unit);
	} 
	else if (unit->getType().isWorker())
	{
		workers_morphing--;
		resource_manager->addWorker(unit);
	}
	else if (unit->getType().getID() == UnitTypes::Zerg_Spawning_Pool)
	{
		spawning_pool_finished = true;
		stop_workers = true;
		Broodwar->printf("Spawning pool finished");
	}
	else if (unit->getType().getID() == UnitTypes::Zerg_Overlord)
	{
		scouting_manager->addScout(unit);
	}
	else if (unit->getType().getID() == UnitTypes::Zerg_Hatchery)
	{
		hatcheries.push_back(unit);
	}
}

void BaseAIModule::checkReadinessQueue()
{
	for(int i = unit_readiness_queue.size() - 1; i >= 0; i--)
	{
		if(
			(
				!unit_readiness_queue[i]->getType().isBuilding() && 
				unit_readiness_queue[i]->canIssueCommand(UnitCommand::stop(unit_readiness_queue[i]))
			) ||
			(unit_readiness_queue[i]->getType().isBuilding() && !unit_readiness_queue[i]->isMorphing())
		)
		{
			onUnitReady(unit_readiness_queue[i]);
			unit_readiness_queue.erase(unit_readiness_queue.begin() + i);
		}
	}
}

void BaseAIModule::onOverlordAttacked(Unit* unit)
{
	int before = attacked_overlords.size();
	attacked_overlords.insert(unit->getID());
	if(before != attacked_overlords.size()) {
		build_queue.push(UnitTypes::Zerg_Overlord);
	}
}

void BaseAIModule::onFrame()
{
	checkReadinessQueue();
	if(Broodwar->getFrameCount() > 1) 
	{
		if(Broodwar->getFrameCount()%12==0) scouting_manager->heartbeat();

		if(build_queue.size())
		{
			if(make_unit(build_queue.front()))
			{
				build_queue.pop();
			}
		}
		else
		{
			if((resource_manager->getTargetMinsToWork() + workers_morphing) < resource_manager->getNumMinsToWork())
			{
				workers_morphing += make_worker();
			}
			if(Broodwar->self()->supplyTotal() <= Broodwar->self()->supplyUsed()) {
				if(Broodwar->self()->minerals() >= 100 && !overlord_building) {
					if(make_overlord()) overlord_building = true;
				}
			} else if(overlord_building) {
				overlord_building = false;
			}

			if(!stop_workers)
			{
				if(slow_workers && workers_morphing == 3)
				{
					stop_workers = true;
				}
				else if(resource_manager->getNumMinsToWork() > workers_morphing && Broodwar->self()->minerals() >= 50 && Broodwar->self()->supplyTotal() > Broodwar->self()->supplyUsed())
				{
					workers_morphing += make_worker();
				}
			}

			if(spawning_pool_finished)
			{
				make_unit(UnitTypes::Zerg_Zergling, 3);
			}

			if(
					(starting_pool && need_spawning_pool) || 
					(need_spawning_pool && Broodwar->self()->minerals() >= 180)
			) {
				need_spawning_pool = !createSpawningPool();
				starting_pool = true;
				stop_workers = true;
				slow_workers = true;
				if(!need_spawning_pool)
				{
					starting_pool = false;
					stop_workers = false;
				}
			}

			if(Broodwar->self()->minerals() > 350 && Broodwar->self()->supplyUsed() >= 14)
			{
				makeHatchery();
			}
		}
	}

	if(Broodwar->getFrameCount()%30==0) {
		for(unsigned int i = 0; i < unit_readiness_queue.size(); i++)
		{
			Broodwar->printf("Pending unit %i of %i type %s",
				i+1,
				unit_readiness_queue.size(),
				unit_readiness_queue[i]->getType().getName().c_str()
			);
		}
		Broodwar->printf("Have %i minerals of target %i minerals unworked",resource_manager->getNumMinsToWork(), resource_manager->getTargetMinsToWork());
	}
}

bool BaseAIModule::createSpawningPool()
{
	static Position* worker_position;
	static Unit* worker;
	bool retval = false;
	if(!worker_position)
	{
		TilePosition base_pos = main_base->getTilePosition();
		worker_position = new Position(TilePosition(map_locations->getPoolPosition().x() + 2, map_locations->getPoolPosition().y() + 1));
	}
	if(!worker)
	{
		worker = resource_manager->getWorker();
	}
	if(!worker->getType().isWorker()) {
		retval = true;
		delete worker_position;
		worker_position = 0;
	} else {
		if(worker && Broodwar->self()->minerals() < 200 && worker->getOrder().getID() != Orders::Move)
		{
			worker->rightClick(*worker_position);
		} else if(worker && Broodwar->self()->minerals() >= 200) {
			worker->build(map_locations->getPoolPosition(), UnitTypes::Zerg_Spawning_Pool);
		}
	}
	return retval;
}

void BaseAIModule::makeHatchery()
{
	static Unit* worker = 0;
	static int tile_offset = 0;
	static TilePosition* position = 0;

	if(worker && !worker->getType().isWorker())
	{
		worker = 0;
		delete position;
		position = 0;
	}

	if(tile_offset == 0)
	{
		if(main_base->getTilePosition().x() < 20)
		{
			tile_offset = 6;
		}
		else
		{
			tile_offset = -6;
		}
	}

	if(!worker)
	{
		worker = resource_manager->getWorker();
		if(worker)
		{
			position = map_locations->popHatcheryPosition();
		}
		if(worker)
		{
			worker->move(Position(*position));
		}
	}
	
	if(worker)
	{
		worker->build(*position, UnitTypes::Zerg_Hatchery);
	}
}

/**
 * Begin getters
 */
BaseAIModule* BaseAIModule::getUnitCreator()
{
	return this;
}

ResourceManager* BaseAIModule::getResourceManager()
{
	return resource_manager;
}

ScoutingManager* BaseAIModule::getScoutingManager()
{
	return scouting_manager;
}

MapLocations* BaseAIModule::getMapLocations()
{
	return map_locations;
}

BWAPI::Unit* BaseAIModule::getMainBase()
{
	return main_base;
}

/**
 * Begin pre-ready unit declarations
 */
void BaseAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	Broodwar->printf("%s morphed",unit->getType().getName().c_str());
	if(unit->getPlayer() == Broodwar->self() && Broodwar->getFrameCount()>1)
	{
		if(unit->getType().canMove() || unit->getType().isBuilding()) {
			unit_readiness_queue.push_back(unit);
		}
	}
}

void BaseAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	Broodwar->printf("%s created",unit->getType().getName().c_str());
	if(unit->getPlayer() == Broodwar->self() && Broodwar->getFrameCount()>1)
	{
		if(unit->getType().canMove() || unit->getType().isBuilding()) {
			unit_readiness_queue.push_back(unit);
		}
	}
}

/**
 * Begin pro forma declarations
 */
void BaseAIModule::onUnitComplete(BWAPI::Unit *unit)
{

}

void BaseAIModule::onEnd(bool isWinner)
{
	if (isWinner)
	{
		//log win to file
	}
}

void BaseAIModule::onSendText(std::string text)
{

}

void BaseAIModule::sendMessage(const char* text)
{
	Broodwar->printf(text);
}

void BaseAIModule::onReceiveText(BWAPI::Player* player, std::string text)
{

}

void BaseAIModule::onPlayerLeft(BWAPI::Player* player)
{

}

void BaseAIModule::onNukeDetect(BWAPI::Position target)
{

}

void BaseAIModule::onUnitDiscover(BWAPI::Unit* unit)
{

}

void BaseAIModule::onUnitEvade(BWAPI::Unit* unit)
{

}

void BaseAIModule::onUnitShow(BWAPI::Unit* unit)
{

}

void BaseAIModule::onUnitHide(BWAPI::Unit* unit)
{

}

void BaseAIModule::onUnitDestroy(BWAPI::Unit* unit)
{

}

void BaseAIModule::onUnitRenegade(BWAPI::Unit* unit)
{

}

void BaseAIModule::onSaveGame(std::string gameName)
{

}


void BaseAIModule::drawStats()
{

}

void BaseAIModule::drawBullets()
{

}

void BaseAIModule::drawVisibilityData()
{

}

void BaseAIModule::drawTerrainData()
{

}

void BaseAIModule::showPlayers()
{

}

void BaseAIModule::showForces()
{

}


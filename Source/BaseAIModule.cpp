#include "BaseAIModule.h"
#include "ResourceManager.h"
#include "ScoutingManager.h"
#include "MapLocations.h"
using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
std::vector<BWAPI::Unit*> minerals_to_work;
std::map<BWAPI::Unit*,BWAPI::Unit*> worker_mineral_orders;
bool overlord_building = false;

/**
 * Begin game state functions
 */
BWAPI::Unit* BaseAIModule::getSpawningPool()
{
	return spawning_pool;
}

void BaseAIModule::setSpawningPool(BWAPI::Unit* unit)
{
	spawning_pool = unit;
}

/**
 * Begin manager getters
 */
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

ArmyManager* BaseAIModule::getArmyManager()
{
	return army_manager;
}

UnitCreator* BaseAIModule::getUnitCreator()
{
	return unit_creator;
}

Player* BaseAIModule::getEnemyPlayer()
{
	if(!enemy_player)
	{
		for(
				std::set<Player*>::const_iterator i = Broodwar->getPlayers().begin(); 
				i != Broodwar->getPlayers().end() && !enemy_player;
				i++
		)
		{
			if((*i)->isEnemy(Broodwar->self()))
			{
				enemy_player = *i;
			}
		}
	}
	return enemy_player;
}

/**
 * Event management functions
 */
void BaseAIModule::addFrameHandler(FrameEventHandler* handler)
{
	removeFrameHandler(handler);
	frame_event_handlers.push_back(handler);
};

void BaseAIModule::removeFrameHandler(FrameEventHandler* handler)
{
	for(unsigned int i = 0; i < frame_event_handlers.size(); i++)
	{
		if(frame_event_handlers[i] == handler)
		{
			frame_event_handlers.erase(frame_event_handlers.begin() + i);
			return;
		}
	}
};

void BaseAIModule::addHeartbeatHandler(HeartbeatEventHandler* handler)
{
	removeHeartbeatHandler(handler);
	heartbeat_event_handlers.push_back(handler);
};


void BaseAIModule::removeHeartbeatHandler(HeartbeatEventHandler* handler)
{
	for(unsigned int i = 0; i < heartbeat_event_handlers.size(); i++)
	{
		if(heartbeat_event_handlers[i] == handler)
		{
			heartbeat_event_handlers.erase(heartbeat_event_handlers.begin() + i);
			return;
		}
	}
};


void BaseAIModule::addUnitReadyHandler(UnitReadyEventHandler* handler)
{
	removeUnitReadyHandler(handler);
	unit_ready_event_handlers.push_back(handler);
};


void BaseAIModule::removeUnitReadyHandler(UnitReadyEventHandler* handler)
{
	for(unsigned int i = 0; i < unit_ready_event_handlers.size(); i++)
	{
		if(unit_ready_event_handlers[i] == handler)
		{
			unit_ready_event_handlers.erase(unit_ready_event_handlers.begin() + i);
			return;
		}
	}
};



/**
 * Begin used events and their internal functions
 */
void BaseAIModule::onStart()
{
	// Begin!
	if (Broodwar->isReplay())
	{
		throw "This is a replay and unsupported by this AI.";
	}
	Broodwar->sendText("Hello world! After Unit Creator refactor!");
	Broodwar->enableFlag(Flag::UserInput);

	enemy_player = 0;

	// Init managers and units
	resource_manager = new ResourceManager(this);
	scouting_manager = new ScoutingManager(this);
	strategy_manager = new StrategyManager(this);
	unit_creator = new UnitCreator(this);
	map_locations = new MapLocations(this);
	army_manager = new ArmyManager(this);

	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if ((*i)->getType().isResourceDepot())
		{
			resource_manager->addBase(*i);
			unit_creator->addProducer(*i);
		}
	}

	strategy_manager->onStart();
	scouting_manager->onStart();
	army_manager->onStart();
	resource_manager->onStart();
}

void BaseAIModule::onFrame()
{
	checkReadinessQueue();
	if(Broodwar->getFrameCount() > 1) 
	{
		for(unsigned int i = 0; i < frame_event_handlers.size(); i++)
		{
			frame_event_handlers[i]->onFrame();
		}
		for(
			unsigned int i = (Broodwar->getFrameCount()%FRAMES_PER_HEARTBEAT); 
			i < heartbeat_event_handlers.size();
			i += FRAMES_PER_HEARTBEAT
		)
		{
			heartbeat_event_handlers[i]->onHeartbeat();
		}
	}

	if(false && Broodwar->getFrameCount()%100==0) {
		for(unsigned int i = 0; i < unit_readiness_queue.size(); i++)
		{
			Broodwar->printf("Pending unit %i of %i type %s",
				i+1,
				unit_readiness_queue.size(),
				unit_readiness_queue[i]->getType().getName().c_str()
			);
		}
		Broodwar->printf(
							"Have %i minerals of target %i minerals unworked by %i workers",
							resource_manager->getNumMinsToWork(), 
							resource_manager->getTargetMinsToWork(),
							resource_manager->getNumWorkers()
		);
	}
}

void BaseAIModule::onUnitReady(Unit* unit)
{
	for(unsigned int i = 0; i < unit_ready_event_handlers.size(); i++)
	{
		unit_ready_event_handlers[i]->onUnitReady(unit);
	}
}

void BaseAIModule::onUnitAttacked(Unit* unit)
{
	strategy_manager->onUnitAttacked(unit);
}

/**
 * Begin forwarded events
 */
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

void BaseAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	//Broodwar->printf("%s morphed",unit->getType().getName().c_str());
	if(unit->getPlayer() == Broodwar->self() && Broodwar->getFrameCount()>1)
	{
		if(unit->getType().canMove() || unit->getType().isBuilding()) {
			unit_readiness_queue.push_back(unit);
		}
		else
		{
			onUnitReady(unit);
		}
	}
}

void BaseAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	//Broodwar->printf("%s created",unit->getType().getName().c_str());
	if(unit->getPlayer() == Broodwar->self() && Broodwar->getFrameCount()>1)
	{
		if(unit->getType().canMove() || unit->getType().isBuilding()) {
			unit_readiness_queue.push_back(unit);
		}
		else
		{
			onUnitReady(unit);
		}
	}
}

/**
 * Begin unused functions
 */
void BaseAIModule::onUnitComplete(BWAPI::Unit *unit) { }
void BaseAIModule::onEnd(bool isWinner) { if (isWinner) { /* log win to file */ } }
void BaseAIModule::onSendText(std::string text) { }
void BaseAIModule::sendMessage(const char* text) { }
void BaseAIModule::onReceiveText(BWAPI::Player* player, std::string text) { }
void BaseAIModule::onPlayerLeft(BWAPI::Player* player) { }
void BaseAIModule::onNukeDetect(BWAPI::Position target) { }
void BaseAIModule::onUnitDiscover(BWAPI::Unit* unit) { }
void BaseAIModule::onUnitEvade(BWAPI::Unit* unit) { }
void BaseAIModule::onUnitShow(BWAPI::Unit* unit) { }
void BaseAIModule::onUnitHide(BWAPI::Unit* unit) { }
void BaseAIModule::onUnitDestroy(BWAPI::Unit* unit) { }
void BaseAIModule::onUnitRenegade(BWAPI::Unit* unit) { }
void BaseAIModule::onSaveGame(std::string gameName) { }
void BaseAIModule::drawStats() { }
void BaseAIModule::drawBullets() { }
void BaseAIModule::drawVisibilityData() { }
void BaseAIModule::drawTerrainData() { }
void BaseAIModule::showPlayers() { }
void BaseAIModule::showForces() { }

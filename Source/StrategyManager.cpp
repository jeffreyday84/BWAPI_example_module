#include "StrategyManager.h"
using namespace BWAPI;

StrategyManager::StrategyManager(void)
{
}

StrategyManager::~StrategyManager(void)
{
}

StrategyManager::StrategyManager(BaseAIModule* module)
{
	ai_module = module;
	workers_morphing = 0;
	workers_since_pool = 0;
	need_spawning_pool = true;
	starting_pool = false;
	spawning_pool_finished = false;
	slow_workers = false;
	stop_workers = false;
	overlord_building = false;
	hatch_built = false;
	hatch_started = false;
	starting_geyser = false;
}

void StrategyManager::onStart()
{
}

void StrategyManager::onUnitReady(BWAPI::Unit* unit)
{
	//Broodwar->printf("%s ready",unit->getType().getName().c_str());
	if(unit->getType().getID() == UnitTypes::Zerg_Zergling)
	{
		ai_module->getArmyManager()->addArmyUnit(unit);
	} 
	else if (unit->getType().isWorker())
	{
		workers_morphing--;
		ai_module->getResourceManager()->addWorker(unit);
	}
	else if (unit->getType().getID() == UnitTypes::Zerg_Spawning_Pool)
	{
		spawning_pool_finished = true;
		stop_workers = true;
		ai_module->setSpawningPool(unit);
		Broodwar->printf("Spawning pool finished");
	}
	else if (unit->getType().getID() == UnitTypes::Zerg_Overlord)
	{
		ai_module->getScoutingManager()->addScout(unit);
	}
	else if (unit->getType().getID() == UnitTypes::Zerg_Hatchery)
	{
		ai_module->getUnitCreator()->addProducer(unit);
	}
	else if (unit->getType().getID() == UnitTypes::Zerg_Extractor)
	{
		ai_module->getResourceManager()->addFinishedGeyser(unit);
		ai_module->getResourceManager()->setGatherGas(true);
	}
}

void StrategyManager::onUnitAttacked(BWAPI::Unit* unit)
{
	if(unit->getType().getID() == UnitTypes::Zerg_Overlord)
	{
		int before = attacked_overlords.size();
		attacked_overlords.insert(unit->getID());
		if(before != attacked_overlords.size()) {
			build_queue.push(UnitTypes::Zerg_Overlord);
			unit->stop();
		}
	}
}

void StrategyManager::onFrame()
{
	if(Broodwar->self()->gas() >= 92) 
	{
		ai_module->getResourceManager()->setGatherGas(false);
		if(Broodwar->self()->gas() >= 100)
		{
			ai_module->getSpawningPool()->upgrade(UpgradeTypes::Metabolic_Boost);
		}
	}
	if(build_queue.size())
	{
		if(ai_module->getUnitCreator()->makeUnit(build_queue.front()))
		{
			build_queue.pop();
		}
	}
	else
	{
		if(!stop_workers && (ai_module->getResourceManager()->getTargetMinsToWork() + workers_morphing) < ai_module->getResourceManager()->getNumMinsToWork())
		{
			if(ai_module->makeWorker())
			{
				workers_morphing++;
				if(need_spawning_pool) workers_since_pool++;
			}
		}
		if(Broodwar->self()->supplyTotal() <= Broodwar->self()->supplyUsed()) {
			if(Broodwar->self()->minerals() >= 100 && !overlord_building) {
				if(ai_module->makeOverlord()) overlord_building = true;
			}
		} else if(overlord_building) {
			overlord_building = false;
		}

		if(!stop_workers)
		{
			if(slow_workers && workers_since_pool == 3)
			{
				stop_workers = true;
			}
			else if(ai_module->getResourceManager()->getNumMinsToWork() > workers_morphing && Broodwar->self()->minerals() >= 50 && Broodwar->self()->supplyTotal() > Broodwar->self()->supplyUsed())
			{
				if(ai_module->makeWorker())
				{
					workers_morphing++;
					if(need_spawning_pool) workers_since_pool++;
				}
			}
		}

		if(spawning_pool_finished)
		{
			ai_module->makeUnit(UnitTypes::Zerg_Zergling, 3);
		}

		if(
				(starting_pool && need_spawning_pool) || 
				(need_spawning_pool && Broodwar->self()->minerals() >= 180)
		) {
			Broodwar->printf("Building spawning pool at %i supply",Broodwar->self()->supplyUsed());
			need_spawning_pool = !ai_module->buildSpawningPool();
			starting_pool = true;
			stop_workers = true;
			slow_workers = true;
			if(!need_spawning_pool)
			{
				starting_pool = false;
				starting_geyser = true;
				workers_since_pool = 0;
			}
		}

		if(starting_geyser && Broodwar->self()->minerals() >= 50)
		{
			if(ai_module->buildGeyser()) 
			{
				starting_geyser = false;
				stop_workers = false;
			}
		}

		if(hatch_started || Broodwar->self()->minerals() > 285)
		{
			hatch_started = true;
			if(ai_module->buildHatchery())
			{
				hatch_built = true;
				hatch_started = false;
			}
		}
	}
}
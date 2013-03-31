#include "UnitCreator.h"
using namespace BWAPI;

UnitCreator::UnitCreator(void)
{
}

UnitCreator::~UnitCreator(void)
{
}

UnitCreator::UnitCreator(BaseAIModule* module)
{
	ai_module = module;
}

bool UnitCreator::hasLarva()
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

bool UnitCreator::hasFullLarva()
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

int UnitCreator::makeUnit(UnitType type, int num) 
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

int UnitCreator::makeOverlord(int num)
{
	return makeUnit(UnitTypes::Zerg_Overlord, num);
}

int UnitCreator::makeWorker(int num)
{
	return makeUnit(UnitTypes::Zerg_Drone, num);
}

void UnitCreator::addProducer(Unit* unit)
{
	hatcheries.push_back(unit);
}

bool UnitCreator::buildHatchery()
{
	bool retval = false;
	static Unit* worker = 0;
	static int tile_offset = 0;
	static TilePosition* position = 0;

	if(worker && !worker->getType().isWorker())
	{
		retval = true;
		worker = 0;
		delete position;
		position = 0;
	}
	else
	{
		if(!worker)
		{
			worker = ai_module->getResourceManager()->getWorker();
			if(worker)
			{
				position = ai_module->getMapLocations()->popHatcheryPosition();
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
	return retval;
}

bool UnitCreator::buildSpawningPool()
{
	static Position* worker_position;
	static Unit* worker;
	bool retval = false;
	if(!worker_position)
	{
		worker_position = new Position(TilePosition(ai_module->getMapLocations()->getPoolPosition().x() + 2, ai_module->getMapLocations()->getPoolPosition().y() + 1));
	}
	if(!worker)
	{
		worker = ai_module->getResourceManager()->getWorker();
	}
	if(worker)
	{
		if(!worker->getType().isWorker()) {
			retval = true;
			delete worker_position;
			worker_position = 0;
		} else {
			if(worker && Broodwar->self()->minerals() < 200 && worker->getOrder().getID() != Orders::Move)
			{
				worker->rightClick(*worker_position);
			} else if(worker && Broodwar->self()->minerals() >= 200) {
				worker->build(ai_module->getMapLocations()->getPoolPosition(), UnitTypes::Zerg_Spawning_Pool);
			}
		}
	}
	return retval;
}

bool UnitCreator::buildGeyser()
{
	static TilePosition* geyser_position = 0;
	static Unit* worker = 0;
	static bool built = false;
	bool retval = false;
	if(!geyser_position)
	{
		Unit* geyser = ai_module->getResourceManager()->getGeyser();
		if(geyser)
		{
			geyser_position = &geyser->getTilePosition();
		}
	}
	if(!worker && geyser_position)
	{
		worker = ai_module->getResourceManager()->getWorker();
		worker->rightClick(Position(*geyser_position));
	}
	if(worker)
	{
		if(!worker->getType().isWorker()) {
			Broodwar->printf("Geyser finished");
			retval = true;
			worker = 0;
			geyser_position = 0;
		} else if (Broodwar->self()->minerals() >= 50 && !built) {
			built = worker->build(*geyser_position, UnitTypes::Zerg_Extractor);
			Broodwar->printf("%i Building geyser - %s", Broodwar->getFrameCount(), built ? "Success!" : "Failure!");
		}
	}
	return retval;
}

#include "ResourceManager.h"
#include "source\BaseAIModule.h"
using namespace BWAPI;

std::vector<BWAPI::Unit*> ResourceManager::bases;

bool sortByDistanceFromBase(BWAPI::Unit *unit1, BWAPI::Unit *unit2)
{
	int unit1_distance = 0, unit2_distance = 0;
	for(unsigned int i = 0; i < ResourceManager::bases.size(); i++) 
	{
		int cur1 = unit1->getDistance(ResourceManager::bases[i]), cur2 = unit2->getDistance(ResourceManager::bases[i]);
		if(unit1_distance == 0 || cur1 < unit1_distance)
			unit1_distance = cur1;
		if(unit2_distance == 0 || cur2 < unit2_distance)
			unit2_distance = cur2;
	}
	return unit1_distance > unit2_distance;
}

ResourceManager::ResourceManager(BaseAIModule* module)
{
	ai_module = module;
};

ResourceManager::ResourceManager() {};

void ResourceManager::addBase(BWAPI::Unit* unit)
{
	bases.push_back(unit);
	std::vector<BWAPI::Unit*> newmin;
	for(std::set<Unit*>::iterator m=Broodwar->getMinerals().begin();m!=Broodwar->getMinerals().end();m++)
	{
		if(unit->getDistance(*m) < FIND_MINERAL_DISTANCE)
		{
			newmin.push_back(*m);
		}
	}
	for(std::set<Unit*>::iterator g=Broodwar->getGeysers().begin();g!=Broodwar->getGeysers().end();g++)
	{
		if(unit->getDistance(*g) < FIND_MINERAL_DISTANCE) {
			geysers_to_work.push_back(*g);
		}
	}
	std::sort(newmin.begin(), newmin.end(), sortByDistanceFromBase);
	default_mineral = newmin.front();
	for(unsigned int i=0; i < newmin.size(); i++)
	{
		minerals_to_work.push_back(newmin[i]);
	}
	for(unsigned int i=0; i < newmin.size(); i++)
	{
		minerals_to_work.push_back(newmin[i]);
	}
	Broodwar->printf("There are %i minerals to work",minerals_to_work.size());
	min_min = minerals_to_work.size();
};

void ResourceManager::init()
{
	Broodwar->printf("Initing resource manager");
};

void ResourceManager::addWorker(BWAPI::Unit *unit)
{
	workers.push_back(unit);
	unit->rightClick(minerals_to_work.back());
	minerals_to_work.pop_back();
	if(minerals_to_work.size() < min_min) min_min = minerals_to_work.size();
};

BWAPI::Unit* ResourceManager::getWorker() {
	BWAPI::Unit* retval = 0;
	std::map<unsigned int,BWAPI::Unit*> possible_workers;

	for(unsigned int i = 0; i < workers.size(); i++) {
		if(workers[i]->getOrder().getID() == Orders::MoveToMinerals) {
			possible_workers[i] = workers[i];
		}
	}

	if(possible_workers.size()) 
	{
		int closest_distance = 0;
		unsigned int current_pos = 0;
		for(unsigned int b = 0; b < bases.size(); b++)
		{
			for(std::map<unsigned int,BWAPI::Unit*>::iterator i = possible_workers.begin(); i != possible_workers.end(); i++)
			{
				int dist = i->second->getDistance(bases[b]);
				if(closest_distance == 0 || dist < closest_distance)
				{
					retval = i->second;
					current_pos = i->first;
				}
			}
		}
		workers.erase(workers.begin() + current_pos);
		if(retval->getOrderTarget()->getType().isMineralField()) {
			minerals_to_work.push_back(retval->getOrderTarget());
		}
		else
		{
			Broodwar->printf("Invalid target, defaulting to minerals");
			minerals_to_work.push_back(default_mineral);
		}
	}
	return retval;
}

int ResourceManager::getNumMinsToWork()
{
	return minerals_to_work.size();
}

int ResourceManager::getTargetMinsToWork()
{
	return min_min;
}

int ResourceManager::getNumWorkers()
{
	return workers.size();
}

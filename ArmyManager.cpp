#include "ArmyManager.h"
using namespace BWAPI;

ArmyManager::ArmyManager(void)
{
}

ArmyManager::~ArmyManager(void)
{
}

ArmyManager::ArmyManager(BaseAIModule* module)
{
	ai_module = module;
	current_attack = ai_module->getMapLocations()->getDefaultAttackPosition();
}

void ArmyManager::addArmyUnit(BWAPI::Unit* unit)
{
	army.push_back(unit);
	unit->attack(current_attack);
}

void ArmyManager::setNextAttack()
{
	if(next_attack)
	{
		bool validated = false;
	}
}

void ArmyManager::heartbeat()
{
	if(army.size())
	{
		for(unsigned int i = 0; i < army.size(); i++)
		{
			if(army[i]->getOrder().getID() == Orders::None)
			{
				if(army[i]->exists()) Broodwar->printf("This unit exists!");
				army.erase(army.begin() + i);
				i--;
			}
			else if(army[i]->getOrder().getID() == Orders::PlayerGuard)
			{
								
			}
		}
	}
}

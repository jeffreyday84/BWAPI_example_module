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
		std::vector<Unit*> units_needing_orders;
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
				units_needing_orders.push_back(army[i]);
			}
		}
		if(units_needing_orders.size() && ai_module->getEnemyPlayer()->getUnits().size())
		{
			Unit* unit_to_attack = *(ai_module->getEnemyPlayer()->getUnits().begin());
			for(int i = 0; i < units_needing_orders.size(); i++)
			{
				units_needing_orders[i]->attack(unit_to_attack->getPosition());
			}
		}
	}
}

#include "ArmyManager.h"

ArmyManager::ArmyManager(void)
{
}

ArmyManager::~ArmyManager(void)
{
}

ArmyManager::ArmyManager(ExampleAIModule* module)
{
	ai_module = module;
	current_attack = ai_module->getMapLocations()->getDefaultAttackPosition();
}

void ArmyManager::addArmyUnit(BWAPI::Unit* unit)
{
	army.push_back(unit);
	unit->attack(current_attack);
}

void ArmyManager::heartbeat()
{
}
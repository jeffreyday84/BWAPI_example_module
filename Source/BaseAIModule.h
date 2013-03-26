#pragma once
#include <BWAPI.h>

#include <BWTA.h>
#include <windows.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <set>
#include "..\ResourceManager.h"
#include "..\ScoutingManager.h"
#include "..\MapLocations.h"
#include "..\ArmyManager.h"

extern bool analyzed;
extern bool analysis_just_finished;
extern BWTA::Region* home;
extern BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();

class ResourceManager;
class ScoutingManager;
class MapLocations;
class ArmyManager;
class BaseAIModule : public BWAPI::AIModule
{
protected:
	/**
	 * Managers
	 */
	ResourceManager* resource_manager;
	ScoutingManager* scouting_manager;
	MapLocations* map_locations;
	ArmyManager* army_manager;

	/**
	 * Game State
	 */
	BWAPI::Unit* main_base;
	BWAPI::Unit* spawning_pool;
	BWAPI::Player* enemy_player;

	/**
	 * Internal Methods
	 */
	void checkReadinessQueue();
	void buildHatchery();
	bool buildSpawningPool();

	/**
	 * Internal State
	 */
	int workers_morphing;
	bool need_spawning_pool;
	bool starting_pool;
	bool spawning_pool_finished;
	bool morphing_hatchery;
	bool slow_workers;
	bool stop_workers;
	std::set<int> attacked_overlords;
	std::vector<BWAPI::Unit*> unit_readiness_queue;
	std::queue<BWAPI::UnitType> build_queue;
	std::vector<BWAPI::Unit*> hatcheries;
public:
	/**
	 * Game state functions
	 */
	BWAPI::Unit* getMainBase();

	/**
	 * Utility Functions
	 */
	bool hasLarva();
	bool hasFullLarva();
	int makeUnit(BWAPI::UnitType type, int num = 1);
	int makeWorker(int num = 1);
	int makeOverlord(int num = 1);

	/**
	 * Manager Getters
	 */
	BaseAIModule* getUnitCreator();
	ResourceManager* getResourceManager();
	ScoutingManager* getScoutingManager();
	MapLocations* getMapLocations();

	/**
	 * Used events
	 */
	virtual void onStart();
	virtual void onFrame();
	virtual void onUnitReady(BWAPI::Unit* unit);
	virtual void onOverlordAttacked(BWAPI::Unit* unit);

	/**
	 * Forwarded Events
	 */
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);

	/**
	 * Unused Events/Functions/Members
	 */
	virtual void onUnitComplete(BWAPI::Unit *unit);
	virtual void onEnd(bool isWinner);
	virtual void onSendText(std::string text);
	static void sendMessage(const char* text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onSaveGame(std::string gameName);
	void drawStats(); 
	void drawBullets();
	void drawVisibilityData();
	void drawTerrainData();
	void showPlayers();
	void showForces();
};

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
	ResourceManager* resource_manager;
	ScoutingManager* scouting_manager;
	MapLocations* map_locations;
	ArmyManager* army_manager;

	int workers_morphing;
	bool need_spawning_pool;
	bool starting_pool;
	bool spawning_pool_finished;
	bool morphing_hatchery;
	BWAPI::Unit* main_base;
	BWAPI::Unit* spawning_pool;
	bool slow_workers;
	bool stop_workers;
	std::vector<BWAPI::Unit*> unit_readiness_queue;
	std::queue<BWAPI::UnitType> build_queue;
	std::set<int> attacked_overlords;
	void checkReadinessQueue();
	void makeHatchery();
public:
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onUnitReady(BWAPI::Unit* unit);
	virtual void onOverlordAttacked(BWAPI::Unit* unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit *unit);
	static void sendMessage(const char* text);
	void drawStats(); 
	void drawBullets();
	void drawVisibilityData();
	void drawTerrainData();
	void showPlayers();
	void showForces();
	bool show_bullets;
	bool show_visibility_data;
	int make_unit(BWAPI::UnitType type, int num = 1);
	int make_worker(int num = 1);
	int make_overlord(int num = 1);
	void init_min_for_hatch(BWAPI::Unit *unit);
	void make_worker_gather(BWAPI::Unit *unit);
	void make_worker_gather_min(BWAPI::Unit *worker, BWAPI::Unit *min);
	bool createSpawningPool();
	ExampleAIModule* getUnitCreator();
	ResourceManager* getResourceManager();
	ScoutingManager* getScoutingManager();
	MapLocations* getMapLocations();
	BWAPI::Unit* getMainBase();
	bool hasFullLarva();
	bool hasLarva();
};

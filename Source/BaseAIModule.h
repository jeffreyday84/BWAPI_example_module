#pragma once
#include <BWAPI.h>

#include <BWTA.h>
#include <windows.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <set>

extern bool analyzed;
extern bool analysis_just_finished;
extern BWTA::Region* home;
extern BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();

class ResourceManager;
class ScoutingManager;
class MapLocations;
class ArmyManager;
class StrategyManager;
class UnitCreator;
class FrameEventHandler;
class HeartbeatEventHandler;
class UnitReadyEventHandler;
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
	StrategyManager* strategy_manager;
	UnitCreator* unit_creator;

	/**
	 * Game State
	 */
	BWAPI::Unit* spawning_pool;
	BWAPI::Player* enemy_player;

	/**
	 * Internal Methods
	 */
	void checkReadinessQueue();

	/**
	 * Internal State
	 */
	std::vector<BWAPI::Unit*> unit_readiness_queue;

	/**
	 * Event management
	 */
	static const int FRAMES_PER_HEARTBEAT = 12;
	std::vector<FrameEventHandler*> frame_event_handlers;
	std::vector<HeartbeatEventHandler*> heartbeat_event_handlers;
	std::vector<UnitReadyEventHandler*> unit_ready_event_handlers;
public:
	/**
	 * Game state functions
	 */
	BWAPI::Unit* getSpawningPool();
	void setSpawningPool(BWAPI::Unit*);

	/**
	 * Manager Getters
	 */
	ResourceManager* getResourceManager();
	ScoutingManager* getScoutingManager();
	MapLocations* getMapLocations();
	ArmyManager* getArmyManager();
	UnitCreator* getUnitCreator();
	BWAPI::Player* getEnemyPlayer();

	/**
	 * Event handler functions
	 */
	void addFrameHandler(FrameEventHandler*);
	void removeFrameHandler(FrameEventHandler*);
	void addHeartbeatHandler(HeartbeatEventHandler*);
	void removeHeartbeatHandler(HeartbeatEventHandler*);
	void addUnitReadyHandler(UnitReadyEventHandler*);
	void removeUnitReadyHandler(UnitReadyEventHandler*);

	/**
	 * Used events
	 */
	virtual void onStart();
	virtual void onFrame();
	virtual void onUnitReady(BWAPI::Unit* unit);
	virtual void onUnitAttacked(BWAPI::Unit* unit);

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

class FrameEventHandler
{
public:
	virtual void onFrame() { };
};

class HeartbeatEventHandler
{
public:
	virtual void onHeartbeat() { };
};

class UnitReadyEventHandler
{
public:
	virtual void onUnitReady(BWAPI::Unit*) { };
};
#include "ResourceManager.h"
#include "ScoutingManager.h"
#include "MapLocations.h"
#include "ArmyManager.h"
#include "StrategyManager.h"
#include "UnitCreator.h"

// SwarmObserver constants
class SwarmObserverConstants
{
	// Base directory
	static const string DIR_SWARM_OBSERVER = "$profile:SwarmObserver\\";
	
	// Configuration files
	static const string FILE_CONFIG = DIR_SWARM_OBSERVER + "config.json";
	static const string FILE_HISTORY = DIR_SWARM_OBSERVER + "history.json";
	
	// File extensions
	static const string EXT_JSON = ".json";
	
	// Default settings
	static const int DEFAULT_GRACE_PERIOD_SECONDS = 60;
	static const int DEFAULT_HISTORY_DAYS = 3;
	static const string DEFAULT_WEBHOOK_URL = "https://discord.com/api/webhooks/YOUR_WEBHOOK_ID/YOUR_WEBHOOK_TOKEN";
	
	// Combat logout settings
	static const int DEFAULT_COMBAT_DURATION_SECONDS = 300;
	static const float DEFAULT_SHOT_PROXIMITY_RADIUS = 50.0;
	static const float DEFAULT_EXPLOSIVE_PROXIMITY_RADIUS = 30.0;
	static const int COMBAT_CLEANUP_INTERVAL_MS = 30000;
	static const int MAX_COMBAT_ACTIONS_STORED = 20;
	
	// Menu IDs
	static const int MENU_SWARM_OBSERVER_WARNING = 28501;
	static const int MENU_SWARM_OBSERVER_NOTICE = 28502;
	static const int MENU_SWARM_OBSERVER_LOGOUT = 28503;
}

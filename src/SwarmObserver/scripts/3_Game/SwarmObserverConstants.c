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
	
	// Menu IDs
	static const int MENU_SWARM_OBSERVER_WARNING = 28501;
	static const int MENU_SWARM_OBSERVER_NOTICE = 28502;
}

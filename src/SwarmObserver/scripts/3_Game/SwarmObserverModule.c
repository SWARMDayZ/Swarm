// SwarmObserver initialization module
class SwarmObserverModule
{
	private static ref SwarmObserverModule s_Instance;
	
	void SwarmObserverModule()
	{
		Print("[SwarmObserver] Module constructor called");
		
		// Create directory structure
		CreateDirectories();
	}
	
	// Get singleton instance
	static SwarmObserverModule GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new SwarmObserverModule();
		}
		return s_Instance;
	}
	
	// Create required directories
	private void CreateDirectories()
	{
		Print("[SwarmObserver] Creating directory structure...");
		MakeDirectory(SwarmObserverConstants.DIR_SWARM_OBSERVER);
		Print("[SwarmObserver] Directory created: " + SwarmObserverConstants.DIR_SWARM_OBSERVER);
	}
	
	// Initialize the module (called from MissionServer.OnInit)
	static void Initialize()
	{
		Print("[SwarmObserver] Initializing...");
		
		// Ensure instance exists (creates directories)
		GetInstance();
		
		// Load configuration (creates config.json if missing)
		SwarmObserverSettings.LoadConfig();
		
		// Load history (creates history database structure)
		SwarmObserverHistory.LoadHistory();
		
		// Note: Combat system initializes lazily when first accessed
		
		Print("[SwarmObserver] Initialization complete");
	}
}

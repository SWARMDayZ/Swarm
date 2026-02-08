// Restricted area definition
class SwarmObserverRestrictedArea
{
	string Name;
	vector Position;
	float Radius;
	
	void SwarmObserverRestrictedArea(string name, vector pos, float radius)
	{
		Name = name;
		Position = pos;
		Radius = radius;
	}
	
	// Check if a position is within this restricted area
	bool IsPositionInside(vector pos)
	{
		float distance = vector.Distance(pos, Position);
		return distance <= Radius;
	}
}

// Main configuration class
class SwarmObserverSettings
{
	private static ref SwarmObserverSettings s_Instance;
	
	string WebhookURL;
	int GracePeriodSeconds;
	int HistoryDays;
	autoptr array<ref SwarmObserverRestrictedArea> RestrictedAreas;
	
	// Combat logout settings
	bool CombatLogoutEnabled;
	int CombatDurationSeconds;
	float ShotProximityRadius;
	float ExplosiveProximityRadius;
	string CombatWebhookURL;
	bool CombatDebugMode;
	
	void SwarmObserverSettings()
	{
		RestrictedAreas = new array<ref SwarmObserverRestrictedArea>;
		GracePeriodSeconds = SwarmObserverConstants.DEFAULT_GRACE_PERIOD_SECONDS;
		HistoryDays = SwarmObserverConstants.DEFAULT_HISTORY_DAYS;
		WebhookURL = "";
		
		// Combat logout defaults
		CombatLogoutEnabled = true;
		CombatDurationSeconds = SwarmObserverConstants.DEFAULT_COMBAT_DURATION_SECONDS;
		ShotProximityRadius = SwarmObserverConstants.DEFAULT_SHOT_PROXIMITY_RADIUS;
		ExplosiveProximityRadius = SwarmObserverConstants.DEFAULT_EXPLOSIVE_PROXIMITY_RADIUS;
		CombatWebhookURL = "";
		CombatDebugMode = false;
	}
	
	// Get singleton instance
	static SwarmObserverSettings GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new SwarmObserverSettings();
		}
		return s_Instance;
	}
	
	// Load configuration from JSON file
	static bool LoadConfig()
	{
		SwarmObserverSettings settings = GetInstance();
		
		if (FileExist(SwarmObserverConstants.FILE_CONFIG))
		{
			JsonFileLoader<SwarmObserverSettings>.JsonLoadFile(SwarmObserverConstants.FILE_CONFIG, settings);
			Print("[SwarmObserver] Config loaded successfully");
			Print("[SwarmObserver] Webhook URL: " + settings.WebhookURL);
			Print("[SwarmObserver] Grace Period: " + settings.GracePeriodSeconds + " seconds");
			Print("[SwarmObserver] History Days: " + settings.HistoryDays);
			Print("[SwarmObserver] Restricted Areas: " + settings.RestrictedAreas.Count());
			
			foreach (SwarmObserverRestrictedArea area : settings.RestrictedAreas)
			{
				Print("[SwarmObserver]   - " + area.Name + " at " + area.Position.ToString() + " (radius: " + area.Radius + "m)");
			}
			
			Print("[SwarmObserver] Combat Logout Enabled: " + settings.CombatLogoutEnabled);
			Print("[SwarmObserver] Combat Duration: " + settings.CombatDurationSeconds + " seconds");
			Print("[SwarmObserver] Shot Proximity Radius: " + settings.ShotProximityRadius + "m");
			Print("[SwarmObserver] Explosive Proximity Radius: " + settings.ExplosiveProximityRadius + "m");
			Print("[SwarmObserver] Combat Debug Mode: " + settings.CombatDebugMode);
		}
		else
		{
			Print("[SwarmObserver] Config file not found, creating default config...");
			settings.Defaults();
			settings.Save();
		}
		
		return true;
	}
	
	// Save configuration to file
	void Save()
	{
		JsonFileLoader<SwarmObserverSettings>.JsonSaveFile(SwarmObserverConstants.FILE_CONFIG, this);
		Print("[SwarmObserver] Config saved");
	}
	
	// Set default values
	void Defaults()
	{
		WebhookURL = SwarmObserverConstants.DEFAULT_WEBHOOK_URL;
		GracePeriodSeconds = SwarmObserverConstants.DEFAULT_GRACE_PERIOD_SECONDS;
		HistoryDays = SwarmObserverConstants.DEFAULT_HISTORY_DAYS;
		
		RestrictedAreas.Clear();
		RestrictedAreas.Insert(new SwarmObserverRestrictedArea("Athena-3", Vector(3592, 120, 6709), 80));
		
		// Combat logout defaults
		CombatLogoutEnabled = true;
		CombatDurationSeconds = SwarmObserverConstants.DEFAULT_COMBAT_DURATION_SECONDS;
		ShotProximityRadius = SwarmObserverConstants.DEFAULT_SHOT_PROXIMITY_RADIUS;
		ExplosiveProximityRadius = SwarmObserverConstants.DEFAULT_EXPLOSIVE_PROXIMITY_RADIUS;
		CombatWebhookURL = "";
		CombatDebugMode = false;
	}
	
	// Check if a position is in any restricted area
	static SwarmObserverRestrictedArea GetRestrictedAreaAtPosition(vector pos)
	{
		SwarmObserverSettings settings = GetInstance();
		
		foreach (SwarmObserverRestrictedArea area : settings.RestrictedAreas)
		{
			if (area.IsPositionInside(pos))
			{
				return area;
			}
		}
		
		return null;
	}
}

// Grace period data for a player
class SwarmObserverGracePeriodData
{
	string SteamID;
	string PlayerName;
	string AreaName;
	int DisconnectTimestamp;
	
	void SwarmObserverGracePeriodData(string steamID, string playerName, string areaName)
	{
		SteamID = steamID;
		PlayerName = playerName;
		AreaName = areaName;
		DisconnectTimestamp = GetGame().GetTime();
	}
}

// Grace period manager singleton
class SwarmObserverGracePeriodManager
{
	private static ref SwarmObserverGracePeriodManager s_Instance;
	private ref map<string, ref SwarmObserverGracePeriodData> m_ActiveGracePeriods;
	private bool m_TimerRunning;
	
	void SwarmObserverGracePeriodManager()
	{
		m_ActiveGracePeriods = new map<string, ref SwarmObserverGracePeriodData>;
		m_TimerRunning = false;
	}
	
	// Get singleton instance
	static SwarmObserverGracePeriodManager GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new SwarmObserverGracePeriodManager();
		}
		return s_Instance;
	}
	
	// Start grace period for a player
	static void StartGracePeriod(string steamID, string playerName, string areaName)
	{
		SwarmObserverGracePeriodManager manager = GetInstance();
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		
		// Create grace period data
		SwarmObserverGracePeriodData data = new SwarmObserverGracePeriodData(steamID, playerName, areaName);
		manager.m_ActiveGracePeriods.Set(steamID, data);
		
		Print("[SwarmObserver] Grace period started for " + playerName + " (" + steamID + ") in area: " + areaName);
		
		// Schedule callback after grace period expires
		// Note: We check all grace periods in a single timer rather than per-player timers
		if (!manager.m_TimerRunning)
		{
			manager.m_TimerRunning = true;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(manager.CheckExpiredGracePeriods, 5000, true);
		}
	}
	
	// Check all grace periods and handle expired ones
	void CheckExpiredGracePeriods()
	{
		int currentTime = GetGame().GetTime();
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		int graceTimeMS = settings.GracePeriodSeconds * 1000;
		
		array<string> expiredPlayers = new array<string>;
		
		// Find expired grace periods
		foreach (string steamID, SwarmObserverGracePeriodData data : m_ActiveGracePeriods)
		{
			int elapsedTime = currentTime - data.DisconnectTimestamp;
			if (elapsedTime >= graceTimeMS)
			{
				expiredPlayers.Insert(steamID);
			}
		}
		
		// Process expired grace periods
		foreach (string expiredSteamID : expiredPlayers)
		{
			OnGracePeriodExpired(expiredSteamID);
		}
		
		// If there are no more active grace periods, stop the repeating timer
		if (m_ActiveGracePeriods.Count() == 0 && m_TimerRunning)
		{
			SwarmObserverGracePeriodManager manager = SwarmObserverGracePeriodManager.GetInstance();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(manager.CheckExpiredGracePeriods);
			m_TimerRunning = false;
		}
	}
	
	// Called when grace period timer expires
	void OnGracePeriodExpired(string steamID)
	{
		// Check if grace period is still active (player didn't reconnect)
		if (!m_ActiveGracePeriods.Contains(steamID))
		{
			return; // Player reconnected in time
		}
		
		SwarmObserverGracePeriodData data = m_ActiveGracePeriods.Get(steamID);
		
		Print("[SwarmObserver] Grace period expired for " + data.PlayerName + " (" + steamID + ")");
		
		// Increment violation count in history
		SwarmObserverHistory.IncrementViolation(steamID, data.PlayerName, data.AreaName);
		
		// Send webhook notification (timestamp not needed as it will use current time)
		SwarmObserverWebhook.SendDisconnectNotification(steamID, data.PlayerName, data.AreaName, 0);
		
		// Remove from active grace periods (history already updated above)
		m_ActiveGracePeriods.Remove(steamID);
	}
	
	// Cancel grace period (player reconnected in time)
	static bool CancelGracePeriod(string steamID)
	{
		SwarmObserverGracePeriodManager manager = GetInstance();
		
		if (manager.m_ActiveGracePeriods.Contains(steamID))
		{
			SwarmObserverGracePeriodData data = manager.m_ActiveGracePeriods.Get(steamID);
			Print("[SwarmObserver] Grace period cancelled for " + data.PlayerName + " - reconnected in time");
			
			manager.m_ActiveGracePeriods.Remove(steamID);
			return true;
		}
		
		return false;
	}
	
	// Check if player has active grace period
	static bool HasActiveGracePeriod(string steamID)
	{
		SwarmObserverGracePeriodManager manager = GetInstance();
		return manager.m_ActiveGracePeriods.Contains(steamID);
	}
	
	// Check if player should receive reconnect notification
	// Returns violation count if they reconnected after grace period expired, 0 otherwise
	static int GetReconnectViolationCount(string steamID)
	{
		SwarmObserverGracePeriodManager manager = GetInstance();
		
		// If they still have an active grace period, they reconnected in time
		if (manager.m_ActiveGracePeriods.Contains(steamID))
		{
			// Cancel grace period and no notification
			CancelGracePeriod(steamID);
			return 0;
		}
		
		// Check history for unacknowledged violations only
		SwarmObserverPlayerRecord record = SwarmObserverHistory.GetPlayerRecord(steamID);
		if (record)
		{
			int unacknowledgedCount = record.GetUnacknowledgedCount();
			// Only show notification if they have new violations since last acknowledgement
			return unacknowledgedCount;
		}
		
		return 0;
	}
}

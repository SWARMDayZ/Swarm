// Stores player data during logout process (keyed by Steam ID)
class SwarmObserverLogoutData
{
	string PlayerName;
	vector Position;
	string RestrictedAreaName;
	bool WasInCombat;
	ref CombatState CombatStateData;
	string SteamID;
	
	void SwarmObserverLogoutData(string playerName, string steamId, vector pos, string areaName, bool inCombat, CombatState combatState)
	{
		PlayerName = playerName;
        SteamID = steamId;
		Position = pos;
		RestrictedAreaName = areaName;
		WasInCombat = inCombat;
		CombatStateData = combatState;
	}
}

// Manager to track logout data by Steam ID
class SwarmObserverLogoutManager
{
	private static ref SwarmObserverLogoutManager s_Instance;
	private ref map<string, ref SwarmObserverLogoutData> m_LogoutData;
	
	void SwarmObserverLogoutManager()
	{
		m_LogoutData = new map<string, ref SwarmObserverLogoutData>;
	}
	
	static SwarmObserverLogoutManager GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new SwarmObserverLogoutManager();
		}
		return s_Instance;
	}
	
	// Store logout data when disconnect is initiated (keyed by Steam ID)
	static void StoreLogoutData(string uid, string steamId, string playerName, vector pos, string areaName, bool inCombat, CombatState combatState)
	{
		SwarmObserverLogoutManager manager = GetInstance();
		SwarmObserverLogoutData data = new SwarmObserverLogoutData(playerName, steamId, pos, areaName, inCombat, combatState);
		manager.m_LogoutData.Set(uid, data);
	}
	
	// Get stored logout data by Steam ID
	static SwarmObserverLogoutData GetLogoutData(string uid)
	{
		SwarmObserverLogoutManager manager = GetInstance();
		if (manager.m_LogoutData.Contains(uid))
		{
			return manager.m_LogoutData.Get(uid);
		}
		return null;
	}
	
	// Remove logout data after processing
	static void RemoveLogoutData(string uid)
	{
		SwarmObserverLogoutManager manager = GetInstance();
		manager.m_LogoutData.Remove(uid);
	}
}

// Combat state manager singleton
class CombatStateManager
{
	private static ref CombatStateManager s_Instance;
	private ref map<PlayerBase, ref CombatState> m_CombatStates;
	private bool m_CleanupTimerRunning;
	
	void CombatStateManager()
	{
		m_CombatStates = new map<PlayerBase, ref CombatState>();
		m_CleanupTimerRunning = false;
		
		Print("[SwarmObserver] CombatStateManager initialized");
	}
	
	// Get singleton instance
	static CombatStateManager GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new CombatStateManager();
		}
		return s_Instance;
	}
	
	// Register a combat action for a player
	void RegisterCombatAction(PlayerBase player, PlayerBase otherPlayer, string actionType, string damageType = "")
	{
		if (!player || !GetGame().IsServer())
			return;
		
		// Get config settings
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		if (!settings.CombatLogoutEnabled)
			return;
		
		// Get or create combat state
		CombatState state;
		if (m_CombatStates.Contains(player))
		{
			state = m_CombatStates.Get(player);
		}
		else
		{
			state = new CombatState();
			m_CombatStates.Set(player, state);
		}
		
		// Update state
		state.Update(player, otherPlayer, actionType, damageType);
		
		// Debug logging
		if (settings.CombatDebugMode && player.GetIdentity())
		{
			string otherName = "Unknown";
			if (otherPlayer && otherPlayer.GetIdentity())
			{
				otherName = otherPlayer.GetIdentity().GetName();
			}
			Print(string.Format("[SwarmObserver] Combat action: %1 %2 %3", 
				player.GetIdentity().GetName(), actionType, otherName));
		}
		
		// Ensure cleanup timer is running
		StartCleanupTimer();
	}
	
	// Check if a player is in combat
	bool IsInCombat(PlayerBase player)
	{
		if (!player || !m_CombatStates.Contains(player))
			return false;
		
		CombatState state = m_CombatStates.Get(player);
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		int currentTime = GetGame().GetTime();
		
		return !state.IsExpired(currentTime, settings.CombatDurationSeconds);
	}
	
	// Get combat state for a player
	CombatState GetCombatState(PlayerBase player)
	{
		if (!player || !m_CombatStates.Contains(player))
			return null;
		
		return m_CombatStates.Get(player);
	}
	
	// Remove combat state (called on disconnect)
	void RemoveCombatState(PlayerBase player)
	{
		if (!player)
			return;
		
		if (m_CombatStates.Contains(player))
		{
			m_CombatStates.Remove(player);
			
			if (player.GetIdentity())
			{
				Print("[SwarmObserver] Combat state removed for: " + player.GetIdentity().GetName());
			}
		}
	}
	
	// Start the cleanup timer if not already running
	private void StartCleanupTimer()
	{
		if (!m_CleanupTimerRunning)
		{
			m_CleanupTimerRunning = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CleanExpiredStates, SwarmObserverConstants.COMBAT_CLEANUP_INTERVAL_MS, true);
			Print("[SwarmObserver] Combat cleanup timer started");
		}
	}
	
	// Clean expired combat states (called by timer)
	void CleanExpiredStates()
	{
		if (!GetGame().IsServer())
			return;
		
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		int currentTime = GetGame().GetTime();
		array<PlayerBase> expiredPlayers = new array<PlayerBase>();
		
		// Find expired states
		foreach (PlayerBase player, CombatState state : m_CombatStates)
		{
			if (state.IsExpired(currentTime, settings.CombatDurationSeconds))
			{
				expiredPlayers.Insert(player);
			}
		}
		
		// Remove expired states
		foreach (PlayerBase expiredPlayer : expiredPlayers)
		{
			if (expiredPlayer && expiredPlayer.GetIdentity())
			{
				if (settings.CombatDebugMode)
				{
					Print("[SwarmObserver] Combat state expired for: " + expiredPlayer.GetIdentity().GetName());
				}
			}
			m_CombatStates.Remove(expiredPlayer);
		}
		
		// Stop timer if no more active combat states
		if (m_CombatStates.Count() == 0 && m_CleanupTimerRunning)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(CleanExpiredStates);
			m_CleanupTimerRunning = false;
			Print("[SwarmObserver] Combat cleanup timer stopped (no active combat)");
		}
	}
	
	// Get count of active combat states (for debugging)
	int GetActiveCombatCount()
	{
		return m_CombatStates.Count();
	}
}

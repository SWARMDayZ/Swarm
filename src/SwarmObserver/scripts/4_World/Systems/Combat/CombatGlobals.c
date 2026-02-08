// Global accessor functions for combat system
// Following the existing singleton pattern from SwarmObserverModule

// Get combat state manager instance
CombatStateManager GetCombatStateManager()
{
	static ref CombatStateManager s_Instance;
	if (!s_Instance)
	{
		s_Instance = new CombatStateManager();
	}
	return s_Instance;
}

// Get combat settings from main config
SwarmObserverSettings GetCombatSettings()
{
	return SwarmObserverSettings.GetInstance();
}

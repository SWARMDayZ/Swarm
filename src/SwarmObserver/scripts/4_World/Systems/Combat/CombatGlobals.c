// Global accessor functions for combat system
// Following the existing singleton pattern from SwarmObserverModule

// Get combat state manager instance
CombatStateManager GetCombatStateManager()
{
	return CombatStateManager.GetInstance();
}

// Get combat settings from main config
SwarmObserverSettings GetCombatSettings()
{
	return SwarmObserverSettings.GetInstance();
}

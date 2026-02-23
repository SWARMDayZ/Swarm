// Combat state for a player
class CombatState
{
	int m_CombatStartTime;
	int m_LastCombatActionTime;
	ref array<string> m_InvolvedPlayers;
	ref array<ref CombatAction> m_CombatActions;
	vector m_LastPosition;
	string m_LastDamageType;
	float m_LastHealthLevel;
	
	void CombatState()
	{
		m_InvolvedPlayers = new array<string>();
		m_CombatActions = new array<ref CombatAction>();
		m_CombatStartTime = GetGame().GetTime();
		m_LastCombatActionTime = m_CombatStartTime;
		m_LastPosition = "0 0 0";
		m_LastDamageType = "";
		m_LastHealthLevel = 1.0;
	}
	
	// Update combat state with new action
	void Update(PlayerBase player, PlayerBase otherPlayer, string actionType, string damageType)
	{
		m_LastCombatActionTime = GetGame().GetTime();
		m_LastPosition = player.GetPosition();
		m_LastDamageType = damageType;
		m_LastHealthLevel = player.GetHealth("", "Health");
		
		// Add the other player to involved players if not already present
		if (otherPlayer && otherPlayer.GetIdentity())
		{
			string otherPlayerID = otherPlayer.GetIdentity().GetPlainId();
			if (m_InvolvedPlayers.Find(otherPlayerID) == -1)
			{
				m_InvolvedPlayers.Insert(otherPlayerID);
			}
		}
		
		// Add combat action (limit to MAX_COMBAT_ACTIONS_STORED)
		ref CombatAction action = new CombatAction(actionType, otherPlayer, damageType, player.GetPosition());
		m_CombatActions.Insert(action);
		
		if (m_CombatActions.Count() > SwarmObserverConstants.MAX_COMBAT_ACTIONS_STORED)
		{
			m_CombatActions.Remove(0);
		}
	}
	
	// Check if combat state has expired
	bool IsExpired(int currentTime, int combatDuration)
	{
		return (currentTime - m_LastCombatActionTime) >= (combatDuration * 1000);
	}
	
	// Get remaining time in seconds
	int GetRemainingTime(int currentTime, int combatDuration)
	{
		int elapsed = currentTime - m_LastCombatActionTime;
		int remaining = (combatDuration * 1000) - elapsed;
		return Math.Max(0, remaining / 1000);
	}
	
	// Get summary of combat actions for webhook
	string GetActionSummary()
	{
		if (m_CombatActions.Count() == 0)
			return "No actions recorded";
		
		string summary = "";
		int displayCount = Math.Min(5, m_CombatActions.Count());
		
		for (int i = m_CombatActions.Count() - displayCount; i < m_CombatActions.Count(); i++)
		{
			CombatAction action = m_CombatActions.Get(i);
			if (summary != "")
				summary += "\\n";
			summary += action.GetActionString();
		}
		
		if (m_CombatActions.Count() > 5)
		{
			summary = "... (" + (m_CombatActions.Count() - 5).ToString() + " more)\\n" + summary;
		}
		
		return summary;
	}
}

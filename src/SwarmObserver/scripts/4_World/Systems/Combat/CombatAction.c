// Individual combat action record
class CombatAction
{
	int m_Timestamp;
	string m_ActionType;
	string m_OtherPlayerName;
	string m_OtherPlayerID;
	string m_DamageType;
	float m_Distance;
	vector m_Position;
	
	void CombatAction(string actionType, PlayerBase otherPlayer, string damageType, vector position)
	{
		m_Timestamp = GetGame().GetTime();
		m_ActionType = actionType;
		m_DamageType = damageType;
		m_Position = position;
		
		if (otherPlayer && otherPlayer.GetIdentity())
		{
			m_OtherPlayerName = otherPlayer.GetIdentity().GetName();
			m_OtherPlayerID = otherPlayer.GetIdentity().GetPlainId();
			m_Distance = vector.Distance(position, otherPlayer.GetPosition());
		}
		else
		{
			m_OtherPlayerName = "";
			m_OtherPlayerID = "";
			m_Distance = 0;
		}
	}
	
	// Get formatted string for debugging
	string GetActionString()
	{
		return string.Format("[%1] %2 -> %3 (distance: %4m, damage: %5)", 
			m_Timestamp, m_ActionType, m_OtherPlayerName, m_Distance, m_DamageType);
	}
}

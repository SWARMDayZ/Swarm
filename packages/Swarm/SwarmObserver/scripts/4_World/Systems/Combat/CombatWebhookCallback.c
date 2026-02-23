// Webhook callback for combat logout notifications
class CombatWebhookCallback extends RestCallback
{
	string m_PlayerName;
	
	void CombatWebhookCallback(string playerName)
	{
		m_PlayerName = playerName;
	}
	
	override void OnSuccess(string data, int dataSize)
	{
		Print("[SwarmObserver] Combat logout webhook sent successfully for player: " + m_PlayerName);
	}
	
	override void OnError(int errorCode)
	{
		Print("[SwarmObserver] Combat logout webhook failed with error code: " + errorCode + " for player: " + m_PlayerName);
	}
	
	override void OnTimeout()
	{
		Print("[SwarmObserver] Combat logout webhook timeout for player: " + m_PlayerName);
	}
}

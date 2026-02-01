modded class BrainDiseaseMdfr : ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 2000;
	static const int AGENT_THRESHOLD_DEACTIVATE = 0;
	const int SHAKE_INTERVAL_MIN = 1;
	const int SHAKE_INTERVAL_MAX = 4;
	
	float m_Time;
	float m_ShakeTime;
	
	modded override void Init()
	{
		m_TrackActivatedTime	= false;
		m_AnalyticsStatsEnabled = true;
		m_ID 					= eModifiers.MDF_BRAIN;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	modded override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	modded override protected bool ActivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.BRAIN) >= AGENT_THRESHOLD_ACTIVATE);
	}

	modded override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();
		player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_LAUGHTER);
	}

	modded override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();
	}

	modded override protected bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.BRAIN) <= AGENT_THRESHOLD_DEACTIVATE);
	}

	modded override protected void OnTick(PlayerBase player, float deltaT)
	{
		m_Time += deltaT;
		float brainAgents = player.GetSingleAgentCountNormalized(eAgents.BRAIN) / 8.0;
		float chanceOfLaughter = Math.RandomFloat01();

		if (chanceOfLaughter < brainAgents / 3.0)
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_LAUGHTER);
		
		if (m_Time >= m_ShakeTime)
		{
			DayZPlayerSyncJunctures.SendKuruRequest(player, brainAgents);
			m_ShakeTime = m_Time + Math.RandomFloat(SHAKE_INTERVAL_MIN, SHAKE_INTERVAL_MAX);
		}
		
	}
}

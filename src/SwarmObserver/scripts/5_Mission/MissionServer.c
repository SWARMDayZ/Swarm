modded class MissionServer
{
	// Storage for players pending restricted area check
	private ref map<string, bool> m_SwarmObserverPendingPlayers;
	
	void MissionServer()
	{
		m_SwarmObserverPendingPlayers = new map<string, bool>;
	}
	
	override void OnInit()
	{
		super.OnInit();
		
		// Initialize SwarmObserver module (creates directories, loads config, loads history)
		SwarmObserverModule.Initialize();
	}
	
	override void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed)
	{
		super.OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
		
		// Only process if we have a valid player and identity
		if (!player || !identity)
			return;
		
		string steamID = identity.GetPlainId();
		string playerName = identity.GetName();
		vector playerPos = player.GetPosition();
		
		// Check if player is in a restricted area
		SwarmObserverRestrictedArea area = SwarmObserverSettings.GetRestrictedAreaAtPosition(playerPos);
		
		if (area)
		{
			Print("[SwarmObserver] Player " + playerName + " disconnecting from restricted area: " + area.Name);
			
			// Mark player as pending (disconnecting from restricted area)
			m_SwarmObserverPendingPlayers.Set(steamID, true);
			
			// Start grace period timer
			SwarmObserverGracePeriodManager.StartGracePeriod(steamID, playerName, area.Name);
		}
	}
	
	override void OnEvent(EventType eventTypeId, Param params)
	{
		super.OnEvent(eventTypeId, params);
		
		// Handle player ready event (player fully connected/reconnected)
		if (eventTypeId == ClientReadyEventTypeID)
		{
			ClientReadyEventParams readyParams = ClientReadyEventParams.Cast(params);
			if (readyParams)
			{
				PlayerIdentity identity = readyParams.param1;
				PlayerBase player = PlayerBase.Cast(readyParams.param2);
				
				if (identity && player)
				{
					string steamID = identity.GetPlainId();
					string playerName = identity.GetName();
					
					// Check if player has active grace period (reconnected in time)
					if (SwarmObserverGracePeriodManager.HasActiveGracePeriod(steamID))
					{
						SwarmObserverGracePeriodManager.CancelGracePeriod(steamID);
						Print("[SwarmObserver] Player " + playerName + " reconnected within grace period");
					}
					else
					{
						// Check if player has NEW violations and should see notification
						int unacknowledgedCount = SwarmObserverGracePeriodManager.GetReconnectViolationCount(steamID);
						
						if (unacknowledgedCount > 0)
						{
							// Get the TOTAL violation count to display (all violations in last X days)
							int totalViolationCount = SwarmObserverHistory.GetViolationCount(steamID);
							
							Print("[SwarmObserver] Player " + playerName + " reconnected with " + totalViolationCount + " total violations (" + unacknowledgedCount + " new)");
							
							// Send custom RPC to client to show reconnect notice menu with TOTAL count
							GetGame().RPCSingleParam(player, SwarmObserverRPC.RPC_SWARM_OBSERVER_SHOW_RECONNECT_NOTICE, 
								new Param1<int>(totalViolationCount), 
								true, identity);
						}
					}
				}
			}
		}
	}
}

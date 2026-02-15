modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		
		// Initialize SwarmObserver module (creates directories, loads config, loads history)
		SwarmObserverModule.Initialize();
	}
	
	// Called when player clicks disconnect - store data for when they actually disconnect
	// Note: DayZ developers being lazy mfs they do not pass identity or player to PlayerDisconnected because they use a separate logout timer
	// because of that we have to store the data here and then retrieve it in PlayerDisconnected
	override void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed)
	{
		super.OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
		
		if (!player || !identity)
			return;
		
		string steamID = identity.GetPlainId();
		string uid = identity.GetId();
		string playerName = identity.GetName();
		vector playerPos = player.GetPosition();
		
		// Check if player is in a restricted area
		SwarmObserverRestrictedArea area = SwarmObserverSettings.GetRestrictedAreaAtPosition(playerPos);
		string areaName = "";
		if (area)
			areaName = area.Name;
		
		// Check if player is in combat
		bool inCombat = GetCombatStateManager().IsInCombat(player);
		CombatState combatState = null;
		if (inCombat)
		{
			combatState = GetCombatStateManager().GetCombatState(player);
			Print("[SwarmObserver] Storing combat logout data for " + playerName + " (" + steamID + ")");
		}
		
		// Store data keyed by Steam ID for retrieval in PlayerDisconnected
		SwarmObserverLogoutManager.StoreLogoutData(uid, steamID, playerName, playerPos, areaName, inCombat, combatState);
	}
	
	// Called when player ACTUALLY disconnects (after logout timer)
	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		Print("[SwarmObserver] PlayerDisconnected called for uid: " + uid);
		
		// Retrieve stored data by Steam ID (uid is always valid even if identity is null)
		SwarmObserverLogoutData data = SwarmObserverLogoutManager.GetLogoutData(uid);
		
		if (data)
			Print("[SwarmObserver] Retrieved logout data for " + data.PlayerName + ", combat=" + data.WasInCombat.ToString());
		else
			Print("[SwarmObserver] No logout data found for uid: " + uid);
		
		if (data)
		{
			// Handle restricted area violation
			if (data.RestrictedAreaName != "")
			{
				Print("[SwarmObserver] Player " + data.PlayerName + " disconnected from restricted area: " + data.RestrictedAreaName);
				SwarmObserverGracePeriodManager.StartGracePeriod(data.SteamID, data.PlayerName, data.RestrictedAreaName);
			}
			
			// Handle combat logout
			if (data.WasInCombat && data.CombatStateData)
			{
				Print("[SwarmObserver] Player " + data.PlayerName + " disconnected while in combat");
				CombatWebhook.SendCombatLogoutNotification(data.SteamID, data.PlayerName, data.Position, data.CombatStateData);
			}
			
			// Clean up stored data
			SwarmObserverLogoutManager.RemoveLogoutData(uid);
		}
		
		// Clean up combat state
		if (player)
			GetCombatStateManager().RemoveCombatState(player);
		
		super.PlayerDisconnected(player, identity, uid);
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

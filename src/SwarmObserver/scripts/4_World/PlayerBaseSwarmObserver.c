// Custom RPC IDs for SwarmObserver
enum SwarmObserverRPC
{
	RPC_SWARM_OBSERVER_SHOW_RECONNECT_NOTICE = 28500,
	RPC_SWARM_OBSERVER_ACKNOWLEDGE_NOTICE = 28501
}

modded class PlayerBase
{
	// Static variable to pass violation count to menu
	private static int s_SwarmObserverViolationCount;
	
	static int GetSwarmObserverViolationCount()
	{
		return s_SwarmObserverViolationCount;
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		// Handle SwarmObserver reconnect notice RPC
		if (rpc_type == SwarmObserverRPC.RPC_SWARM_OBSERVER_SHOW_RECONNECT_NOTICE)
		{
			// Read violation count from RPC params
			Param1<int> data;
			if (!ctx.Read(data))
				return;
			
			int violationCount = data.param1;
			
			// Only show menu on client side
			if (GetGame().IsClient() && GetGame().GetUIManager() && !GetGame().GetUIManager().FindMenu(SwarmObserverConstants.MENU_SWARM_OBSERVER_NOTICE))
			{
				// Store violation count for menu to read
				s_SwarmObserverViolationCount = violationCount;
				
				// Open the reconnect notice menu using call queue (like DayZ's RPC_WARNING_TELEPORT)
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(GetGame().GetUIManager().EnterScriptedMenu, SwarmObserverConstants.MENU_SWARM_OBSERVER_NOTICE, null);
				GetGame().GetMission().AddActiveInputExcludes({"menu"});
			}
		}
		
		// Handle acknowledgement RPC from client
		if (rpc_type == SwarmObserverRPC.RPC_SWARM_OBSERVER_ACKNOWLEDGE_NOTICE)
		{
			if (sender && GetGame().IsServer())
			{
				string steamID = sender.GetPlainId();
				string playerName = sender.GetName();
				
				Print("[SwarmObserver] Player " + playerName + " acknowledged reconnect notice");
				
				// Mark violations as acknowledged in history
				SwarmObserverHistory.AcknowledgeViolations(steamID);
			}
		}
	}
	
	// Combat detection - override EEHitBy to detect damage
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{		
		// Only process on server side
		if (!GetGame().IsServer()) {
			super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
			return;
		}
		
		// Check if combat logout is enabled
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		if (!settings.CombatLogoutEnabled) {
			super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
			return;
		}
		
		// Get the attacker player from the damage source
		if (source)
		{
			PlayerBase attacker = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			
			// Only register if attacker is a different player
			if (attacker && attacker != this)
			{
				// Register combat action for victim (receiving damage)
				GetCombatStateManager().RegisterCombatAction(this, attacker, "DAMAGE_RECEIVED", ammo);
				
				// Register combat action for attacker (dealing damage)
				GetCombatStateManager().RegisterCombatAction(attacker, this, "DAMAGE_DEALT", ammo);
			}
		}

		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	override void EEKilled(Object killer)
	{
		// Only process on server side
		if (!GetGame().IsServer()) {
			super.EEKilled(killer);
			return;
		}
		
		// Check if combat logout is enabled
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		if (!settings.CombatLogoutEnabled) {
			super.EEKilled(killer);
			return;
		}

		// Delete combat state immediately on death to prevent post-death logout violations
		GetCombatStateManager().RemoveCombatState(this);

		super.EEKilled(killer);
	}
}

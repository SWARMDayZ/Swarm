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
}

// SwarmTest Module for Community Online Tools
class JMSwarmTestModule: JMRenderableModuleBase
{
	void JMSwarmTestModule()
	{
		GetPermissionsManager().RegisterPermission("Admin.SwarmTest");
		GetPermissionsManager().RegisterPermission("Admin.SwarmTest.Play");
		GetPermissionsManager().RegisterPermission("Admin.SwarmTest.Stop");
	}

	override bool HasAccess()
	{
		return GetPermissionsManager().HasPermission("Admin.SwarmTest");
	}

	override string GetLayoutRoot()
	{
		return "SwarmTest/gui/layouts/SwarmTest_form.layout";
	}

	override string GetTitle()
	{
		return "Animation Tester";
	}
	
	override string GetIconName()
	{
		return "ST";
	}

	override bool ImageIsIcon()
	{
		return false;
	}
	
	// RPC Setup
	override int GetRPCMin()
	{
		return JMSwarmTestModuleRPC.INVALID;
	}

	override int GetRPCMax()
	{
		return JMSwarmTestModuleRPC.COUNT;
	}

	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		switch (rpc_type)
		{
			case JMSwarmTestModuleRPC.PlayAnimation:
				RPC_PlayAnimation(ctx, sender, target);
				break;
			case JMSwarmTestModuleRPC.PlayAnimationClient:
				RPC_PlayAnimationClient(ctx, sender, target);
				break;
			case JMSwarmTestModuleRPC.StopAnimation:
				RPC_StopAnimation(ctx, sender, target);
				break;
			case JMSwarmTestModuleRPC.StopAnimationClient:
				RPC_StopAnimationClient(ctx, sender, target);
				break;
		}
	}
	
	// === Play Animation ===
	void PlayAnimation(int commandID, int animType, int stanceMask, array<string> guids)
	{
		if (IsMissionHost())
		{
			Exec_PlayAnimation(commandID, animType, stanceMask, guids, NULL);
		}
		else
		{
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(commandID);
			rpc.Write(animType);
			rpc.Write(stanceMask);
			rpc.Write(guids);
			rpc.Send(NULL, JMSwarmTestModuleRPC.PlayAnimation, true, NULL);
		}
	}
	
	private void RPC_PlayAnimation(ParamsReadContext ctx, PlayerIdentity senderRPC, Object target)
	{
		int commandID;
		if (!ctx.Read(commandID))
			return;

		int animType;
		if (!ctx.Read(animType))
			return;

		int stanceMask;
		if (!ctx.Read(stanceMask))
			return;

		array<string> guids;
		if (!ctx.Read(guids))
			return;

		JMPlayerInstance instance;
		if (!GetPermissionsManager().HasPermission("Admin.SwarmTest.Play", senderRPC, instance))
			return;

		Exec_PlayAnimation(commandID, animType, stanceMask, guids, senderRPC);
	}
	
	private void Exec_PlayAnimation(int commandID, int animType, int stanceMask, array<string> guids, PlayerIdentity ident)
	{
		array<JMPlayerInstance> players = GetPermissionsManager().GetPlayers(guids);
		
		// Check if this is a listen server (server + client in same process)
		bool isListenServer = GetGame().IsServer() && !GetGame().IsDedicatedServer();

		foreach (JMPlayerInstance playerInstance : players)
		{
			PlayerBase player = PlayerBase.Cast(playerInstance.PlayerObject);
			if (!player)
				continue;

			PlayerIdentity targetIdentity = player.GetIdentity();
			
			// On listen server: check if target is the local player
			// ident is NULL when called from host, so we check if target matches local player
			bool isLocalPlayer = false;
			if (isListenServer)
			{
				PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
				isLocalPlayer = (player == localPlayer);
			}
			
			if (isListenServer && isLocalPlayer)
			{
				// On listen server targeting ourselves: execute directly (no RPC needed)
				Print("[SwarmTest] Listen server: executing animation directly - commandID=" + commandID + ", type=" + animType);
				SwarmTestAnimInfo tempInfo = new SwarmTestAnimInfo("temp", commandID, "", animType, stanceMask);
				SwarmTestAnimationHandler.PlayAnimationEx(player, tempInfo);
			}
			else if (targetIdentity)
			{
				// Remote player: send RPC to their client
				Print("[SwarmTest] Server forwarding animation to client: commandID=" + commandID + ", type=" + animType + ", stanceMask=" + stanceMask);
				
				ScriptRPC rpc = new ScriptRPC();
				rpc.Write(commandID);
				rpc.Write(animType);
				rpc.Write(stanceMask);
				rpc.Send(player, JMSwarmTestModuleRPC.PlayAnimationClient, true, targetIdentity);
			}
			else
			{
				Print("[SwarmTest] Target player has no identity, skipping");
			}
		}
	}
	
	// Client receives this RPC and executes animation locally
	private void RPC_PlayAnimationClient(ParamsReadContext ctx, PlayerIdentity senderRPC, Object target)
	{
		// Only execute on client side (skip dedicated server)
		if (GetGame().IsDedicatedServer())
			return;
		
		int commandID;
		if (!ctx.Read(commandID))
			return;

		int animType;
		if (!ctx.Read(animType))
			return;

		int stanceMask;
		if (!ctx.Read(stanceMask))
			return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
		{
			Print("[SwarmTest] Client: Local player is null!");
			return;
		}

		Print("[SwarmTest] Client executing animation: commandID=" + commandID + ", type=" + animType + ", stanceMask=" + stanceMask);
		
		// Execute animation on local player
		SwarmTestAnimInfo tempInfo = new SwarmTestAnimInfo("temp", commandID, "", animType, stanceMask);
		SwarmTestAnimationHandler.PlayAnimationEx(player, tempInfo);
	}
	
	// === Stop Animation ===
	void StopAnimation(array<string> guids)
	{
		if (IsMissionHost())
		{
			Exec_StopAnimation(guids, NULL);
		}
		else
		{
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(guids);
			rpc.Send(NULL, JMSwarmTestModuleRPC.StopAnimation, true, NULL);
		}
	}
	
	private void RPC_StopAnimation(ParamsReadContext ctx, PlayerIdentity senderRPC, Object target)
	{
		array<string> guids;
		if (!ctx.Read(guids))
			return;

		JMPlayerInstance instance;
		if (!GetPermissionsManager().HasPermission("Admin.SwarmTest.Stop", senderRPC, instance))
			return;

		Exec_StopAnimation(guids, senderRPC);
	}
	
	private void Exec_StopAnimation(array<string> guids, PlayerIdentity ident)
	{
		array<JMPlayerInstance> players = GetPermissionsManager().GetPlayers(guids);
		
		// Check if this is a listen server (server + client in same process)
		bool isListenServer = GetGame().IsServer() && !GetGame().IsDedicatedServer();

		foreach (JMPlayerInstance playerInstance : players)
		{
			PlayerBase player = PlayerBase.Cast(playerInstance.PlayerObject);
			if (!player)
				continue;

			PlayerIdentity targetIdentity = player.GetIdentity();
			
			// On listen server: check if target is the local player
			bool isLocalPlayer = false;
			if (isListenServer)
			{
				PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
				isLocalPlayer = (player == localPlayer);
			}
			
			if (isListenServer && isLocalPlayer)
			{
				// On listen server targeting ourselves: execute directly (no RPC needed)
				Print("[SwarmTest] Listen server: stopping animation directly");
				SwarmTestAnimationHandler.EndCurrentAnimation(player);
			}
			else if (targetIdentity)
			{
				// Remote player: send RPC to their client
				Print("[SwarmTest] Server forwarding StopAnimation to client");
				
				ScriptRPC rpc = new ScriptRPC();
				rpc.Send(player, JMSwarmTestModuleRPC.StopAnimationClient, true, targetIdentity);
			}
			else
			{
				Print("[SwarmTest] Target player has no identity, skipping");
			}
		}
	}
	
	// Client receives this RPC and stops animation locally
	private void RPC_StopAnimationClient(ParamsReadContext ctx, PlayerIdentity senderRPC, Object target)
	{
		// Only execute on client side (skip dedicated server)
		if (GetGame().IsDedicatedServer())
			return;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
		{
			Print("[SwarmTest] Client: Local player is null!");
			return;
		}

		Print("[SwarmTest] Client stopping animation");
		SwarmTestAnimationHandler.EndCurrentAnimation(player);
	}
}

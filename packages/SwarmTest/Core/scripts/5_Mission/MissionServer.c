modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		Print("[SwarmTest] Server initialized - Animation commands enabled");
		GetRPCManager().AddRPC("", "SwarmTestRPC_AnimCommand", this, SingleplayerExecutionType.Server);
	}
	
	void SwarmTestRPC_AnimCommand(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type == CallType.Server)
		{
			string command;
			string parameter;
			
			if (!ctx.Read(command))
				return;
			if (!ctx.Read(parameter))
				return;
			
			// Get the player from the target object
			PlayerBase player = PlayerBase.Cast(target);
			if (player)
			{
				HandleAnimationCommand(player, command, parameter);
			}
		}
	}
	
	void HandleAnimationCommand(PlayerBase player, string command, string parameter)
	{
		if (!player)
			return;
		
		Print("[SwarmTest] Animation command from " + player.GetIdentity().GetName() + ": " + command + " " + parameter);
		
		if (command == "play")
		{
			if (SwarmTestAnimationHandler.FindAndPlayAnimation(player, parameter))
			{
				GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("Playing animation: " + parameter), true);
			}
			else
			{
				GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("Animation not found: " + parameter), true);
			}
		}
		else if (command == "stop")
		{
			SwarmTestAnimationHandler.EndCurrentAnimation(player);
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("Animation stopped"), true);
		}
		else if (command == "list")
		{
			string list = SwarmTestAnimationHandler.ListAnimations();
			// Split the list into chunks to avoid message size limits
			array<string> lines = new array<string>;
			list.Split("\n", lines);
			
			string chunk = "";
			foreach (string line : lines)
			{
				if (chunk.Length() + line.Length() > 200)
				{
					GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(chunk), true);
					chunk = "";
				}
				chunk += line + "\n";
			}
			
			if (chunk.Length() > 0)
			{
				GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(chunk), true);
			}
		}
		else if (command == "help")
		{
			string help = SwarmTestAnimationHandler.GetAnimationHelp();
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(help), true);
		}
	}
}

modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();
		Print("[SwarmTest] Initialized - Animation testing enabled");
	}
	
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		
		// Handle chat commands
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		if (!player)
			return;
	}
}

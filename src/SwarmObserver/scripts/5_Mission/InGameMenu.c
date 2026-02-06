modded class InGameMenu
{
	// Override the actual exit button click handler
	override protected void OnClick_Exit()
	{
		// Check if player is in a restricted area
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			vector playerPos = player.GetPosition();
			SwarmObserverRestrictedArea area = SwarmObserverSettings.GetRestrictedAreaAtPosition(playerPos);
			
			if (area)
			{
				// Player is in restricted area - show warning dialog
				Print("[SwarmObserver] Player attempting disconnect from restricted area: " + area.Name);
				
				// Open warning menu on top of InGameMenu (don't close InGameMenu)
				GetGame().GetUIManager().EnterScriptedMenu(SwarmObserverConstants.MENU_SWARM_OBSERVER_WARNING, GetGame().GetUIManager().GetMenu());
				
				// Prevent the default disconnect action
				return;
			}
		}
		
		// Not in restricted area - proceed with normal exit
		super.OnClick_Exit();
	}
}

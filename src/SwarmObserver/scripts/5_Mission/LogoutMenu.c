modded class LogoutMenu
{
	// Override the "Logout Now" button click handler
	override bool OnClick(Widget w, int x, int y, int button)
	{
		// m_bLogoutNow is the "Logout Now" button widget
		if (w == m_bLogoutNow)
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
					Print("[SwarmObserver] Player attempting logout from restricted area: " + area.Name);
					
					// Close logout menu
					Hide();
					Cancel();
					
					// Show warning dialog
					GetGame().GetUIManager().EnterScriptedMenu(SwarmObserverConstants.MENU_SWARM_OBSERVER_WARNING, null);
					
					// Prevent the default logout action
					return true;
				}
			}
		}
		
		// Not in restricted area - proceed with normal logout
		return super.OnClick(w, x, y, button);
	}
	
	// Override the automatic logout after timer expires
	override void Exit()
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
				Print("[SwarmObserver] Player logout timer expired in restricted area: " + area.Name);
				
				// Close logout menu
				Hide();
				Cancel();
				
				// Show warning dialog
				GetGame().GetUIManager().EnterScriptedMenu(SwarmObserverConstants.MENU_SWARM_OBSERVER_WARNING, null);
				
				// Prevent the default logout action
				return;
			}
		}
		
		// Not in restricted area - proceed with normal logout
		super.Exit();
	}
}

modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();
		
		// Load config on client side too (needed for restricted area checks)
		SwarmObserverSettings.LoadConfig();
	}
	
	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		UIScriptedMenu menu = null;
		
		switch (id)
		{
			case SwarmObserverConstants.MENU_SWARM_OBSERVER_WARNING:
				menu = new SwarmObserverDisconnectWarningMenu;
				break;
			
			case SwarmObserverConstants.MENU_SWARM_OBSERVER_NOTICE:
				menu = new SwarmObserverReconnectNoticeMenu;
				break;
		}
		
		if (menu)
		{
			menu.SetID(id);
			return menu;
		}
		
		return super.CreateScriptedMenu(id);
	}
}

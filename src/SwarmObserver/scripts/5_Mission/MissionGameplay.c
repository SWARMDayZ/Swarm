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
			
			case SwarmObserverConstants.MENU_SWARM_OBSERVER_LOGOUT:
				menu = new SwarmObserverLogoutMenu;
				break;
		}
		
		if (menu)
		{
			menu.SetID(id);
			return menu;
		}
		
		return super.CreateScriptedMenu(id);
	}
	
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		
		// Handle admin commands for testing (only on server)
		if (GetGame().IsServer())
		{
			HandleTestCommands();
		}
	}
	
	void HandleTestCommands()
	{
		// Check for player input
		Input input = GetGame().GetInput();
		if (!input)
			return;
		
		// Get local player for testing
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		
		// Check for F8 key press to spawn test infected
		if (input.LocalPress("UAUISelect"))
		{
			SpawnTestInfected(player);
		}
		
		// Check for F9 key to trigger test combat action
		if (input.LocalPress("UAUIBack"))
		{
			TestCombatProximity(player);
		}
		
		// Check for F10 key to show combat status
		if (input.LocalPress("UAUICtrlY"))
		{
			ShowCombatStatus(player);
		}
	}
	
	void SpawnTestInfected(PlayerBase player)
	{
		vector pos = player.GetPosition();
		vector dir = player.GetDirection();
		
		// Spawn infected 5 meters in front
		pos = pos + (dir * 5);
		
		EntityAI infected = EntityAI.Cast(GetGame().CreateObject("ZmbM_HermitSkinny_Beige", pos, false, true, true));
		if (infected)
		{
			Print("[SwarmObserver TEST] Spawned infected at " + pos.ToString());
			Print("[SwarmObserver TEST] Shoot it to test damage detection!");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().ObjectDelete, 30000, false, infected);
		}
	}
	
	void TestCombatProximity(PlayerBase player)
	{
		// Find all nearby players
		array<Object> objects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();
		GetGame().GetObjectsAtPosition(player.GetPosition(), 100, objects, proxyCargos);
		
		PlayerBase nearestPlayer = null;
		float nearestDistance = 999999;
		
		foreach (Object obj : objects)
		{
			PlayerBase otherPlayer = PlayerBase.Cast(obj);
			if (otherPlayer && otherPlayer != player && otherPlayer.GetIdentity())
			{
				float dist = vector.Distance(player.GetPosition(), otherPlayer.GetPosition());
				if (dist < nearestDistance)
				{
					nearestDistance = dist;
					nearestPlayer = otherPlayer;
				}
			}
		}
		
		if (nearestPlayer)
		{
			// Manually trigger combat proximity for testing
			GetCombatStateManager().RegisterCombatAction(player, nearestPlayer, "TEST_SHOT_NEAR", "TestAmmo");
			GetCombatStateManager().RegisterCombatAction(nearestPlayer, player, "TEST_SHOT_NEARBY", "TestAmmo");
			Print("[SwarmObserver TEST] Triggered test combat between " + player.GetIdentity().GetName() + " and " + nearestPlayer.GetIdentity().GetName() + " (distance: " + nearestDistance + "m)");
		}
		else
		{
			Print("[SwarmObserver TEST] No other players found within 100m");
		}
	}
	
	void ShowCombatStatus(PlayerBase player)
	{
		if (GetCombatStateManager().IsInCombat(player))
		{
			CombatState state = GetCombatStateManager().GetCombatState(player);
			if (state)
			{
				SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
				int remaining = state.GetRemainingTime(GetGame().GetTime(), settings.CombatDurationSeconds);
				Print("[SwarmObserver TEST] " + player.GetIdentity().GetName() + " IS IN COMBAT - " + remaining + "s remaining");
				Print("[SwarmObserver TEST] Involved players: " + state.m_InvolvedPlayers.Count());
				Print("[SwarmObserver TEST] Combat actions: " + state.m_CombatActions.Count());
			}
		}
		else
		{
			Print("[SwarmObserver TEST] " + player.GetIdentity().GetName() + " is NOT in combat");
		}
		
		Print("[SwarmObserver TEST] Total active combat states: " + GetCombatStateManager().GetActiveCombatCount());
	}
}

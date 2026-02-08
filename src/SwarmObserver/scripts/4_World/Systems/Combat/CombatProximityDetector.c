// Combat proximity detection helper
class CombatProximityDetector
{
	// Check for nearby players when a shot is fired
	static void CheckShotProximity(PlayerBase shooter, vector shotPosition)
	{
		if (!shooter || !GetGame().IsServer())
			return;
		
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		if (!settings.CombatLogoutEnabled)
			return;
		
		float radius = settings.ShotProximityRadius;
		array<Man> players = new array<Man>();
		array<Object> objects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();
		
		// Get all objects in radius
		GetGame().GetObjectsAtPosition(shotPosition, radius, objects, proxyCargos);
		
		// Find nearby players
		foreach (Object obj : objects)
		{
			PlayerBase nearbyPlayer = PlayerBase.Cast(obj);
			if (nearbyPlayer && nearbyPlayer != shooter && nearbyPlayer.GetIdentity())
			{
				float distance = vector.Distance(shotPosition, nearbyPlayer.GetPosition());
				if (distance <= radius)
				{
					// Register combat for both shooter and nearby player
					GetCombatStateManager().RegisterCombatAction(shooter, nearbyPlayer, "SHOT_NEAR_PLAYER", "");
					GetCombatStateManager().RegisterCombatAction(nearbyPlayer, shooter, "SHOT_NEARBY", "");
					
					if (settings.CombatDebugMode)
					{
						Print(string.Format("[SwarmObserver] Shot proximity: %1 fired near %2 (distance: %3m)",
							shooter.GetIdentity().GetName(), 
							nearbyPlayer.GetIdentity().GetName(),
							distance));
					}
				}
			}
		}
	}
	
	// Check for nearby players when an explosive is thrown/placed
	static void CheckExplosiveProximity(PlayerBase thrower, vector explosivePosition)
	{
		if (!thrower || !GetGame().IsServer())
			return;
		
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		if (!settings.CombatLogoutEnabled)
			return;
		
		float radius = settings.ExplosiveProximityRadius;
		array<Man> players = new array<Man>();
		array<Object> objects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();
		
		// Get all objects in radius
		GetGame().GetObjectsAtPosition(explosivePosition, radius, objects, proxyCargos);
		
		// Find nearby players
		foreach (Object obj : objects)
		{
			PlayerBase nearbyPlayer = PlayerBase.Cast(obj);
			if (nearbyPlayer && nearbyPlayer != thrower && nearbyPlayer.GetIdentity())
			{
				float distance = vector.Distance(explosivePosition, nearbyPlayer.GetPosition());
				if (distance <= radius)
				{
					// Register combat for both thrower and nearby player
					GetCombatStateManager().RegisterCombatAction(thrower, nearbyPlayer, "EXPLOSIVE_NEAR_PLAYER", "");
					GetCombatStateManager().RegisterCombatAction(nearbyPlayer, thrower, "EXPLOSIVE_NEARBY", "");
					
					if (settings.CombatDebugMode)
					{
						Print(string.Format("[SwarmObserver] Explosive proximity: %1 threw explosive near %2 (distance: %3m)",
							thrower.GetIdentity().GetName(), 
							nearbyPlayer.GetIdentity().GetName(),
							distance));
					}
				}
			}
		}
	}
}

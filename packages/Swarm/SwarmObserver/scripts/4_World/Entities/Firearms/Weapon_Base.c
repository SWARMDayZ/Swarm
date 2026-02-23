// Modded Weapon_Base for shot proximity detection
modded class Weapon_Base
{
	override void EEFired(int muzzleType, int mode, string ammoType)
	{
		super.EEFired(muzzleType, mode, ammoType);
		
		// Only process on server side
		if (!GetGame().IsServer())
			return;
		
		// Check if combat logout is enabled
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		if (!settings.CombatLogoutEnabled)
			return;
		
		// Get the shooter (player holding the weapon)
		PlayerBase shooter = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!shooter)
			return;
		
		// Check proximity to other players
		vector shooterPos = shooter.GetPosition();
		CombatProximityDetector.CheckShotProximity(shooter, shooterPos);
	}
}

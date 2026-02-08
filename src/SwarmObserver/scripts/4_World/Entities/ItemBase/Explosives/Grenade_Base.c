// Modded grenade class to detect explosive proximity
modded class Grenade_Base
{
	override void EEOnCECreate()
	{
		super.EEOnCECreate();
		
		// Only process on server side
		if (!GetGame().IsServer())
			return;
		
		// Check if combat logout is enabled
		SwarmObserverSettings settings = SwarmObserverSettings.GetInstance();
		if (!settings.CombatLogoutEnabled)
			return;
		
		// Get the thrower (player who threw the grenade)
		PlayerBase thrower = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!thrower)
			return;
		
		// Check proximity to other players
		vector explosivePos = this.GetPosition();
		CombatProximityDetector.CheckExplosiveProximity(thrower, explosivePos);
	}
}

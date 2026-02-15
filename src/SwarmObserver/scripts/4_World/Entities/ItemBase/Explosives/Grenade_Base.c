// Modded grenade class to detect explosive proximity
modded class Grenade_Base
{
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		// Capture thrower when leaving hands
		if (oldLoc.GetType() == InventoryLocationType.HANDS && newLoc.GetType() != InventoryLocationType.HANDS)
		{
			Print("[SwarmObserver] Grenade thrown, capturing thrower");
			m_Thrower = PlayerBase.Cast(oldLoc.GetParent());

			if (m_Thrower) {
				Print(string.Format("[SwarmObserver] Grenade thrower: %1", m_Thrower.GetIdentity().GetName()));
			} else {
				Print("[SwarmObserver] Warning: Grenade thrower is null");
			}
		}
		
		//! activate grenade when it leaves player hands (safety handle released)
		if (newLoc.GetType() != InventoryLocationType.HANDS && !IsPinned())
		{
			Activate();
		}
	}
	
	override void OnExplode()
	{
		super.OnExplode();
		
		if (!m_Thrower) {
			Print("[SwarmObserver] Warning: Grenade exploded but thrower is null");
			return;
		}

		Print(string.Format("[SwarmObserver] Grenade exploded, checking proximity for thrower %1", m_Thrower.GetIdentity().GetName()));

		// Get position of explosion (grenade's current position)
		vector explosionPosition = GetPosition();

		// Check for nearby players and register combat if needed
		CombatProximityDetector.CheckExplosiveProximity(m_Thrower, explosionPosition);
	}
}

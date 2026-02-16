// Modded FireplaceBase to add dry action to all fireplace types
modded class FireplaceBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDryFireplace);
	}
}

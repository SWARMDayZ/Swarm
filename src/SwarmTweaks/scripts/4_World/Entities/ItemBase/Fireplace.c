// Modded Fireplace to add dry action
modded class Fireplace
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDryFireplace);
	}
}

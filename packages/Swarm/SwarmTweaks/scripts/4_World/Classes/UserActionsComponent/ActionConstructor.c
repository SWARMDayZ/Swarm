// ActionConstructor - Register custom actions for SwarmTweaks
modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		
		// Register dry fireplace action
		actions.Insert(ActionDryFireplace);
	}
}

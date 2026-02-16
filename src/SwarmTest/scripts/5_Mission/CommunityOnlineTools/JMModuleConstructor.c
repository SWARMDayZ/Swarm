// Register SwarmTest module with COT
modded class JMModuleConstructor
{
	override void RegisterModules(out TTypenameArray modules)
	{
		super.RegisterModules(modules);
		modules.Insert(JMSwarmTestModule);
	}
}

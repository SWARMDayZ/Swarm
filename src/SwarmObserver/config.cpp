class CfgPatches
{
	class SwarmObserver
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts"};
	};
};

class CfgMods
{
	class SwarmObserver
	{
		dir = "SwarmObserver";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "SwarmObserver";
		credits = "";
		author = "Swarm";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		
		dependencies[] = {"Game","World","Mission"};
		
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"SwarmObserver/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"SwarmObserver/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"SwarmObserver/scripts/5_Mission"};
			};
		};
	};
};

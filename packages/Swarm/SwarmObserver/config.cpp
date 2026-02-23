// SwarmObserver is a mod that provides a way to observe players activity
// This mod aims to be used as a moderation tool for server admins
// The current implementation focuses on providing a logout menu on restricted areas and a way to track combat logout.
// The combat logout tracking is done without informing the player, and it will be used to provide evidence for server admins to take action against players who abuse combat logout.

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

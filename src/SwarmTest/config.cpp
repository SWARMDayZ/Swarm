// SwarmTest is a testing addon for Swarm mod development
// This addon provides tools to test DayZ animations via chat commands
// Only built when --test flag is specified

class CfgPatches
{
    class SwarmTest
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "JM_COT_Scripts"};
    };
};

class CfgMods
{
    class SwarmTest
    {
        dir = "SwarmTest";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmTest";
        credits = "";
        author = "Swarm";
        authorID = "0";
        version = "1.0";
        extra = 0;
        type = "mod";

        dependencies[] = {"Game", "World", "Mission"};

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"SwarmTest/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"SwarmTest/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SwarmTest/scripts/5_Mission"};
            };
        };
    };
};

// SwarmArenaCore provides shared data classes, configurations, RPC definitions,
// and API utilities for the SwarmArena competitive match system.

class CfgPatches
{
    class SwarmArenaCore
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "Json", "Http"};
    };
};

class CfgMods
{
    class SwarmArenaCore
    {
        dir = "SwarmArenaCore";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmArenaCore";
        credits = "Swarm";
        author = "Arky";
        authorID = "";
        version = "%VERSION%";
        extra = 0;
        type = "mod";

        dependencies[] = {"Game"};

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"SwarmArenaCore/scripts/3_Game"};
            };
        };
    };
};

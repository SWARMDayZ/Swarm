// SwarmArenaSpectator provides custom 3rd-person spectator camera for arena matches.

class CfgPatches
{
    class SwarmArenaSpectator
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"SwarmArenaCore"};
    };
};

class CfgMods
{
    class SwarmArenaSpectator
    {
        dir = "SwarmArenaSpectator";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmArenaSpectator";
        credits = "Swarm";
        author = "Arky";
        authorID = "";
        version = "%VERSION%";
        extra = 0;
        type = "mod";

        dependencies[] = {"Game", "World", "Mission"};

        inputs = "SwarmArenaSpectator/data/inputs.xml";

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"SwarmArenaSpectator/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"SwarmArenaSpectator/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SwarmArenaSpectator/scripts/5_Mission"};
            };
        };
    };
};

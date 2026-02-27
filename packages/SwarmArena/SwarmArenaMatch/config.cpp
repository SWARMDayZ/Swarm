// SwarmArenaMatch provides match controller, round system, buy system, and death handling.

class CfgPatches
{
    class SwarmArenaMatch
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"SwarmArenaCore", "SwarmArenaLobby", "SwarmArenaSpectator"};
    };
};

class CfgMods
{
    class SwarmArenaMatch
    {
        dir = "SwarmArenaMatch";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmArenaMatch";
        credits = "Swarm";
        author = "Arky";
        authorID = "";
        version = "%VERSION%";
        extra = 0;
        type = "mod";

        dependencies[] = {"Game", "World", "Mission"};

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"SwarmArenaMatch/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"SwarmArenaMatch/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SwarmArenaMatch/scripts/5_Mission"};
            };
        };
    };
};

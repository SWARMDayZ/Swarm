// SwarmArenaLobby provides lobby system, party management, and matchmaking queue.

class CfgPatches
{
    class SwarmArenaLobby
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"SwarmArenaCore"};
    };
};

class CfgMods
{
    class SwarmArenaLobby
    {
        dir = "SwarmArenaLobby";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmArenaLobby";
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
                files[] = {"SwarmArenaLobby/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"SwarmArenaLobby/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SwarmArenaLobby/scripts/5_Mission"};
            };
        };
    };
};

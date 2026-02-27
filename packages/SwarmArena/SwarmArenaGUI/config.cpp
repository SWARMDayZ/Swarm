// SwarmArenaGUI provides lobby menu, preset editor, buy menu, and match HUD.

class CfgPatches
{
    class SwarmArenaGUI
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"SwarmArenaCore", "SwarmArenaLobby", "SwarmArenaMatch", "SwarmArenaSpectator"};
    };
};

class CfgMods
{
    class SwarmArenaGUI
    {
        dir = "SwarmArenaGUI";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmArenaGUI";
        credits = "Swarm";
        author = "Arky";
        authorID = "";
        version = "%VERSION%";
        extra = 0;
        type = "mod";

        dependencies[] = {"Game", "World", "Mission"};

        inputs = "SwarmArenaGUI/data/inputs.xml";

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"SwarmArenaGUI/scripts/3_Game"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SwarmArenaGUI/scripts/5_Mission"};
            };
        };
    };
};

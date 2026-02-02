class CfgPatches
{
    class SwarmSpectator
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"JM_COT_Scripts"};
    };
};

class CfgMods
{
    class SwarmSpectator
    {
        dir = "SwarmSpectator";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmSpectator";
        credits = "Swarm";
        author = "Kize";
        authorID = "";
        version = "%VERSION%";
        extra = 0;
        type = "mod";

        dependencies[] = {"Game", "World", "Mission"};
        inputs = "SwarmSpectator/data/inputs.xml";

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"SwarmSpectator/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"SwarmSpectator/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SwarmSpectator/scripts/5_Mission"};
            };
        };
    };
};

// SwarmSpectator is a mod that overrides the COT (Community Online Tools by Jacob-Mango) spectator system to provide
// a way to delete marker, light and lock-on target from the camera.
// This mod is NOT a repack of COT, it needs CF and COT to work, and it only overrides the spectator camera functionality of COT.
// The main purpose of this mod is to provide a way for server admins to film cool stuff

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

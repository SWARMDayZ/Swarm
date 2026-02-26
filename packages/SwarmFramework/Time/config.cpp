// Time provides date/time formatting utilities for Swarm mods.

class CfgPatches
{
    class Time
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class Time
    {
        dir = "Time";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Time";
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
                files[] = {"Time/scripts/3_Game"};
            };
        };
    };
};

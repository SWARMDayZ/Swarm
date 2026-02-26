// Http provides HTTP request helpers and a generic webhook callback for Swarm mods.

class CfgPatches
{
    class Http
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class Http
    {
        dir = "Http";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Http";
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
                files[] = {"Http/scripts/3_Game"};
            };
        };
    };
};

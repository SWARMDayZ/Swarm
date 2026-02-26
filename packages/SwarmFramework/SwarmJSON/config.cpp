// SwarmJSON provides JSON utility functions for Swarm mods.
// Includes a fluent JSON builder (SwarmJsonBuilder), a dynamic JSON value tree (SwarmJsonNode),
// and static helper methods for serialization, deserialization, escaping, parsing and stringification.

class CfgPatches
{
    class SwarmJSON
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class SwarmJSON
    {
        dir = "SwarmJSON";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmJSON";
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
                files[] = {"SwarmJSON/scripts/3_Game"};
            };
        };
    };
};

// Json provides JSON utility functions for Swarm mods.
// Includes a fluent JSON builder (JsonBuilder), a dynamic JSON value tree (JsonNode),
// and static helper methods for serialization, deserialization, escaping, parsing and stringification.

class CfgPatches
{
    class Json
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class Json
    {
        dir = "Json";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Json";
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
                files[] = {"Json/scripts/3_Game"};
            };
        };
    };
};

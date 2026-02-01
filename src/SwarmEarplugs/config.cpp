class CfgPatches
{
    class SwarmEarplugs
    {
        requiredAddons[] = {};
    };
};

class CfgMods
{
    class SwarmEarplugs
    {
        name = "SwarmEarplugs";
        type = "mod";
        dependencies[] = {"Mission"};
        inputs = "SwarmEarplugs/data/inputs.xml";
        
        class defs
        {
            class missionScriptModule
            {
                files[] =
                {
                    "SwarmEarplugs/scripts/5_Mission"
                };
            };
        };
    };
};

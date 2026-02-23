// SwarmEarplugs is a repack of the original EarPlugs mod by DaemonF0rge.
// Github: https://github.com/DaemonForge/DayZ-EarPlugs/
// Workshop: https://steamcommunity.com/workshop/filedetails/?id=1819514788
// Thanks to DaemonF0rge for creating the original mod and allowing it to be repacked without permission nor credits.

// We added Swarm branding and made some minor adjustments to fit it into the Swarm ecosystem, but the core functionality 
// remains the same as the original EarPlugs mod.

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

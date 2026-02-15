// SwarmTweaks is a mod that provides various tweaks to existing items in the game
// This mod adds a new fishing rod and tripod that can be shouldered, denies player to bury
// big bags in the ground (we want them to have a hard time with the Lantia suit), modifies
// the BrainDisease and Influenza to make the player laugh and sneeze less and finally makes 
// the ImprovisedSuppressor use a whole duct tape to craft.

class CfgPatches
{
    class SwarmTweaks
    {
        units[] = {"Swarm_FishingRod", "Swarm_Tripod"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Gear_Tools", "DZ_Gear_Cooking"};
    };
};

class CfgVehicles
{
    class FishingRod;
    class FishingRod_Base;

    class Swarm_FishingRod : FishingRod
    {
        scope = 2;                               
        displayName = "$STR_CfgVehicles_FishingRod0"; 
        descriptionShort = "$STR_CfgVehicles_FishingRod1";
        inventorySlot[] = {"Shoulder", "Melee", "Backpack_1"};
    };

    class Tripod;

    class Swarm_Tripod : Tripod
    {
        scope = 2;
        displayName = "$STR_CfgVehicles_Tripod0";
        descriptionShort = "$STR_CfgVehicles_Tripod1";
        inventorySlot[] = {"Shoulder", "Melee", "CookingTripod", "Backpack_1"};
    };
};

class CfgMods
{
    class SwarmTweaks
    {
        dir = "SwarmTweaks";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmTweaks";
        credits = "Swarm";
        author = "Kize";
        authorID = "";
        version = "%VERSION%";
        extra = 0;
        type = "mod";

        dependencies[] = {"Game", "World"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"SwarmTweaks/scripts/4_World"};
            };
        };
    };
};

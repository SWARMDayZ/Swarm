class CfgPatches
{
    class ItemsInBack
    {
        units[] = {"Swarm_FishingRod","Swarm_Tripod"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Gear_Tools","DZ_Gear_Cooking"};
    };
    class EarPlugs
  {
    requiredAddons[]={};
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
        inventorySlot[] = {"Shoulder","Melee"};
        
    };

    
    class Tripod;
    class Swarm_Tripod : Tripod
    {
        scope = 2;
        displayName = "$STR_CfgVehicles_Tripod0";
        descriptionShort = "$STR_CfgVehicles_Tripod1";
        inventorySlot[] = {"Shoulder", "Melee"};
    };
};

class CfgMods
{
    class Swarm
    {
        dir = "Swarm";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Swarm";
        credits = "";
        author = "Kize";
        authorID = "";
        version = "1.0.1";
        extra = 0;
        type = "mod";
        inputs = "data\inputs.xml";

        dependencies[] = {"Game", "World", "Mission"};

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"Swarm/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"Swarm/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"Swarm/scripts/5_Mission"};
            };
        };
    };
};
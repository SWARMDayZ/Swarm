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
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Gear_Tools", "DZ_Gear_Cooking", "DZ_Sounds_Effects"};
    };
};

// Sound Shaders for DryFireplace action
class CfgSoundShaders
{
    class DryFireplace_Start_SoundShader
    {
        samples[] = {
            {"SwarmTweaks\sounds\actions\dryfireplace\dry_fireplace_start", 1}
        };
        volume = 1;
        range = 30;
    };

    class DryFireplace_Loop_SoundShader
    {
        samples[] = {
            {"SwarmTweaks\sounds\actions\dryfireplace\dry_fireplace_loop", 1}
        };
        volume = 1;
        range = 30;
    };

    class DryFireplace_Finish_SoundShader
    {
        samples[] = {
            {"SwarmTweaks\sounds\actions\dryfireplace\dry_fireplace_finish", 1}
        };
        volume = 1;
        range = 30;
    };
};

// Sound Sets for DryFireplace action
class CfgSoundSets
{
    class DryFireplace_Start_SoundSet
    {
        soundShaders[] = {"DryFireplace_Start_SoundShader"};
        volumeFactor = 1;
        frequencyFactor = 1;
        spatial = 1;
        doppler = 0;
        loop = 0;
    };

    class DryFireplace_Loop_SoundSet
    {
        soundShaders[] = {"DryFireplace_Loop_SoundShader"};
        volumeFactor = 1;
        frequencyFactor = 1;
        spatial = 1;
        doppler = 0;
        loop = 1;
    };

    class DryFireplace_Finish_SoundSet
    {
        soundShaders[] = {"DryFireplace_Finish_SoundShader"};
        volumeFactor = 1;
        frequencyFactor = 1;
        spatial = 1;
        doppler = 0;
        loop = 0;
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

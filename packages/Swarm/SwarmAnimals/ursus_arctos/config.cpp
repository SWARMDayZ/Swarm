class CfgPatches
{
    class Swarm_UrsusArctos
    {
        units[] = {"Animal_UrsusArctos"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Animals"};
    };
};

class CfgVehicles
{
    class AnimalBase;
    class Animal_UrsusArctos : AnimalBase
    {
        class Skinning
        {
            class ObtainedSteaks
            {
                item = "BearSteakMeat";
                count = 7;
                itemZones[]=
                {
                    "Zone_Head",
                    "Zone_Chest",
                    "Zone_Belly",
                    "Zone_Pelvis"
                };
                countByZone[]={1,2,3,1};
                quantityMinMaxCoef[]={0.5,1};
            };
            class ObtainedPelt
            {
                item="BearPelt";
                count=2;
                itemZones[]=
                {
                    "Zone_Chest",
                    "Zone_Belly"
                };
                quantityCoef=1;
                transferToolDamageCoef=1;
            };
            class ObtainedGuts
            {
                item="Guts";
                count=3;
                itemZones[]=
                {
                    "Zone_Chest",
                    "Zone_Belly"
                };
                countByZone[]={1,2};
                quantityMinMaxCoef[]={0.5,1};
            };
            class ObtainedLard
            {
                item="Lard";
                count=3;
                itemZones[]=
                {
                    "Zone_Chest",
                    "Zone_Belly"
                };
                countByZone[]={2,1};
                quantityMinMaxCoef[]={0.5,1};
            };
            class ObtainedBones
            {
                item="Bone";
                count=3;
                itemZones[]=
                {
                    "Zone_Chest",
                    "Zone_Belly"
                };
                countByZone[]={1,2};
                quantityMinMaxCoef[]={0.5,1};
            };
        };
    };
};

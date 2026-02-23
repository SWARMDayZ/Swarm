class CfgPatches
{
    class Swarm_UrsusMaritimus
    {
        units[] = {"Animal_UrsusMaritimus"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Animals"};
    };
};

class CfgVehicles
{
    class Animal_UrsusArctos;
    class Animal_UrsusMaritimus : Animal_UrsusArctos
    {
        class Skinning
        {
            class ObtainedSteaks
            {
                item = "BearSteakMeat";
                count = 9;
                itemZones[]=
                {
                    "Zone_Head",
                    "Zone_Chest",
                    "Zone_Belly",
                    "Zone_Pelvis"
                };
                countByZone[]={2,3,3,1};
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

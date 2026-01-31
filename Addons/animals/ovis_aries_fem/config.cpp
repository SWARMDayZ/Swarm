class CfgPatches
{
    class Swarm_OvisAriesF
    {
        units[] = {"Animal_OvisAriesF"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Animals"};
    };
};

class CfgVehicles
{
    class AnimalBase;
    class Animal_OvisAriesF : AnimalBase
    {
        class Skinning
        {
             class ObtainedSteaks
            {
                item="SheepSteakMeat";
                count=5;
                itemZones[]=
                {
                    "Zone_Head",
                    "Zone_Chest",
                    "Zone_Belly",
                    "Zone_Pelvis"
                };
                countByZone[]={1,2,1,1};
                quantityMinMaxCoef[]={0.5,1};
            };
            class ObtainedPelt
            {
                item="SheepPelt";
                count=1;
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
                count=2;
                quantityMinMaxCoef[]={0.5,0.80000001};
            };
            class ObtainedLard
            {
                item="Lard";
                count=1;
                quantityMinMaxCoef[]={0.5,1};
            };
            class ObtainedBones
            {
                item="Bone";
                count=2;
                quantityMinMaxCoef[]={0.69999999,1};
                transferToolDamageCoef=1;
            };
        };
    };
};

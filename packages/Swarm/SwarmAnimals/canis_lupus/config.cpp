class CfgPatches
{
    class Swarm_CanisLupus
    {
        units[] = {"Animal_CanisLupus_Grey", "Animal_CanisLupus_White"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Animals"};
    };
};

class CfgVehicles
{
    class AnimalBase;
    class Animal_CanisLupus : AnimalBase
    {
        class Skinning
        {
            class ObtainedHead
            {
                item="Headdress_Wolf";
                count=0;
                itemZones[]=
                {
                    "Zone_Head"
                };
                countByZone[]={0};
                quantityMinMaxCoef[]={0,0.55000001};
            };
            class ObtainedSteaks
            {
                item="WolfSteakMeat";
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
                item="WolfPelt";
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
                count=1;
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
                count=1;
                quantityMinMaxCoef[]={0.69999999,1};
                transferToolDamageCoef=1;
            };
        };
    };

    class Animal_CanisLupus_Grey : Animal_CanisLupus {};
    class Animal_CanisLupus_White : Animal_CanisLupus {};
};

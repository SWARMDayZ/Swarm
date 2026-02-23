// SwarmAnimals is a mod to modify animal drops
// Nothing more
// But it takes a lot of files (because we wanted to make it clean and modular for future expansion, spoiler: we never did)
// Yes this is way too much comment lines for nothing
// Please take care of yourself ♥

class CfgPatches
{
    class SwarmAnimals
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Animals"};
    };
};

class CfgMods
{
    class SwarmAnimals
    {
        dir = "SwarmAnimals";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "SwarmAnimals";
        credits = "Swarm";
        author = "Kize";
        authorID = "";
        version = "%VERSION%";
        extra = 0;
        type = "mod";

        dependencies[] = {};

        class defs
        {
        };
    };
};

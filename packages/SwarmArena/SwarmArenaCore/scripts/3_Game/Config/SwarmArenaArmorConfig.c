// SwarmArenaArmorConfig - Armor configuration loaded from $profile:SwarmArena/armor.json

class SwarmArenaArmorEntry
{
    string Type;
    string Classname;
    int Price;
    int Protection;
    
    void SwarmArenaArmorEntry()
    {
        Type = "";
        Classname = "";
        Price = 0;
        Protection = 0;
    }
    
    void Set(string type, string classname, int price, int protection)
    {
        Type = type;
        Classname = classname;
        Price = price;
        Protection = protection;
    }
}

class SwarmArenaArmorConfig
{
    private static ref SwarmArenaArmorConfig s_Instance;
    private static const string CONFIG_PATH = "$profile:SwarmArena/armor.json";
    
    ref array<ref SwarmArenaArmorEntry> Armors = new array<ref SwarmArenaArmorEntry>();
    
    static SwarmArenaArmorConfig GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaArmorConfig();
        }
        return s_Instance;
    }
    
    static SwarmArenaArmorConfig Get()
    {
        return GetInstance();
    }
    
    static bool Load()
    {
        bool loaded = false;
        
        if (FileExist(CONFIG_PATH))
        {
            FileHandle file = OpenFile(CONFIG_PATH, FileMode.READ);
            if (file)
            {
                string content;
                string line;
                while (FGets(file, line) >= 0)
                {
                    content += line;
                }
                CloseFile(file);
                
                string error;
                Json.Deserialize(content, GetInstance(), error);
                if (error != "")
                {
                    SwarmArenaCore.LogError("Armor config parse error: " + error);
                }
            }
            SwarmArenaCore.Log("Armor config loaded from " + CONFIG_PATH);
            loaded = true;
        }
        else
        {
            SwarmArenaCore.Log("Armor config not found, creating defaults at " + CONFIG_PATH);
            GetInstance().SetDefaults();
            GetInstance().Save();
            loaded = true;
        }
        
        return loaded;
    }
    
    void SetDefaults()
    {
        Armors.Clear();
        
        ref SwarmArenaArmorEntry light = new SwarmArenaArmorEntry();
        light.Set("Light", "PlateCarrierVest", 200, 25);
        Armors.Insert(light);
        
        ref SwarmArenaArmorEntry medium = new SwarmArenaArmorEntry();
        medium.Set("Medium", "BallisticVest", 400, 50);
        Armors.Insert(medium);
        
        ref SwarmArenaArmorEntry heavy = new SwarmArenaArmorEntry();
        heavy.Set("Heavy", "PressVest_Blue", 650, 75);
        Armors.Insert(heavy);
    }
    
    bool Save()
    {
        if (!FileExist("$profile:SwarmArena"))
        {
            MakeDirectory("$profile:SwarmArena");
        }
        
        string json;
        Json.Serialize(this, json);
        
        FileHandle file = OpenFile(CONFIG_PATH, FileMode.WRITE);
        if (file)
        {
            FPrint(file, json);
            CloseFile(file);
        }
        SwarmArenaCore.Log("Armor config saved to " + CONFIG_PATH);
        return true;
    }
    
    SwarmArenaArmorEntry GetArmorByType(SwarmArenaArmorType type)
    {
        string typeName;
        switch (type)
        {
            case SwarmArenaArmorType.LIGHT:
                typeName = "Light";
                break;
            case SwarmArenaArmorType.MEDIUM:
                typeName = "Medium";
                break;
            case SwarmArenaArmorType.HEAVY:
                typeName = "Heavy";
                break;
            default:
                return null;
        }
        
        foreach (SwarmArenaArmorEntry entry : Armors)
        {
            if (entry.Type == typeName)
            {
                return entry;
            }
        }
        
        return null;
    }
    
    int GetArmorPrice(SwarmArenaArmorType type)
    {
        SwarmArenaArmorEntry entry = GetArmorByType(type);
        if (entry)
        {
            return entry.Price;
        }
        return 0;
    }
}

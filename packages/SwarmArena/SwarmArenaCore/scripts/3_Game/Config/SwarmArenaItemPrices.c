// SwarmArenaItemPrices - Item pricing loaded from $profile:SwarmArena/item_prices.json

class SwarmArenaItemPricesData
{
    ref map<string, int> Weapons = new map<string, int>();
    ref map<string, int> Attachments = new map<string, int>();
    ref map<string, int> Magazines = new map<string, int>();
    ref map<string, int> Consumables = new map<string, int>();
}

class SwarmArenaItemPrices
{
    private static ref SwarmArenaItemPrices s_Instance;
    private static const string CONFIG_PATH = "$profile:SwarmArena/item_prices.json";
    
    ref SwarmArenaItemPricesData Items = new SwarmArenaItemPricesData();
    
    // Quick lookup cache (classname -> price)
    private ref map<string, int> m_PriceCache = new map<string, int>();
    
    static SwarmArenaItemPrices GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaItemPrices();
        }
        return s_Instance;
    }
    
    static SwarmArenaItemPrices Get()
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
                    SwarmArenaCore.LogError("Item prices parse error: " + error);
                }
            }
            GetInstance().BuildCache();
            SwarmArenaCore.Log("Item prices loaded from " + CONFIG_PATH);
            loaded = true;
        }
        else
        {
            SwarmArenaCore.Log("Item prices not found, creating defaults at " + CONFIG_PATH);
            GetInstance().SetDefaults();
            GetInstance().Save();
            loaded = true;
        }
        
        return loaded;
    }
    
    void SetDefaults()
    {
        if (!Items)
        {
            Items = new SwarmArenaItemPricesData();
        }
        
        // Weapons
        Items.Weapons.Clear();
        Items.Weapons.Insert("M4A1", 550);
        Items.Weapons.Insert("AKM", 500);
        Items.Weapons.Insert("AK74", 480);
        Items.Weapons.Insert("MP5K", 300);
        Items.Weapons.Insert("UMP45", 350);
        Items.Weapons.Insert("VSS", 600);
        Items.Weapons.Insert("SVD", 700);
        Items.Weapons.Insert("Mosin9130", 400);
        Items.Weapons.Insert("SKS", 350);
        Items.Weapons.Insert("FX45", 250);
        Items.Weapons.Insert("Glock19", 200);
        
        // Attachments
        Items.Attachments.Clear();
        Items.Attachments.Insert("M4_RISHndgrd", 50);
        Items.Attachments.Insert("M4_OEBttstck", 40);
        Items.Attachments.Insert("M4_MPBttstck", 45);
        Items.Attachments.Insert("ACOGOptic", 100);
        Items.Attachments.Insert("M68Optic", 80);
        Items.Attachments.Insert("ReflexOptic", 60);
        Items.Attachments.Insert("PSO1Optic", 90);
        Items.Attachments.Insert("PSO11Optic", 120);
        Items.Attachments.Insert("KobraOptic", 70);
        Items.Attachments.Insert("M4_Suppressor", 150);
        Items.Attachments.Insert("AK_Suppressor", 150);
        Items.Attachments.Insert("PistolSuppressor", 100);
        
        // Magazines
        Items.Magazines.Clear();
        Items.Magazines.Insert("Mag_STANAG_30Rnd", 20);
        Items.Magazines.Insert("Mag_STANAGCoupled_30Rnd", 35);
        Items.Magazines.Insert("Mag_AKM_30Rnd", 20);
        Items.Magazines.Insert("Mag_AK74_30Rnd", 20);
        Items.Magazines.Insert("Mag_MP5_30Rnd", 15);
        Items.Magazines.Insert("Mag_UMP_25Rnd", 15);
        Items.Magazines.Insert("Mag_VSS_10Rnd", 25);
        Items.Magazines.Insert("Mag_SVD_10Rnd", 30);
        Items.Magazines.Insert("Mag_FX45_15Rnd", 10);
        Items.Magazines.Insert("Mag_Glock_15Rnd", 10);
        
        // Consumables
        Items.Consumables.Clear();
        Items.Consumables.Insert("BandageDressing", 10);
        Items.Consumables.Insert("Morphine", 30);
        Items.Consumables.Insert("Epinephrine", 25);
        Items.Consumables.Insert("SalineBagIV", 40);
        Items.Consumables.Insert("BloodBagIV", 50);
        
        BuildCache();
    }
    
    void BuildCache()
    {
        m_PriceCache.Clear();
        
        if (Items.Weapons)
        {
            for (int i = 0; i < Items.Weapons.Count(); i++)
            {
                string weaponKey = Items.Weapons.GetKey(i);
                m_PriceCache.Insert(weaponKey, Items.Weapons.Get(weaponKey));
            }
        }
        
        if (Items.Attachments)
        {
            for (int j = 0; j < Items.Attachments.Count(); j++)
            {
                string attachKey = Items.Attachments.GetKey(j);
                m_PriceCache.Insert(attachKey, Items.Attachments.Get(attachKey));
            }
        }
        
        if (Items.Magazines)
        {
            for (int k = 0; k < Items.Magazines.Count(); k++)
            {
                string magKey = Items.Magazines.GetKey(k);
                m_PriceCache.Insert(magKey, Items.Magazines.Get(magKey));
            }
        }
        
        if (Items.Consumables)
        {
            for (int l = 0; l < Items.Consumables.Count(); l++)
            {
                string consumeKey = Items.Consumables.GetKey(l);
                m_PriceCache.Insert(consumeKey, Items.Consumables.Get(consumeKey));
            }
        }
        
        SwarmArenaCore.Log("Item price cache built with " + m_PriceCache.Count().ToString() + " items");
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
        SwarmArenaCore.Log("Item prices saved to " + CONFIG_PATH);
        return true;
    }
    
    static int GetPrice(string classname)
    {
        if (GetInstance().m_PriceCache.Contains(classname))
        {
            return GetInstance().m_PriceCache.Get(classname);
        }
        return 0;
    }
    
    static bool HasPrice(string classname)
    {
        return GetInstance().m_PriceCache.Contains(classname);
    }
}

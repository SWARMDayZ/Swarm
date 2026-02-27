// SwarmArenaMapsConfig - Map configuration loaded from $profile:SwarmArena/maps.json

class SwarmArenaMapData
{
    string Name;
    ref array<vector> Team1Spawns = new array<vector>();
    ref array<vector> Team2Spawns = new array<vector>();
    vector SpectatorPosition;
    
    void SwarmArenaMapData()
    {
        Name = "";
        SpectatorPosition = "0 0 0";
    }
    
    void Set(string name, vector spectatorPos)
    {
        Name = name;
        SpectatorPosition = spectatorPos;
    }
    
    void AddTeam1Spawn(vector pos)
    {
        Team1Spawns.Insert(pos);
    }
    
    void AddTeam2Spawn(vector pos)
    {
        Team2Spawns.Insert(pos);
    }
    
    vector GetTeam1Spawn(int index)
    {
        if (index >= 0 && index < Team1Spawns.Count())
        {
            return Team1Spawns[index];
        }
        return "0 0 0";
    }
    
    vector GetTeam2Spawn(int index)
    {
        if (index >= 0 && index < Team2Spawns.Count())
        {
            return Team2Spawns[index];
        }
        return "0 0 0";
    }
    
    int GetMaxPlayersPerTeam()
    {
        return Math.Min(Team1Spawns.Count(), Team2Spawns.Count());
    }
}

class SwarmArenaMapsConfig
{
    private static ref SwarmArenaMapsConfig s_Instance;
    private static const string CONFIG_PATH = "$profile:SwarmArena/maps.json";
    
    ref array<ref SwarmArenaMapData> Maps = new array<ref SwarmArenaMapData>();
    
    static SwarmArenaMapsConfig GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaMapsConfig();
        }
        return s_Instance;
    }
    
    static SwarmArenaMapsConfig Get()
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
                    SwarmArenaCore.LogError("Maps config parse error: " + error);
                }
            }
            SwarmArenaCore.Log("Maps config loaded from " + CONFIG_PATH + " (" + GetInstance().Maps.Count().ToString() + " maps)");
            loaded = true;
        }
        else
        {
            SwarmArenaCore.Log("Maps config not found, creating defaults at " + CONFIG_PATH);
            GetInstance().SetDefaults();
            GetInstance().Save();
            loaded = true;
        }
        
        return loaded;
    }
    
    void SetDefaults()
    {
        Maps.Clear();
        
        // Default industrial arena
        ref SwarmArenaMapData industrial = new SwarmArenaMapData();
        industrial.Set("Industrial", "2050 50 3000");
        industrial.AddTeam1Spawn("2000 10 3000");
        industrial.AddTeam1Spawn("2005 10 3005");
        industrial.AddTeam1Spawn("2010 10 3000");
        industrial.AddTeam2Spawn("2100 10 3000");
        industrial.AddTeam2Spawn("2105 10 3005");
        industrial.AddTeam2Spawn("2110 10 3000");
        Maps.Insert(industrial);
        
        // Default forest arena
        ref SwarmArenaMapData forest = new SwarmArenaMapData();
        forest.Set("Forest", "5100 40 5000");
        forest.AddTeam1Spawn("5000 15 5000");
        forest.AddTeam1Spawn("5005 15 5005");
        forest.AddTeam1Spawn("5010 15 5000");
        forest.AddTeam2Spawn("5200 15 5000");
        forest.AddTeam2Spawn("5205 15 5005");
        forest.AddTeam2Spawn("5210 15 5000");
        Maps.Insert(forest);
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
        SwarmArenaCore.Log("Maps config saved to " + CONFIG_PATH);
        return true;
    }
    
    SwarmArenaMapData GetRandomMap()
    {
        if (Maps.Count() == 0)
        {
            SwarmArenaCore.LogError("No maps configured!");
            return null;
        }
        
        int index = Math.RandomInt(0, Maps.Count());
        return Maps[index];
    }
    
    SwarmArenaMapData GetMapByName(string name)
    {
        foreach (SwarmArenaMapData mapData : Maps)
        {
            if (mapData.Name == name)
            {
                return mapData;
            }
        }
        return null;
    }
}

// SwarmArenaConfig - Main configuration loaded from $profile:SwarmArena/config.json

class SwarmArenaRewardsConfig
{
    int Kill = 100;
    int RoundWin = 200;
    int MatchWin = 500;
    int MatchLose = -100;
}

class SwarmArenaConfig
{
    private static ref SwarmArenaConfig s_Instance;
    private static const string CONFIG_PATH = "$profile:SwarmArena/config.json";
    
    // Lobby settings
    vector LobbyPosition = "1000 10 1000";
    float LobbyRadius = 50.0;
    
    // Economy settings
    int StartingMoney = 800;
    int PresetBudgetLimit = 800;
    int MaxPresets = 5;
    
    // Match settings
    int FreezeTimeSeconds = 15;
    int RoundsToWin = 5;
    int OvertimeThreshold = 4;
    int OvertimeWinMargin = 2;
    
    // Rewards
    ref SwarmArenaRewardsConfig Rewards = new SwarmArenaRewardsConfig();
    
    static SwarmArenaConfig GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaConfig();
        }
        return s_Instance;
    }
    
    static SwarmArenaConfig Get()
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
                    SwarmArenaCore.LogError("Config parse error: " + error);
                }
            }
            SwarmArenaCore.Log("Config loaded from " + CONFIG_PATH);
            loaded = true;
        }
        else
        {
            SwarmArenaCore.Log("Config not found, creating defaults at " + CONFIG_PATH);
            GetInstance().SetDefaults();
            GetInstance().Save();
            loaded = true;
        }
        
        return loaded;
    }
    
    void SetDefaults()
    {
        LobbyPosition = "1000 10 1000";
        LobbyRadius = 50.0;
        StartingMoney = 800;
        PresetBudgetLimit = 800;
        MaxPresets = 5;
        FreezeTimeSeconds = 15;
        RoundsToWin = 5;
        OvertimeThreshold = 4;
        OvertimeWinMargin = 2;
        
        if (!Rewards)
        {
            Rewards = new SwarmArenaRewardsConfig();
        }
        Rewards.Kill = 100;
        Rewards.RoundWin = 200;
        Rewards.MatchWin = 500;
        Rewards.MatchLose = -100;
    }
    
    bool Save()
    {
        // Ensure directory exists
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
        SwarmArenaCore.Log("Config saved to " + CONFIG_PATH);
        return true;
    }
}

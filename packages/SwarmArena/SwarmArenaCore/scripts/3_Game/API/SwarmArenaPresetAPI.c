// SwarmArenaPresetAPI - REST API integration for player presets
// Uses SwarmFramework Http module for HTTP requests

class SwarmArenaPresetAPIConfig
{
    private static ref SwarmArenaPresetAPIConfig s_Instance;
    private static const string CONFIG_PATH = "$profile:SwarmArena/api.json";
    
    string ApiBaseUrl = "";  // e.g., "https://api.example.com"
    string ApiKey = "";      // Optional API key for authentication
    
    static SwarmArenaPresetAPIConfig GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaPresetAPIConfig();
        }
        return s_Instance;
    }
    
    static SwarmArenaPresetAPIConfig Get()
    {
        return GetInstance();
    }
    
    static bool Load()
    {
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
                    SwarmArenaCore.LogError("API config parse error: " + error);
                }
            }
            SwarmArenaCore.Log("API config loaded from " + CONFIG_PATH);
            return true;
        }
        else
        {
            SwarmArenaCore.Log("API config not found, creating template at " + CONFIG_PATH);
            GetInstance().Save();
            return true;
        }
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
        return true;
    }
    
    bool IsConfigured()
    {
        return ApiBaseUrl != "";
    }
}

// Callback for GET presets request
class SwarmArenaPresetGetCallback extends RestCallback
{
    protected string m_SteamId;
    protected ref SwarmArenaPlayerData m_PlayerData;
    
    void SwarmArenaPresetGetCallback(string steamId, SwarmArenaPlayerData playerData)
    {
        m_SteamId = steamId;
        m_PlayerData = playerData;
    }
    
    override void OnSuccess(string data, int dataSize)
    {
        SwarmArenaCore.Log("Presets loaded for player " + m_SteamId);
        
        // Parse JSON response and populate player presets
        if (m_PlayerData)
        {
            SwarmArenaPresetAPI.ParsePresetsResponse(data, m_PlayerData);
        }
    }
    
    override void OnError(int errorCode)
    {
        SwarmArenaCore.LogError("Failed to load presets for " + m_SteamId + " (error: " + errorCode.ToString() + ")");
    }
    
    override void OnTimeout()
    {
        SwarmArenaCore.LogError("Preset loading timed out for " + m_SteamId);
    }
}

// Callback for PUT presets request
class SwarmArenaPresetSaveCallback extends RestCallback
{
    protected string m_SteamId;
    
    void SwarmArenaPresetSaveCallback(string steamId)
    {
        m_SteamId = steamId;
    }
    
    override void OnSuccess(string data, int dataSize)
    {
        SwarmArenaCore.Log("Presets saved for player " + m_SteamId);
    }
    
    override void OnError(int errorCode)
    {
        SwarmArenaCore.LogError("Failed to save presets for " + m_SteamId + " (error: " + errorCode.ToString() + ")");
    }
    
    override void OnTimeout()
    {
        SwarmArenaCore.LogError("Preset saving timed out for " + m_SteamId);
    }
}

class SwarmArenaPresetAPI
{
    // Fetch presets from API
    static bool GetPresets(string steamId, SwarmArenaPlayerData playerData)
    {
        SwarmArenaPresetAPIConfig config = SwarmArenaPresetAPIConfig.Get();
        
        if (!config.IsConfigured())
        {
            SwarmArenaCore.LogWarning("API not configured, skipping preset fetch");
            return false;
        }
        
        string url = config.ApiBaseUrl + "/players/" + steamId + "/presets";
        SwarmArenaPresetGetCallback callback = new SwarmArenaPresetGetCallback(steamId, playerData);
        
        return Http.Get(url, callback);
    }
    
    // Save presets to API
    static bool SavePresets(string steamId, array<ref SwarmArenaPreset> presets)
    {
        SwarmArenaPresetAPIConfig config = SwarmArenaPresetAPIConfig.Get();
        
        if (!config.IsConfigured())
        {
            SwarmArenaCore.LogWarning("API not configured, skipping preset save");
            return false;
        }
        
        string url = config.ApiBaseUrl + "/players/" + steamId + "/presets";
        string json = BuildPresetsJson(presets);
        SwarmArenaPresetSaveCallback callback = new SwarmArenaPresetSaveCallback(steamId);
        
        return Http.PostJson(url, json, callback);
    }
    
    // Build JSON payload for presets
    static string BuildPresetsJson(array<ref SwarmArenaPreset> presets)
    {
        JsonBuilder builder = new JsonBuilder();
        builder.BeginObject();
        builder.AddArray("presets");
        
        foreach (SwarmArenaPreset preset : presets)
        {
            builder.BeginObject();
            builder.AddString("name", preset.Name);
            builder.AddArray("items");
            
            foreach (SwarmArenaPresetItem item : preset.Items)
            {
                builder.BeginObject();
                builder.AddString("classname", item.Classname);
                builder.AddArray("attachments");
                
                foreach (string attachment : item.Attachments)
                {
                    builder.ItemString(attachment);
                }
                
                builder.EndArray();
                builder.EndObject();
            }
            
            builder.EndArray();
            builder.EndObject();
        }
        
        builder.EndArray();
        builder.EndObject();
        
        return builder.Build();
    }
    
    // Parse JSON response into player presets
    static void ParsePresetsResponse(string json, SwarmArenaPlayerData playerData)
    {
        // Clear existing presets
        playerData.Presets.Clear();
        
        // Parse using JsonNode
        JsonNode root = Json.Parse(json);
        if (!root)
        {
            SwarmArenaCore.LogError("Failed to parse presets JSON");
            return;
        }
        
        JsonNode presetsNode = root.Get("presets");
        if (!presetsNode)
        {
            SwarmArenaCore.LogWarning("No presets array in response");
            return;
        }
        
        // Iterate presets array
        for (int i = 0; i < presetsNode.Count(); i++)
        {
            JsonNode presetNode = presetsNode.GetAt(i);
            if (!presetNode)
            {
                continue;
            }
            
            SwarmArenaPreset preset = new SwarmArenaPreset();
            JsonNode nameNode = presetNode.Get("name");
            if (nameNode)
            {
                preset.Name = nameNode.GetString();
            }
            
            JsonNode itemsNode = presetNode.Get("items");
            if (itemsNode)
            {
                for (int j = 0; j < itemsNode.Count(); j++)
                {
                    JsonNode itemNode = itemsNode.GetAt(j);
                    if (!itemNode)
                    {
                        continue;
                    }
                    
                    SwarmArenaPresetItem item = new SwarmArenaPresetItem();
                    JsonNode classnameNode = itemNode.Get("classname");
                    if (classnameNode)
                    {
                        item.Classname = classnameNode.GetString();
                    }
                    
                    JsonNode attachmentsNode = itemNode.Get("attachments");
                    if (attachmentsNode)
                    {
                        for (int k = 0; k < attachmentsNode.Count(); k++)
                        {
                            JsonNode attachNode = attachmentsNode.GetAt(k);
                            if (attachNode)
                            {
                                item.Attachments.Insert(attachNode.GetString());
                            }
                        }
                    }
                    
                    preset.Items.Insert(item);
                }
            }
            
            playerData.Presets.Insert(preset);
        }
        
        SwarmArenaCore.Log("Parsed " + playerData.Presets.Count().ToString() + " presets from API");
    }
}

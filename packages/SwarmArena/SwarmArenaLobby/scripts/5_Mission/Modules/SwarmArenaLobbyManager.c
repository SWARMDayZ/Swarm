// SwarmArenaLobbyManager - Manages lobby area and player lobby state

class SwarmArenaLobbyManager
{
    private static ref SwarmArenaLobbyManager s_Instance;
    
    // Players currently in lobby
    protected ref map<string, ref SwarmArenaPlayerData> m_LobbyPlayers = new map<string, ref SwarmArenaPlayerData>();
    
    static SwarmArenaLobbyManager GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaLobbyManager();
        }
        return s_Instance;
    }
    
    static SwarmArenaLobbyManager Get()
    {
        return GetInstance();
    }
    
    void SwarmArenaLobbyManager()
    {
        SwarmArenaCore.Log("LobbyManager initialized");
    }
    
    // Called when a player connects or returns from match
    void EnterLobby(PlayerBase player)
    {
        if (!player || !player.GetIdentity())
        {
            return;
        }
        
        string steamId = player.GetIdentity().GetPlainId();
        string playerName = player.GetIdentity().GetName();
        
        // Get or create player data
        SwarmArenaPlayerData playerData = GetOrCreatePlayerData(steamId, playerName);
        playerData.IsInLobby = true;
        playerData.ResetMatchState();
        
        // Strip all gear
        StripPlayerGear(player);
        
        // Teleport to lobby
        TeleportToLobby(player);
        
        // Enable god mode in lobby
        SetPlayerGodMode(player, true);
        
        // Load presets from API
        SwarmArenaPresetAPI.GetPresets(steamId, playerData);
        
        SwarmArenaCore.Log("Player " + playerName + " entered lobby");
    }
    
    // Called when a player is matched and joining a match
    void LeaveLobby(string steamId)
    {
        if (m_LobbyPlayers.Contains(steamId))
        {
            SwarmArenaPlayerData playerData = m_LobbyPlayers.Get(steamId);
            if (playerData)
            {
                playerData.IsInLobby = false;
            }
        }
    }
    
    // Get or create player data
    SwarmArenaPlayerData GetOrCreatePlayerData(string steamId, string playerName)
    {
        if (m_LobbyPlayers.Contains(steamId))
        {
            return m_LobbyPlayers.Get(steamId);
        }
        
        SwarmArenaPlayerData playerData = new SwarmArenaPlayerData();
        playerData.Initialize(steamId, playerName);
        m_LobbyPlayers.Insert(steamId, playerData);
        
        return playerData;
    }
    
    SwarmArenaPlayerData GetPlayerData(string steamId)
    {
        if (m_LobbyPlayers.Contains(steamId))
        {
            return m_LobbyPlayers.Get(steamId);
        }
        return null;
    }
    
    // Remove player data (on disconnect)
    void RemovePlayer(string steamId)
    {
        if (m_LobbyPlayers.Contains(steamId))
        {
            m_LobbyPlayers.Remove(steamId);
        }
        
        // Also clean up from queue and party
        SwarmArenaMatchmaker.Get().RemoveFromAllQueues(steamId);
        SwarmArenaPartyManager.Get().OnPlayerDisconnect(steamId);
    }
    
    // Strip all player gear
    void StripPlayerGear(PlayerBase player)
    {
        if (!player)
        {
            return;
        }
        
        // Get all inventory items
        array<EntityAI> items = new array<EntityAI>();
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
        
        foreach (EntityAI entity : items)
        {
            if (entity == player)
            {
                continue;
            }
            
            ItemBase item;
            if (Class.CastTo(item, entity))
            {
                item.Delete();
            }
        }
    }
    
    // Teleport player to lobby position
    void TeleportToLobby(PlayerBase player)
    {
        if (!player)
        {
            return;
        }
        
        vector lobbyPos = SwarmArenaConfig.Get().LobbyPosition;
        player.SetPosition(lobbyPos);
    }
    
    // Enable/disable god mode
    void SetPlayerGodMode(PlayerBase player, bool enable)
    {
        if (!player)
        {
            return;
        }
        
        // Using COT-style approach - setting health to max constantly
        // Or implementing via damage handler override
        // For now, we'll store the state and handle in damage events
        
        // TODO: Implement via modded HandleDamage or similar
    }
    
    // Check if a position is within lobby radius
    bool IsInLobbyZone(vector position)
    {
        vector lobbyPos = SwarmArenaConfig.Get().LobbyPosition;
        float radius = SwarmArenaConfig.Get().LobbyRadius;
        
        return vector.Distance(position, lobbyPos) <= radius;
    }
    
    // Get all players in lobby
    array<string> GetLobbyPlayerIds()
    {
        array<string> playerIds = new array<string>();
        
        for (int i = 0; i < m_LobbyPlayers.Count(); i++)
        {
            string steamId = m_LobbyPlayers.GetKey(i);
            SwarmArenaPlayerData data = m_LobbyPlayers.GetElement(i);
            
            if (data && data.IsInLobby)
            {
                playerIds.Insert(steamId);
            }
        }
        
        return playerIds;
    }
    
    // Get online players not in match (for party invites)
    void GetAvailablePlayers(string excludeSteamId, out array<string> steamIds, out array<string> names)
    {
        steamIds = new array<string>();
        names = new array<string>();
        
        for (int i = 0; i < m_LobbyPlayers.Count(); i++)
        {
            string steamId = m_LobbyPlayers.GetKey(i);
            SwarmArenaPlayerData data = m_LobbyPlayers.GetElement(i);
            
            if (steamId != excludeSteamId && data && data.IsInLobby && !data.IsInMatch)
            {
                steamIds.Insert(steamId);
                names.Insert(data.PlayerName);
            }
        }
    }
}

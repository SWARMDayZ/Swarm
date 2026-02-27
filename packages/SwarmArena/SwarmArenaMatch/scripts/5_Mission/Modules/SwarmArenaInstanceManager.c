// SwarmArenaInstanceManager - Manages multiple concurrent matches

class SwarmArenaInstanceManager
{
    private static ref SwarmArenaInstanceManager s_Instance;
    
    // Active matches
    protected ref array<ref SwarmArenaMatch> m_ActiveMatches = new array<ref SwarmArenaMatch>();
    
    // Player to match mapping
    protected ref map<string, int> m_PlayerToMatchId = new map<string, int>();
    
    // Maps currently in use (to avoid duplicates when possible)
    protected ref array<string> m_MapsInUse = new array<string>();
    
    // Counter for unique match IDs
    protected int m_NextMatchId = 1;
    
    static SwarmArenaInstanceManager GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaInstanceManager();
        }
        return s_Instance;
    }
    
    static SwarmArenaInstanceManager Get()
    {
        return GetInstance();
    }
    
    void SwarmArenaInstanceManager()
    {
        SwarmArenaCore.Log("InstanceManager initialized");
    }
    
    // Create a new match
    SwarmArenaMatch CreateMatch(array<string> team1SteamIds, array<string> team2SteamIds)
    {
        int matchId = m_NextMatchId++;
        
        SwarmArenaMatch match = new SwarmArenaMatch(matchId);
        match.Initialize(team1SteamIds, team2SteamIds);
        
        if (match.CurrentMap)
        {
            m_MapsInUse.Insert(match.CurrentMap.Name);
        }
        
        m_ActiveMatches.Insert(match);
        
        // Map players to match
        foreach (string steamId : team1SteamIds)
        {
            m_PlayerToMatchId.Insert(steamId, matchId);
        }
        
        foreach (string steamId2 : team2SteamIds)
        {
            m_PlayerToMatchId.Insert(steamId2, matchId);
        }
        
        // Start the match
        match.Start();
        
        SwarmArenaCore.Log("Match " + matchId.ToString() + " created");
        
        return match;
    }
    
    // Get match by ID
    SwarmArenaMatch GetMatch(int matchId)
    {
        foreach (SwarmArenaMatch match : m_ActiveMatches)
        {
            if (match.MatchId == matchId)
            {
                return match;
            }
        }
        return null;
    }
    
    // Get match by player steam ID
    SwarmArenaMatch GetPlayerMatch(string steamId)
    {
        if (!m_PlayerToMatchId.Contains(steamId))
        {
            return null;
        }
        
        int matchId = m_PlayerToMatchId.Get(steamId);
        return GetMatch(matchId);
    }
    
    // Check if player is in a match
    bool IsPlayerInMatch(string steamId)
    {
        return m_PlayerToMatchId.Contains(steamId);
    }
    
    // End and cleanup a match
    void EndMatch(int matchId)
    {
        SwarmArenaMatch match = GetMatch(matchId);
        if (!match)
        {
            return;
        }
        
        // Remove map from in-use list
        if (match.CurrentMap)
        {
            int mapIndex = m_MapsInUse.Find(match.CurrentMap.Name);
            if (mapIndex >= 0)
            {
                m_MapsInUse.Remove(mapIndex);
            }
        }
        
        // Remove player mappings
        foreach (string steamId : match.Team1.PlayerSteamIds)
        {
            m_PlayerToMatchId.Remove(steamId);
        }
        
        foreach (string steamId2 : match.Team2.PlayerSteamIds)
        {
            m_PlayerToMatchId.Remove(steamId2);
        }
        
        // Remove match from list
        int matchIndex = m_ActiveMatches.Find(match);
        if (matchIndex >= 0)
        {
            m_ActiveMatches.Remove(matchIndex);
        }
        
        SwarmArenaCore.Log("Match " + matchId.ToString() + " ended and cleaned up");
    }
    
    // Handle player disconnect during match
    void OnPlayerDisconnect(string steamId)
    {
        SwarmArenaMatch match = GetPlayerMatch(steamId);
        if (!match)
        {
            return;
        }
        
        // Treat disconnect as death
        match.OnPlayerDeath(steamId, "");
        
        SwarmArenaCore.Log("Player " + steamId + " disconnected from match " + match.MatchId.ToString());
    }
    
    // Update all active matches
    void Update(float timeslice)
    {
        // Update matches in reverse order to safely remove ended ones
        for (int i = m_ActiveMatches.Count() - 1; i >= 0; i--)
        {
            SwarmArenaMatch match = m_ActiveMatches[i];
            
            if (match.State == SwarmArenaMatchState.MATCH_END)
            {
                EndMatch(match.MatchId);
            }
            else
            {
                match.Update(timeslice);
            }
        }
    }
    
    // Get count of active matches
    int GetActiveMatchCount()
    {
        return m_ActiveMatches.Count();
    }
    
    // Get list of maps currently in use
    array<string> GetMapsInUse()
    {
        return m_MapsInUse;
    }
    
    // Check if a specific map is in use
    bool IsMapInUse(string mapName)
    {
        return m_MapsInUse.Find(mapName) >= 0;
    }
}

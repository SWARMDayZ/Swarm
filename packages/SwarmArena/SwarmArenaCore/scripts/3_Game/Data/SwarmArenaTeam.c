// SwarmArenaTeam - Team data for a match

class SwarmArenaTeam
{
    int TeamIndex;  // 0 or 1
    ref array<string> PlayerSteamIds = new array<string>();
    ref array<string> PlayerNames = new array<string>();
    ref array<string> AliveSteamIds = new array<string>();
    ref array<string> DeadSteamIds = new array<string>();
    int Score;
    
    void SwarmArenaTeam(int index)
    {
        TeamIndex = index;
        Score = 0;
    }
    
    void AddPlayer(string steamId, string name)
    {
        PlayerSteamIds.Insert(steamId);
        PlayerNames.Insert(name);
        AliveSteamIds.Insert(steamId);
    }
    
    bool HasPlayer(string steamId)
    {
        return PlayerSteamIds.Find(steamId) >= 0;
    }
    
    bool IsAlive(string steamId)
    {
        return AliveSteamIds.Find(steamId) >= 0;
    }
    
    bool IsDead(string steamId)
    {
        return DeadSteamIds.Find(steamId) >= 0;
    }
    
    void MarkDead(string steamId)
    {
        int aliveIndex = AliveSteamIds.Find(steamId);
        if (aliveIndex >= 0)
        {
            AliveSteamIds.Remove(aliveIndex);
            DeadSteamIds.Insert(steamId);
        }
    }
    
    void ResetAliveStatus()
    {
        AliveSteamIds.Clear();
        DeadSteamIds.Clear();
        
        foreach (string steamId : PlayerSteamIds)
        {
            AliveSteamIds.Insert(steamId);
        }
    }
    
    int GetAliveCount()
    {
        return AliveSteamIds.Count();
    }
    
    int GetDeadCount()
    {
        return DeadSteamIds.Count();
    }
    
    int GetPlayerCount()
    {
        return PlayerSteamIds.Count();
    }
    
    bool IsEliminated()
    {
        return AliveSteamIds.Count() == 0;
    }
    
    void IncrementScore()
    {
        Score++;
    }
    
    string GetPlayerName(string steamId)
    {
        int index = PlayerSteamIds.Find(steamId);
        if (index >= 0 && index < PlayerNames.Count())
        {
            return PlayerNames[index];
        }
        return "";
    }
    
    // Get alive teammates for spectating (excluding self)
    void GetAliveTeammates(string excludeSteamId, out array<string> teammates)
    {
        teammates = new array<string>();
        
        foreach (string steamId : AliveSteamIds)
        {
            if (steamId != excludeSteamId)
            {
                teammates.Insert(steamId);
            }
        }
    }
}

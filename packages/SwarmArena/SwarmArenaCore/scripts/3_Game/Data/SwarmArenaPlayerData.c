// SwarmArenaPlayerData - Player runtime data during arena sessions

class SwarmArenaPlayerData
{
    string SteamId;
    string PlayerName;
    int Money;
    ref array<ref SwarmArenaPreset> Presets = new array<ref SwarmArenaPreset>();
    
    // Match state
    bool IsInMatch;
    int MatchId;
    int TeamIndex;
    bool IsAlive;
    bool IsSpectating;
    string SpectatingTarget;
    
    // Lobby state
    bool IsInLobby;
    bool IsInQueue;
    SwarmArenaQueueType QueueType;
    string PartyOwnerId;  // Empty if not in party
    
    // Stats (session only, not persisted here)
    int SessionKills;
    int SessionDeaths;
    int SessionWins;
    int SessionLosses;
    
    void SwarmArenaPlayerData()
    {
        SteamId = "";
        PlayerName = "";
        Money = 0;
        IsInMatch = false;
        MatchId = -1;
        TeamIndex = -1;
        IsAlive = true;
        IsSpectating = false;
        SpectatingTarget = "";
        IsInLobby = false;
        IsInQueue = false;
        QueueType = SwarmArenaQueueType.INVALID;
        PartyOwnerId = "";
        SessionKills = 0;
        SessionDeaths = 0;
        SessionWins = 0;
        SessionLosses = 0;
    }
    
    void Initialize(string steamId, string playerName)
    {
        SteamId = steamId;
        PlayerName = playerName;
        Money = SwarmArenaConfig.Get().StartingMoney;
        ResetMatchState();
    }
    
    void ResetMatchState()
    {
        IsInMatch = false;
        MatchId = -1;
        TeamIndex = -1;
        IsAlive = true;
        IsSpectating = false;
        SpectatingTarget = "";
    }
    
    void EnterMatch(int matchId, int teamIndex)
    {
        IsInMatch = true;
        MatchId = matchId;
        TeamIndex = teamIndex;
        IsAlive = true;
        IsSpectating = false;
        SpectatingTarget = "";
        IsInLobby = false;
        IsInQueue = false;
        QueueType = SwarmArenaQueueType.INVALID;
    }
    
    void ExitMatch()
    {
        ResetMatchState();
        IsInLobby = true;
    }
    
    void OnDeath()
    {
        IsAlive = false;
        SessionDeaths++;
    }
    
    void OnKill()
    {
        SessionKills++;
        AddMoney(SwarmArenaConfig.Get().Rewards.Kill);
    }
    
    void OnRoundWin()
    {
        AddMoney(SwarmArenaConfig.Get().Rewards.RoundWin);
    }
    
    void OnMatchWin()
    {
        SessionWins++;
        AddMoney(SwarmArenaConfig.Get().Rewards.MatchWin);
    }
    
    void OnMatchLose()
    {
        SessionLosses++;
        AddMoney(SwarmArenaConfig.Get().Rewards.MatchLose);
    }
    
    void AddMoney(int amount)
    {
        Money += amount;
        if (Money < 0)
        {
            Money = 0;
        }
    }
    
    bool SpendMoney(int amount)
    {
        if (Money >= amount)
        {
            Money -= amount;
            return true;
        }
        return false;
    }
    
    bool CanAfford(int amount)
    {
        return Money >= amount;
    }
    
    void StartSpectating(string targetSteamId)
    {
        IsSpectating = true;
        SpectatingTarget = targetSteamId;
    }
    
    void StopSpectating()
    {
        IsSpectating = false;
        SpectatingTarget = "";
    }
    
    void SetParty(string partyOwnerId)
    {
        PartyOwnerId = partyOwnerId;
    }
    
    void LeaveParty()
    {
        PartyOwnerId = "";
    }
    
    bool IsInParty()
    {
        return PartyOwnerId != "";
    }
    
    bool IsPartyOwner()
    {
        return PartyOwnerId == SteamId;
    }
    
    // Preset management
    void AddPreset(SwarmArenaPreset preset)
    {
        if (Presets.Count() < SwarmArenaConfig.Get().MaxPresets)
        {
            Presets.Insert(preset);
        }
    }
    
    void RemovePreset(int index)
    {
        if (index >= 0 && index < Presets.Count())
        {
            Presets.Remove(index);
        }
    }
    
    SwarmArenaPreset GetPreset(int index)
    {
        if (index >= 0 && index < Presets.Count())
        {
            return Presets[index];
        }
        return null;
    }
    
    int GetPresetCount()
    {
        return Presets.Count();
    }
}

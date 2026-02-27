// SwarmArenaMatch - Single match controller with round system

class SwarmArenaMatch
{
    int MatchId;
    ref SwarmArenaMapData CurrentMap;
    ref SwarmArenaTeam Team1;
    ref SwarmArenaTeam Team2;
    SwarmArenaMatchState State;
    
    int CurrentRound;
    float FreezeTimer;
    float RoundTimer;
    
    // Player money tracking
    protected ref map<string, int> m_PlayerMoney = new map<string, int>();
    
    void SwarmArenaMatch(int matchId)
    {
        MatchId = matchId;
        Team1 = new SwarmArenaTeam(0);
        Team2 = new SwarmArenaTeam(1);
        State = SwarmArenaMatchState.INVALID;
        CurrentRound = 0;
        FreezeTimer = 0;
        RoundTimer = 0;
    }
    
    // Initialize match with teams
    void Initialize(array<string> team1SteamIds, array<string> team2SteamIds)
    {
        State = SwarmArenaMatchState.INITIALIZING;
        
        // Select random map
        CurrentMap = SwarmArenaMapsConfig.Get().GetRandomMap();
        if (!CurrentMap)
        {
            SwarmArenaCore.LogError("No map available for match " + MatchId.ToString());
            return;
        }
        
        // Populate teams
        foreach (string steamId : team1SteamIds)
        {
            SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
            if (playerData)
            {
                Team1.AddPlayer(steamId, playerData.PlayerName);
                playerData.EnterMatch(MatchId, 0);
            }
        }
        
        foreach (string steamId2 : team2SteamIds)
        {
            SwarmArenaPlayerData playerData2 = SwarmArenaLobbyManager.Get().GetPlayerData(steamId2);
            if (playerData2)
            {
                Team2.AddPlayer(steamId2, playerData2.PlayerName);
                playerData2.EnterMatch(MatchId, 1);
            }
        }
        
        string initLog = "Match " + MatchId.ToString() + " initialized on map " + CurrentMap.Name + " - Team1: " + Team1.GetPlayerCount().ToString() + " vs Team2: " + Team2.GetPlayerCount().ToString();
        SwarmArenaCore.Log(initLog);
    }
    
    // Start the match
    void Start()
    {
        // Give starting money to all players
        int startingMoney = SwarmArenaConfig.Get().StartingMoney;
        
        foreach (string steamId : Team1.PlayerSteamIds)
        {
            m_PlayerMoney.Insert(steamId, startingMoney);
            SendMoneyUpdate(steamId);
        }
        
        foreach (string steamId2 : Team2.PlayerSteamIds)
        {
            m_PlayerMoney.Insert(steamId2, startingMoney);
            SendMoneyUpdate(steamId2);
        }
        
        // Send match found notification to all players
        NotifyMatchStart();
        
        // Start first round
        StartRound();
    }
    
    // Start a new round
    void StartRound()
    {
        CurrentRound++;
        State = SwarmArenaMatchState.FREEZE_TIME;
        FreezeTimer = SwarmArenaConfig.Get().FreezeTimeSeconds;
        
        SwarmArenaCore.Log("Match " + MatchId.ToString() + " - Round " + CurrentRound.ToString() + " starting");
        
        // Reset alive status
        Team1.ResetAliveStatus();
        Team2.ResetAliveStatus();
        
        // Teleport players to spawn points
        TeleportTeamToSpawns(Team1, true);
        TeleportTeamToSpawns(Team2, false);
        
        // Strip weapons (keep base clothes)
        StripAllPlayersWeapons();
        
        // Freeze all players
        SetAllPlayersFrozen(true);
        
        // Notify clients
        NotifyRoundStart();
    }
    
    // Teleport team to their spawn points
    protected void TeleportTeamToSpawns(SwarmArenaTeam team, bool isTeam1)
    {
        array<vector> spawns;
        if (isTeam1)
        {
            spawns = CurrentMap.Team1Spawns;
        }
        else
        {
            spawns = CurrentMap.Team2Spawns;
        }
        
        for (int i = 0; i < team.PlayerSteamIds.Count(); i++)
        {
            string steamId = team.PlayerSteamIds[i];
            PlayerBase player = GetPlayerBySteamId(steamId);
            
            if (player && i < spawns.Count())
            {
                player.SetPosition(spawns[i]);
            }
        }
    }
    
    // Strip weapons from all players (keep base clothes)
    protected void StripAllPlayersWeapons()
    {
        foreach (string steamId : Team1.PlayerSteamIds)
        {
            PlayerBase player = GetPlayerBySteamId(steamId);
            if (player)
            {
                SwarmArenaGearManager.StripWeapons(player);
            }
        }
        
        foreach (string steamId2 : Team2.PlayerSteamIds)
        {
            PlayerBase player2 = GetPlayerBySteamId(steamId2);
            if (player2)
            {
                SwarmArenaGearManager.StripWeapons(player2);
            }
        }
    }
    
    // Set frozen state for all players
    protected void SetAllPlayersFrozen(bool frozen)
    {
        foreach (string steamId : Team1.PlayerSteamIds)
        {
            PlayerBase player = GetPlayerBySteamId(steamId);
            if (player)
            {
                // TODO: Implement freeze via player state
            }
        }
        
        foreach (string steamId2 : Team2.PlayerSteamIds)
        {
            PlayerBase player2 = GetPlayerBySteamId(steamId2);
            if (player2)
            {
                // TODO: Implement freeze via player state
            }
        }
    }
    
    // Update called every frame
    void Update(float timeslice)
    {
        switch (State)
        {
            case SwarmArenaMatchState.FREEZE_TIME:
                UpdateFreezeTime(timeslice);
                break;
            case SwarmArenaMatchState.ROUND_ACTIVE:
                UpdateRoundActive(timeslice);
                break;
            case SwarmArenaMatchState.ROUND_END:
                // Brief pause before next round
                break;
        }
    }
    
    protected void UpdateFreezeTime(float timeslice)
    {
        FreezeTimer -= timeslice;
        
        if (FreezeTimer <= 0)
        {
            OnFreezeTimeEnd();
        }
    }
    
    protected void OnFreezeTimeEnd()
    {
        State = SwarmArenaMatchState.ROUND_ACTIVE;
        
        // Unfreeze all players
        SetAllPlayersFrozen(false);
        
        // Notify clients that freeze ended
        NotifyFreezeEnd();
        
        SwarmArenaCore.Log("Match " + MatchId.ToString() + " - Freeze time ended, round active");
    }
    
    protected void UpdateRoundActive(float timeslice)
    {
        RoundTimer += timeslice;
        
        // Check if either team is eliminated
        if (Team1.IsEliminated())
        {
            OnRoundEnd(Team2); // Team 2 wins
        }
        else if (Team2.IsEliminated())
        {
            OnRoundEnd(Team1); // Team 1 wins
        }
    }
    
    // Called when a player "dies" in match
    void OnPlayerDeath(string steamId, string killerSteamId)
    {
        if (State != SwarmArenaMatchState.ROUND_ACTIVE)
        {
            return;
        }
        
        // Find which team the player is on
        SwarmArenaTeam playerTeam;
        SwarmArenaTeam enemyTeam;
        
        if (Team1.HasPlayer(steamId))
        {
            playerTeam = Team1;
            enemyTeam = Team2;
        }
        else if (Team2.HasPlayer(steamId))
        {
            playerTeam = Team2;
            enemyTeam = Team1;
        }
        else
        {
            return;
        }
        
        // Mark as dead
        playerTeam.MarkDead(steamId);
        
        // Award kill money
        if (killerSteamId != "" && killerSteamId != steamId)
        {
            AwardMoney(killerSteamId, SwarmArenaConfig.Get().Rewards.Kill);
        }
        
        // Update player data
        SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
        if (playerData)
        {
            playerData.OnDeath();
        }
        
        SwarmArenaPlayerData killerData = SwarmArenaLobbyManager.Get().GetPlayerData(killerSteamId);
        if (killerData)
        {
            killerData.OnKill();
        }
        
        // Get player entity
        PlayerBase player = GetPlayerBySteamId(steamId);
        if (player)
        {
            // Heal to prevent actual death
            player.SetHealth("", "", 100);
            
            // Remove bleeding
            BleedingSourcesManagerServer bleedMgr = player.GetBleedingManagerServer();
            if (bleedMgr)
            {
                bleedMgr.RemoveAllSources();
            }
            
            // Strip weapons
            SwarmArenaGearManager.StripWeapons(player);
            
            // Teleport to spectator position
            player.SetPosition(CurrentMap.SpectatorPosition);
            
            // Get alive teammates for spectating
            array<string> aliveTeammates;
            playerTeam.GetAliveTeammates(steamId, aliveTeammates);
            
            // Enter spectator mode
            if (aliveTeammates.Count() > 0)
            {
                // Notify client to enter spectator
                NotifyPlayerEnterSpectator(steamId, aliveTeammates);
            }
        }
        
        SwarmArenaCore.Log("Match " + MatchId.ToString() + " - Player died. Team alive: " + playerTeam.GetAliveCount().ToString());
        
        // Check team elimination
        if (playerTeam.IsEliminated())
        {
            OnRoundEnd(enemyTeam);
        }
    }
    
    // Round ends with winning team
    protected void OnRoundEnd(SwarmArenaTeam winningTeam)
    {
        State = SwarmArenaMatchState.ROUND_END;
        
        winningTeam.IncrementScore();
        
        // Award round win money to winning team
        foreach (string steamId : winningTeam.PlayerSteamIds)
        {
            AwardMoney(steamId, SwarmArenaConfig.Get().Rewards.RoundWin);
            
            SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
            if (playerData)
            {
                playerData.OnRoundWin();
            }
        }
        
        string roundLog = "Match " + MatchId.ToString() + " - Round " + CurrentRound.ToString() + " ended. Score: " + Team1.Score.ToString() + " - " + Team2.Score.ToString();
        SwarmArenaCore.Log(roundLog);
        
        // Notify all players
        NotifyRoundEnd(winningTeam.TeamIndex);
        
        // Check match end condition
        if (CheckMatchEnd())
        {
            OnMatchEnd();
        }
        else
        {
            // Exit spectator for all dead players
            ExitAllSpectators();
            
            // Start next round after brief delay (handled externally)
            // For now, start immediately
            StartRound();
        }
    }
    
    // Check if match should end
    protected bool CheckMatchEnd()
    {
        int t1 = Team1.Score;
        int t2 = Team2.Score;
        int roundsToWin = SwarmArenaConfig.Get().RoundsToWin;
        int overtimeThreshold = SwarmArenaConfig.Get().OvertimeThreshold;
        int overtimeMargin = SwarmArenaConfig.Get().OvertimeWinMargin;
        
        // Normal win condition: first to roundsToWin
        if (t1 >= roundsToWin && t2 < overtimeThreshold)
        {
            return true;
        }
        if (t2 >= roundsToWin && t1 < overtimeThreshold)
        {
            return true;
        }
        
        // Overtime condition: both at threshold, need margin to win
        if (t1 >= overtimeThreshold && t2 >= overtimeThreshold)
        {
            if (Math.AbsInt(t1 - t2) >= overtimeMargin)
            {
                return true;
            }
        }
        
        return false;
    }
    
    // Match ends
    protected void OnMatchEnd()
    {
        State = SwarmArenaMatchState.MATCH_END;
        
        // Determine winner
        SwarmArenaTeam winner;
        SwarmArenaTeam loser;
        
        if (Team1.Score > Team2.Score)
        {
            winner = Team1;
            loser = Team2;
        }
        else
        {
            winner = Team2;
            loser = Team1;
        }
        
        // Award match win/lose money
        foreach (string steamId : winner.PlayerSteamIds)
        {
            AwardMoney(steamId, SwarmArenaConfig.Get().Rewards.MatchWin);
            
            SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
            if (playerData)
            {
                playerData.OnMatchWin();
            }
        }
        
        foreach (string steamId2 : loser.PlayerSteamIds)
        {
            AwardMoney(steamId2, SwarmArenaConfig.Get().Rewards.MatchLose);
            
            SwarmArenaPlayerData playerData2 = SwarmArenaLobbyManager.Get().GetPlayerData(steamId2);
            if (playerData2)
            {
                playerData2.OnMatchLose();
            }
        }
        
        string endLog = "Match " + MatchId.ToString() + " - ENDED. Final score: " + Team1.Score.ToString() + " - " + Team2.Score.ToString();
        SwarmArenaCore.Log(endLog);
        
        // Notify all players
        NotifyMatchEnd(winner.TeamIndex);
        
        // Exit all spectators
        ExitAllSpectators();
        
        // Return all players to lobby
        ReturnAllToLobby();
    }
    
    // Exit spectator mode for all dead players
    protected void ExitAllSpectators()
    {
        foreach (string steamId : Team1.DeadSteamIds)
        {
            NotifyPlayerExitSpectator(steamId);
        }
        
        foreach (string steamId2 : Team2.DeadSteamIds)
        {
            NotifyPlayerExitSpectator(steamId2);
        }
    }
    
    // Return all players to lobby
    protected void ReturnAllToLobby()
    {
        foreach (string steamId : Team1.PlayerSteamIds)
        {
            PlayerBase player = GetPlayerBySteamId(steamId);
            if (player)
            {
                SwarmArenaLobbyManager.Get().EnterLobby(player);
            }
            
            SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
            if (playerData)
            {
                playerData.ExitMatch();
            }
        }
        
        foreach (string steamId2 : Team2.PlayerSteamIds)
        {
            PlayerBase player2 = GetPlayerBySteamId(steamId2);
            if (player2)
            {
                SwarmArenaLobbyManager.Get().EnterLobby(player2);
            }
            
            SwarmArenaPlayerData playerData2 = SwarmArenaLobbyManager.Get().GetPlayerData(steamId2);
            if (playerData2)
            {
                playerData2.ExitMatch();
            }
        }
    }
    
    // --- Buy System ---
    
    bool BuyPreset(string steamId, int presetIndex)
    {
        if (State != SwarmArenaMatchState.FREEZE_TIME)
        {
            return false;
        }
        
        SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
        if (!playerData)
        {
            return false;
        }
        
        SwarmArenaPreset preset = playerData.GetPreset(presetIndex);
        if (!preset)
        {
            return false;
        }
        
        int cost = preset.CalculateTotalCost();
        int currentMoney = GetPlayerMoney(steamId);
        
        if (cost > currentMoney)
        {
            return false;
        }
        
        // Deduct money
        SpendMoney(steamId, cost);
        
        // Spawn items
        PlayerBase player = GetPlayerBySteamId(steamId);
        if (player)
        {
            SwarmArenaGearManager.SpawnPreset(player, preset);
        }
        
        // Send buy result
        SendBuyResult(steamId, SwarmArenaBuyResult.SUCCESS, cost);
        
        return true;
    }
    
    bool BuyArmor(string steamId, SwarmArenaArmorType armorType)
    {
        if (State != SwarmArenaMatchState.FREEZE_TIME)
        {
            SendBuyResult(steamId, SwarmArenaBuyResult.NOT_IN_FREEZE_TIME, 0);
            return false;
        }
        
        SwarmArenaArmorEntry armorEntry = SwarmArenaArmorConfig.Get().GetArmorByType(armorType);
        if (!armorEntry)
        {
            SendBuyResult(steamId, SwarmArenaBuyResult.INVALID_ARMOR, 0);
            return false;
        }
        
        int cost = armorEntry.Price;
        int currentMoney = GetPlayerMoney(steamId);
        
        if (cost > currentMoney)
        {
            SendBuyResult(steamId, SwarmArenaBuyResult.INSUFFICIENT_FUNDS, cost);
            return false;
        }
        
        // Deduct money
        SpendMoney(steamId, cost);
        
        // Spawn armor
        PlayerBase player = GetPlayerBySteamId(steamId);
        if (player)
        {
            SwarmArenaGearManager.SpawnArmor(player, armorEntry);
        }
        
        SendBuyResult(steamId, SwarmArenaBuyResult.SUCCESS, cost);
        return true;
    }
    
    // --- Money management ---
    
    int GetPlayerMoney(string steamId)
    {
        if (m_PlayerMoney.Contains(steamId))
        {
            return m_PlayerMoney.Get(steamId);
        }
        return 0;
    }
    
    void AwardMoney(string steamId, int amount)
    {
        if (m_PlayerMoney.Contains(steamId))
        {
            int current = m_PlayerMoney.Get(steamId);
            m_PlayerMoney.Set(steamId, current + amount);
        }
        else
        {
            m_PlayerMoney.Insert(steamId, amount);
        }
        
        SendMoneyUpdate(steamId);
    }
    
    void SpendMoney(string steamId, int amount)
    {
        if (m_PlayerMoney.Contains(steamId))
        {
            int current = m_PlayerMoney.Get(steamId);
            int newAmount = current - amount;
            if (newAmount < 0) newAmount = 0;
            m_PlayerMoney.Set(steamId, newAmount);
        }
        
        SendMoneyUpdate(steamId);
    }
    
    // --- Helper methods ---
    
    PlayerBase GetPlayerBySteamId(string steamId)
    {
        // Find player by steam ID
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        
        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player && player.GetIdentity())
            {
                if (player.GetIdentity().GetPlainId() == steamId)
                {
                    return player;
                }
            }
        }
        
        return null;
    }
    
    bool HasPlayer(string steamId)
    {
        return Team1.HasPlayer(steamId) || Team2.HasPlayer(steamId);
    }
    
    SwarmArenaTeam GetPlayerTeam(string steamId)
    {
        if (Team1.HasPlayer(steamId))
        {
            return Team1;
        }
        if (Team2.HasPlayer(steamId))
        {
            return Team2;
        }
        return null;
    }
    
    // --- RPC notifications ---
    
    protected void NotifyMatchStart()
    {
        // TODO: Send Match_Start RPC to all players
    }
    
    protected void NotifyRoundStart()
    {
        // TODO: Send Match_RoundStart RPC to all players
    }
    
    protected void NotifyFreezeEnd()
    {
        // TODO: Send Match_FreezeEnd RPC to all players
    }
    
    protected void NotifyRoundEnd(int winningTeamIndex)
    {
        // TODO: Send Match_RoundEnd RPC to all players
    }
    
    protected void NotifyMatchEnd(int winningTeamIndex)
    {
        // TODO: Send Match_End RPC to all players
    }
    
    protected void NotifyPlayerEnterSpectator(string steamId, array<string> targets)
    {
        // TODO: Send Spectator_Enter RPC to player
    }
    
    protected void NotifyPlayerExitSpectator(string steamId)
    {
        // TODO: Send Spectator_Exit RPC to player
    }
    
    protected void SendMoneyUpdate(string steamId)
    {
        // TODO: Send Match_MoneyUpdate RPC to player
    }
    
    protected void SendBuyResult(string steamId, SwarmArenaBuyResult result, int amount)
    {
        // TODO: Send Buy_Result RPC to player
    }
}

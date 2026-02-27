// SwarmArenaGUIRPCHandler - Client-side RPC handler for GUI updates

class SwarmArenaGUIRPCHandler
{
    protected static ref SwarmArenaGUIRPCHandler s_Instance;
    
    static SwarmArenaGUIRPCHandler GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaGUIRPCHandler();
        }
        return s_Instance;
    }
    
    static SwarmArenaGUIRPCHandler Get()
    {
        return GetInstance();
    }
    
    // Called from Mission OnRPC (client-side)
    bool OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        switch (rpc_type)
        {
            // Match state updates
            case SwarmArenaRPC.Match_StateUpdate:
                return OnMatchStateUpdate(ctx);
                
            case SwarmArenaRPC.Match_ScoreUpdate:
                return OnScoreUpdate(ctx);
                
            case SwarmArenaRPC.Match_MoneyUpdate:
                return OnMoneyUpdate(ctx);
                
            case SwarmArenaRPC.Match_FreezeStart:
                return OnFreezeTimeStart(ctx);
                
            case SwarmArenaRPC.Match_RoundStart:
                return OnRoundStart(ctx);
                
            case SwarmArenaRPC.Match_RoundEnd:
                return OnRoundEnd(ctx);
                
            case SwarmArenaRPC.Match_End:
                return OnMatchEnd(ctx);
                
            case SwarmArenaRPC.Match_PlayerDeath:
                return OnPlayerDeath(ctx);
                
            // Buy system responses
            case SwarmArenaRPC.Buy_Result:
                return OnBuyResult(ctx);
                
            // Lobby/Queue updates
            case SwarmArenaRPC.Lobby_QueueStatus:
                return OnQueueUpdate(ctx);
                
            case SwarmArenaRPC.Match_Found:
                return OnMatchFound(ctx);
                
            // Party updates
            case SwarmArenaRPC.Party_Update:
                return OnPartyUpdate(ctx);
                
            case SwarmArenaRPC.Party_InviteReceived:
                return OnPartyInviteReceived(ctx);
        }
        
        return false;
    }
    
    // --- Match state handlers ---
    
    protected bool OnMatchStateUpdate(ParamsReadContext ctx)
    {
        int state;
        if (!ctx.Read(state))
        {
            return false;
        }
        
        SwarmArenaGUIInputHandler inputHandler = SwarmArenaGUIInputHandler.Get();
        
        if (state == SwarmArenaMatchState.FREEZE_TIME)
        {
            inputHandler.SetInMatch(true);
            inputHandler.SetInFreezeTime(true);
            inputHandler.SetInLobby(false);
        }
        else if (state == SwarmArenaMatchState.ROUND_ACTIVE)
        {
            inputHandler.SetInFreezeTime(false);
        }
        else if (state == SwarmArenaMatchState.MATCH_END)
        {
            inputHandler.SetInMatch(false);
        }
        
        return true;
    }
    
    protected bool OnScoreUpdate(ParamsReadContext ctx)
    {
        int teamAScore;
        int teamBScore;
        
        if (!ctx.Read(teamAScore) || !ctx.Read(teamBScore))
        {
            return false;
        }
        
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        hud.SetScore(teamAScore, teamBScore);
        
        return true;
    }
    
    protected bool OnMoneyUpdate(ParamsReadContext ctx)
    {
        int money;
        if (!ctx.Read(money))
        {
            return false;
        }
        
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        hud.SetMoney(money);
        
        // Update buy menu if open
        UIScriptedMenu currentMenu = GetGame().GetUIManager().GetMenu();
        SwarmArenaBuyMenu buyMenu = SwarmArenaBuyMenu.Cast(currentMenu);
        if (buyMenu)
        {
            buyMenu.SetMoney(money);
        }
        
        return true;
    }
    
    protected bool OnFreezeTimeStart(ParamsReadContext ctx)
    {
        int freezeSeconds;
        if (!ctx.Read(freezeSeconds))
        {
            return false;
        }
        
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        hud.SetBuyPromptVisible(true);
        hud.ShowFreezeTime(freezeSeconds);
        
        SwarmArenaGUIInputHandler.Get().SetInFreezeTime(true);
        
        return true;
    }
    
    protected bool OnRoundStart(ParamsReadContext ctx)
    {
        int roundNumber;
        if (!ctx.Read(roundNumber))
        {
            return false;
        }
        
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        hud.SetRound(roundNumber);
        hud.SetBuyPromptVisible(false);
        hud.ShowRoundStart(roundNumber);
        
        SwarmArenaGUIInputHandler.Get().SetInFreezeTime(false);
        
        // Close buy menu if open
        UIScriptedMenu currentMenu = GetGame().GetUIManager().GetMenu();
        if (currentMenu && currentMenu.GetID() == MENU_SWARMARENA_BUY)
        {
            currentMenu.Close();
        }
        
        return true;
    }
    
    protected bool OnRoundEnd(ParamsReadContext ctx)
    {
        int winningTeamIndex;
        int teamAScore;
        int teamBScore;
        
        if (!ctx.Read(winningTeamIndex) || !ctx.Read(teamAScore) || !ctx.Read(teamBScore))
        {
            return false;
        }
        
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        hud.SetScore(teamAScore, teamBScore);
        
        // Determine if local player's team won
        // TODO: Get actual team membership
        bool yourTeamWon = (winningTeamIndex == 0);
        hud.ShowRoundWin(yourTeamWon);
        
        return true;
    }
    
    protected bool OnMatchEnd(ParamsReadContext ctx)
    {
        int winningTeamIndex;
        int teamAScore;
        int teamBScore;
        
        if (!ctx.Read(winningTeamIndex) || !ctx.Read(teamAScore) || !ctx.Read(teamBScore))
        {
            return false;
        }
        
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        
        // Determine if local player's team won
        bool yourTeamWon = (winningTeamIndex == 0);
        hud.ShowMatchWin(yourTeamWon, teamAScore, teamBScore);
        
        SwarmArenaGUIInputHandler inputHandler = SwarmArenaGUIInputHandler.Get();
        inputHandler.SetInMatch(false);
        inputHandler.SetInFreezeTime(false);
        
        return true;
    }
    
    protected bool OnPlayerDeath(ParamsReadContext ctx)
    {
        string killerName;
        string victimName;
        string weaponName;
        
        if (!ctx.Read(killerName) || !ctx.Read(victimName) || !ctx.Read(weaponName))
        {
            return false;
        }
        
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        hud.AddKillFeedEntry(killerName, victimName, weaponName);
        
        return true;
    }
    
    // --- Buy system handlers ---
    
    protected bool OnBuyResult(ParamsReadContext ctx)
    {
        int result;
        int newMoney;
        
        if (!ctx.Read(result) || !ctx.Read(newMoney))
        {
            return false;
        }
        
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        hud.SetMoney(newMoney);
        
        // Show result feedback
        switch (result)
        {
            case SwarmArenaBuyResult.SUCCESS:
                // TODO: Play success sound
                break;
                
            case SwarmArenaBuyResult.INSUFFICIENT_FUNDS:
                // TODO: Show "Not enough money" notification
                break;
                
            case SwarmArenaBuyResult.NOT_IN_FREEZE_TIME:
                // TODO: Show "Buy time ended" notification
                break;
        }
        
        return true;
    }
    
    // --- Lobby/Queue handlers ---
    
    protected bool OnQueueUpdate(ParamsReadContext ctx)
    {
        bool inQueue;
        int queueType;
        
        if (!ctx.Read(inQueue) || !ctx.Read(queueType))
        {
            return false;
        }
        
        // Update lobby menu if open
        UIScriptedMenu currentMenu = GetGame().GetUIManager().GetMenu();
        SwarmArenaLobbyMenu lobbyMenu = SwarmArenaLobbyMenu.Cast(currentMenu);
        if (lobbyMenu)
        {
            lobbyMenu.OnQueueStateChanged(inQueue, queueType);
        }
        
        return true;
    }
    
    protected bool OnMatchFound(ParamsReadContext ctx)
    {
        // Close lobby menu
        UIScriptedMenu currentMenu = GetGame().GetUIManager().GetMenu();
        if (currentMenu && currentMenu.GetID() == MENU_SWARMARENA_LOBBY)
        {
            currentMenu.Close();
        }
        
        // Show match HUD
        SwarmArenaMatchHUD hud = SwarmArenaMatchHUD.Get();
        hud.Init();
        hud.Show();
        
        SwarmArenaGUIInputHandler inputHandler = SwarmArenaGUIInputHandler.Get();
        inputHandler.SetInLobby(false);
        inputHandler.SetInMatch(true);
        
        // TODO: Show "Match found" notification
        
        return true;
    }
    
    // --- Party handlers ---
    
    protected bool OnPartyUpdate(ParamsReadContext ctx)
    {
        // Read party info and update lobby menu
        // TODO: Implement party state reading
        
        return true;
    }
    
    protected bool OnPartyInviteReceived(ParamsReadContext ctx)
    {
        string fromSteamId;
        string fromName;
        
        if (!ctx.Read(fromSteamId) || !ctx.Read(fromName))
        {
            return false;
        }
        
        // Create pending invite
        SwarmArenaPendingInvite invite = new SwarmArenaPendingInvite();
        invite.FromSteamId = fromSteamId;
        invite.FromName = fromName;
        invite.ToSteamId = "";
        invite.SentTime = GetGame().GetTime();
        
        // Update lobby menu if open
        UIScriptedMenu currentMenu = GetGame().GetUIManager().GetMenu();
        SwarmArenaLobbyMenu lobbyMenu = SwarmArenaLobbyMenu.Cast(currentMenu);
        if (lobbyMenu)
        {
            lobbyMenu.SetPendingInvite(invite);
        }
        
        // TODO: Show notification popup
        
        return true;
    }
}

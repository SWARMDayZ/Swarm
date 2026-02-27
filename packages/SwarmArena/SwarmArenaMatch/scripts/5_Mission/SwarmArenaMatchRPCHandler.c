// SwarmArenaMatchRPCHandler - Handles RPC communication for match operations

class SwarmArenaMatchRPCHandler
{
    private static ref SwarmArenaMatchRPCHandler s_Instance;
    
    static SwarmArenaMatchRPCHandler GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaMatchRPCHandler();
        }
        return s_Instance;
    }
    
    static SwarmArenaMatchRPCHandler Get()
    {
        return GetInstance();
    }
    
    // Called from MissionServer OnRPC
    bool OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        if (!sender)
        {
            return false;
        }
        
        string steamId = sender.GetPlainId();
        
        switch (rpc_type)
        {
            // Buy RPCs
            case SwarmArenaRPC.Buy_Preset:
                return OnBuyPreset(steamId, ctx);
                
            case SwarmArenaRPC.Buy_Armor:
                return OnBuyArmor(steamId, ctx);
        }
        
        return false;
    }
    
    // --- Buy RPC handlers ---
    
    protected bool OnBuyPreset(string steamId, ParamsReadContext ctx)
    {
        int presetIndex;
        if (!ctx.Read(presetIndex))
        {
            SwarmArenaCore.LogError("Failed to read preset index");
            return false;
        }
        
        SwarmArenaMatch match = SwarmArenaInstanceManager.Get().GetPlayerMatch(steamId);
        if (!match)
        {
            SwarmArenaCore.LogWarning("Player " + steamId + " not in a match");
            return false;
        }
        
        return match.BuyPreset(steamId, presetIndex);
    }
    
    protected bool OnBuyArmor(string steamId, ParamsReadContext ctx)
    {
        int armorTypeInt;
        if (!ctx.Read(armorTypeInt))
        {
            SwarmArenaCore.LogError("Failed to read armor type");
            return false;
        }
        
        SwarmArenaArmorType armorType = armorTypeInt;
        
        SwarmArenaMatch match = SwarmArenaInstanceManager.Get().GetPlayerMatch(steamId);
        if (!match)
        {
            SwarmArenaCore.LogWarning("Player " + steamId + " not in a match");
            return false;
        }
        
        return match.BuyArmor(steamId, armorType);
    }
    
    // --- Send RPCs to clients ---
    
    static void SendMatchFound(PlayerIdentity identity, int matchId, string mapName, int teamIndex)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(matchId);
        rpc.Write(mapName);
        rpc.Write(teamIndex);
        rpc.Send(null, SwarmArenaRPC.Match_Found, true, identity);
    }
    
    static void SendRoundStart(PlayerIdentity identity, int roundNumber, float freezeTime)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(roundNumber);
        rpc.Write(freezeTime);
        rpc.Send(null, SwarmArenaRPC.Match_RoundStart, true, identity);
    }
    
    static void SendFreezeEnd(PlayerIdentity identity)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Send(null, SwarmArenaRPC.Match_FreezeEnd, true, identity);
    }
    
    static void SendRoundEnd(PlayerIdentity identity, int winningTeam, int team1Score, int team2Score)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(winningTeam);
        rpc.Write(team1Score);
        rpc.Write(team2Score);
        rpc.Send(null, SwarmArenaRPC.Match_RoundEnd, true, identity);
    }
    
    static void SendMatchEnd(PlayerIdentity identity, int winningTeam, int team1Score, int team2Score)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(winningTeam);
        rpc.Write(team1Score);
        rpc.Write(team2Score);
        rpc.Send(null, SwarmArenaRPC.Match_End, true, identity);
    }
    
    static void SendScoreUpdate(PlayerIdentity identity, int team1Score, int team2Score)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(team1Score);
        rpc.Write(team2Score);
        rpc.Send(null, SwarmArenaRPC.Match_ScoreUpdate, true, identity);
    }
    
    static void SendMoneyUpdate(PlayerIdentity identity, int money)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(money);
        rpc.Send(null, SwarmArenaRPC.Match_MoneyUpdate, true, identity);
    }
    
    static void SendBuyResult(PlayerIdentity identity, int result, int newMoney)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(result);
        rpc.Write(newMoney);
        rpc.Send(null, SwarmArenaRPC.Buy_Result, true, identity);
    }
}

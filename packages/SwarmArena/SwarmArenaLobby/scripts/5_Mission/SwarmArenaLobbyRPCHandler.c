// SwarmArenaLobbyRPCHandler - Handles RPC communication for lobby operations

class SwarmArenaLobbyRPCHandler
{
    private static ref SwarmArenaLobbyRPCHandler s_Instance;
    
    static SwarmArenaLobbyRPCHandler GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaLobbyRPCHandler();
        }
        return s_Instance;
    }
    
    static SwarmArenaLobbyRPCHandler Get()
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
        string playerName = sender.GetName();
        
        switch (rpc_type)
        {
            // Queue RPCs
            case SwarmArenaRPC.Lobby_JoinQueue:
                return OnJoinQueue(steamId, playerName, ctx);
                
            case SwarmArenaRPC.Lobby_LeaveQueue:
                return OnLeaveQueue(steamId);
                
            // Party RPCs
            case SwarmArenaRPC.Party_Create:
                return OnPartyCreate(steamId, playerName);
                
            case SwarmArenaRPC.Party_Invite:
                return OnPartyInvite(steamId, playerName, ctx);
                
            case SwarmArenaRPC.Party_Accept:
                return OnPartyAccept(steamId, playerName, ctx);
                
            case SwarmArenaRPC.Party_Decline:
                return OnPartyDecline(steamId, ctx);
                
            case SwarmArenaRPC.Party_Leave:
                return OnPartyLeave(steamId);
                
            case SwarmArenaRPC.Party_Kick:
                return OnPartyKick(steamId, ctx);
        }
        
        return false;
    }
    
    // --- Queue RPC handlers ---
    
    protected bool OnJoinQueue(string steamId, string playerName, ParamsReadContext ctx)
    {
        int queueTypeInt;
        if (!ctx.Read(queueTypeInt))
        {
            SwarmArenaCore.LogError("Failed to read queue type");
            return false;
        }
        
        SwarmArenaQueueType queueType = queueTypeInt;
        return SwarmArenaMatchmaker.Get().JoinQueue(steamId, playerName, queueType);
    }
    
    protected bool OnLeaveQueue(string steamId)
    {
        return SwarmArenaMatchmaker.Get().LeaveQueue(steamId);
    }
    
    // --- Party RPC handlers ---
    
    protected bool OnPartyCreate(string steamId, string playerName)
    {
        return SwarmArenaPartyManager.Get().CreateParty(steamId, playerName);
    }
    
    protected bool OnPartyInvite(string steamId, string playerName, ParamsReadContext ctx)
    {
        string targetSteamId;
        if (!ctx.Read(targetSteamId))
        {
            SwarmArenaCore.LogError("Failed to read target steam id");
            return false;
        }
        
        // Get target player name
        SwarmArenaPlayerData targetData = SwarmArenaLobbyManager.Get().GetPlayerData(targetSteamId);
        if (!targetData)
        {
            SwarmArenaCore.LogWarning("Target player not found");
            return false;
        }
        
        return SwarmArenaPartyManager.Get().InviteToParty(steamId, playerName, targetSteamId, targetData.PlayerName);
    }
    
    protected bool OnPartyAccept(string steamId, string playerName, ParamsReadContext ctx)
    {
        string fromSteamId;
        if (!ctx.Read(fromSteamId))
        {
            SwarmArenaCore.LogError("Failed to read from steam id");
            return false;
        }
        
        return SwarmArenaPartyManager.Get().AcceptInvite(steamId, playerName, fromSteamId);
    }
    
    protected bool OnPartyDecline(string steamId, ParamsReadContext ctx)
    {
        string fromSteamId;
        if (!ctx.Read(fromSteamId))
        {
            SwarmArenaCore.LogError("Failed to read from steam id");
            return false;
        }
        
        SwarmArenaPartyManager.Get().DeclineInvite(steamId, fromSteamId);
        return true;
    }
    
    protected bool OnPartyLeave(string steamId)
    {
        return SwarmArenaPartyManager.Get().LeaveParty(steamId);
    }
    
    protected bool OnPartyKick(string steamId, ParamsReadContext ctx)
    {
        string targetSteamId;
        if (!ctx.Read(targetSteamId))
        {
            SwarmArenaCore.LogError("Failed to read target steam id");
            return false;
        }
        
        return SwarmArenaPartyManager.Get().KickFromParty(steamId, targetSteamId);
    }
    
    // --- Send RPCs to clients ---
    
    static void SendQueueStatus(PlayerIdentity identity, int position, int queueSize)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(position);
        rpc.Write(queueSize);
        rpc.Send(null, SwarmArenaRPC.Lobby_QueueStatus, true, identity);
    }
    
    static void SendPartyInvite(PlayerIdentity identity, string fromSteamId, string fromName)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(fromSteamId);
        rpc.Write(fromName);
        rpc.Send(null, SwarmArenaRPC.Party_InviteReceived, true, identity);
    }
    
    static void SendPartyUpdate(PlayerIdentity identity, SwarmArenaParty party)
    {
        if (!identity || !party)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(party.OwnerSteamId);
        rpc.Write(party.OwnerName);
        rpc.Write(party.MemberSteamIds.Count());
        
        for (int i = 0; i < party.MemberSteamIds.Count(); i++)
        {
            rpc.Write(party.MemberSteamIds[i]);
            rpc.Write(party.MemberNames[i]);
        }
        
        rpc.Write(party.IsInQueue);
        rpc.Write(party.QueuedFor);
        
        rpc.Send(null, SwarmArenaRPC.Party_Update, true, identity);
    }
    
    static void SendPartyDisbanded(PlayerIdentity identity)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Send(null, SwarmArenaRPC.Party_Disbanded, true, identity);
    }
}

// SwarmArenaSpectatorRPCHandler - Client-side RPC handler for spectator commands

class SwarmArenaSpectatorRPCHandler
{
    private static ref SwarmArenaSpectatorRPCHandler s_Instance;
    
    static SwarmArenaSpectatorRPCHandler GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaSpectatorRPCHandler();
        }
        return s_Instance;
    }
    
    static SwarmArenaSpectatorRPCHandler Get()
    {
        return GetInstance();
    }
    
    // Called from Mission OnRPC (client-side)
    bool OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        switch (rpc_type)
        {
            case SwarmArenaRPC.Spectator_Enter:
                return OnSpectatorEnter(ctx);
                
            case SwarmArenaRPC.Spectator_Exit:
                return OnSpectatorExit();
                
            case SwarmArenaRPC.Spectator_TargetUpdate:
                return OnSpectatorTargetUpdate(ctx);
        }
        
        return false;
    }
    
    protected bool OnSpectatorEnter(ParamsReadContext ctx)
    {
        // Read initial target
        string targetSteamId;
        if (!ctx.Read(targetSteamId))
        {
            SwarmArenaCore.LogError("Failed to read spectator target");
            return false;
        }
        
        // Read valid targets count
        int targetCount;
        if (!ctx.Read(targetCount))
        {
            SwarmArenaCore.LogError("Failed to read spectator target count");
            return false;
        }
        
        // Read valid targets
        array<string> validTargets = new array<string>();
        for (int i = 0; i < targetCount; i++)
        {
            string steamId;
            if (ctx.Read(steamId))
            {
                validTargets.Insert(steamId);
            }
        }
        
        // Enter spectator mode
        SwarmArenaSpectatorCamera.Enter(targetSteamId, validTargets);
        
        return true;
    }
    
    protected bool OnSpectatorExit()
    {
        SwarmArenaSpectatorCamera.Leave();
        return true;
    }
    
    protected bool OnSpectatorTargetUpdate(ParamsReadContext ctx)
    {
        // Read new targets list
        int targetCount;
        if (!ctx.Read(targetCount))
        {
            return false;
        }
        
        array<string> validTargets = new array<string>();
        for (int i = 0; i < targetCount; i++)
        {
            string steamId;
            if (ctx.Read(steamId))
            {
                validTargets.Insert(steamId);
            }
        }
        
        SwarmArenaSpectatorCamera.UpdateValidTargets(validTargets);
        
        return true;
    }
    
    // --- Server-side RPC sending ---
    
    static void SendEnterSpectator(PlayerIdentity identity, string targetSteamId, array<string> validTargets)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(targetSteamId);
        rpc.Write(validTargets.Count());
        
        foreach (string steamId : validTargets)
        {
            rpc.Write(steamId);
        }
        
        rpc.Send(null, SwarmArenaRPC.Spectator_Enter, true, identity);
    }
    
    static void SendExitSpectator(PlayerIdentity identity)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Send(null, SwarmArenaRPC.Spectator_Exit, true, identity);
    }
    
    static void SendTargetUpdate(PlayerIdentity identity, array<string> validTargets)
    {
        if (!identity)
        {
            return;
        }
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(validTargets.Count());
        
        foreach (string steamId : validTargets)
        {
            rpc.Write(steamId);
        }
        
        rpc.Send(null, SwarmArenaRPC.Spectator_TargetUpdate, true, identity);
    }
}

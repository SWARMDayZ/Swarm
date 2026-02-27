// SwarmArenaPlayerBase - Modded PlayerBase for arena death handling

modded class PlayerBase
{
    // Arena state
    protected bool m_SwarmArenaInMatch = false;
    protected bool m_SwarmArenaInLobby = false;
    protected int m_SwarmArenaMatchId = -1;
    protected int m_SwarmArenaTeamIndex = -1;
    protected bool m_SwarmArenaFrozen = false;
    
    // Set lobby state (for god mode in lobby)
    void SetSwarmArenaLobbyState(bool inLobby)
    {
        m_SwarmArenaInLobby = inLobby;
    }
    
    // Set arena match state
    void SetSwarmArenaMatchState(bool inMatch, int matchId, int teamIndex)
    {
        m_SwarmArenaInMatch = inMatch;
        m_SwarmArenaMatchId = matchId;
        m_SwarmArenaTeamIndex = teamIndex;
    }
    
    bool IsInSwarmArenaMatch()
    {
        return m_SwarmArenaInMatch;
    }
    
    int GetSwarmArenaMatchId()
    {
        return m_SwarmArenaMatchId;
    }
    
    int GetSwarmArenaTeamIndex()
    {
        return m_SwarmArenaTeamIndex;
    }
    
    // Freeze state for buy time
    void SetSwarmArenaFrozen(bool frozen)
    {
        m_SwarmArenaFrozen = frozen;
        
        // Disable movement when frozen
        // TODO: Implement movement prevention
    }
    
    bool IsSwarmArenaFrozen()
    {
        return m_SwarmArenaFrozen;
    }
    
    // Override death handling
    override void EEKilled(Object killer)
    {
        if (m_SwarmArenaInMatch && m_SwarmArenaMatchId >= 0)
        {
            // Get killer's steam ID
            string killerSteamId = "";
            PlayerBase killerPlayer = PlayerBase.Cast(killer);
            if (killerPlayer && killerPlayer.GetIdentity())
            {
                killerSteamId = killerPlayer.GetIdentity().GetPlainId();
            }
            
            // Get our steam ID
            string steamId = "";
            if (GetIdentity())
            {
                steamId = GetIdentity().GetPlainId();
            }
            
            // Send death notification via RPC (match is in 5_Mission layer)
            ScriptRPC rpc = new ScriptRPC();
            rpc.Write(m_SwarmArenaMatchId);
            rpc.Write(steamId);
            rpc.Write(killerSteamId);
            rpc.Send(this, SwarmArenaRPC.Match_PlayerDeath, true, null);
            
            // Prevent actual death - this is handled by the match
            return;
        }
        
        // Not in arena match, normal death
        super.EEKilled(killer);
    }
    
    // Override damage handling for lobby protection
    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        // Check if in lobby (god mode) - lobby state set by SwarmArenaLobbyManager
        if (!m_SwarmArenaInMatch && m_SwarmArenaInLobby)
        {
            // In lobby - ignore damage
            return;
        }
        
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
    }
}

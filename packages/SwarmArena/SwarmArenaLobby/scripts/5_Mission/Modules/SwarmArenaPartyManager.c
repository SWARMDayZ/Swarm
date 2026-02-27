// SwarmArenaPartyManager - Manages party creation, invites, and membership

class SwarmArenaPartyManager
{
    private static ref SwarmArenaPartyManager s_Instance;
    
    // Active parties (owner steam id -> party)
    protected ref map<string, ref SwarmArenaParty> m_Parties = new map<string, ref SwarmArenaParty>();
    
    // Player to party mapping (player steam id -> owner steam id)
    protected ref map<string, string> m_PlayerToParty = new map<string, string>();
    
    // Pending invites
    protected ref array<ref SwarmArenaPendingInvite> m_PendingInvites = new array<ref SwarmArenaPendingInvite>();
    
    static SwarmArenaPartyManager GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaPartyManager();
        }
        return s_Instance;
    }
    
    static SwarmArenaPartyManager Get()
    {
        return GetInstance();
    }
    
    void SwarmArenaPartyManager()
    {
        SwarmArenaCore.Log("PartyManager initialized");
    }
    
    // Create a new party with the player as owner
    bool CreateParty(string ownerSteamId, string ownerName)
    {
        // Check if already in a party
        if (IsInParty(ownerSteamId))
        {
            SwarmArenaCore.LogWarning("Player " + ownerName + " tried to create party but is already in one");
            return false;
        }
        
        SwarmArenaParty party = new SwarmArenaParty();
        party.Create(ownerSteamId, ownerName);
        
        m_Parties.Insert(ownerSteamId, party);
        m_PlayerToParty.Insert(ownerSteamId, ownerSteamId);
        
        // Update player data
        SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(ownerSteamId);
        if (playerData)
        {
            playerData.SetParty(ownerSteamId);
        }
        
        SwarmArenaCore.Log("Party created by " + ownerName);
        return true;
    }
    
    // Send party invite to another player
    bool InviteToParty(string fromSteamId, string fromName, string toSteamId, string toName)
    {
        // Check if sender is party owner
        SwarmArenaParty party = GetParty(fromSteamId);
        if (!party)
        {
            SwarmArenaCore.LogWarning("Player " + fromName + " tried to invite but has no party");
            return false;
        }
        
        if (!party.IsOwner(fromSteamId))
        {
            SwarmArenaCore.LogWarning("Player " + fromName + " tried to invite but is not party owner");
            return false;
        }
        
        // Check if party is full
        if (party.IsFull())
        {
            SwarmArenaCore.LogWarning("Party is full, cannot invite " + toName);
            return false;
        }
        
        // Check if target is already in a party
        if (IsInParty(toSteamId))
        {
            SwarmArenaCore.LogWarning("Player " + toName + " is already in a party");
            return false;
        }
        
        // Check if invite already pending
        if (HasPendingInvite(fromSteamId, toSteamId))
        {
            SwarmArenaCore.LogWarning("Invite already pending for " + toName);
            return false;
        }
        
        // Create pending invite
        SwarmArenaPendingInvite invite = new SwarmArenaPendingInvite();
        invite.Set(fromSteamId, fromName, toSteamId, toName);
        m_PendingInvites.Insert(invite);
        
        // Send RPC to target player
        SendInviteToClient(toSteamId, fromSteamId, fromName);
        
        SwarmArenaCore.Log("Party invite sent from " + fromName + " to " + toName);
        return true;
    }
    
    // Accept a party invite
    bool AcceptInvite(string toSteamId, string toName, string fromSteamId)
    {
        // Find and validate invite
        SwarmArenaPendingInvite invite = FindInvite(fromSteamId, toSteamId);
        if (!invite)
        {
            SwarmArenaCore.LogWarning("No valid invite found for " + toName);
            return false;
        }
        
        // Check if invite expired
        if (invite.IsExpired())
        {
            RemoveInvite(invite);
            SwarmArenaCore.LogWarning("Invite expired for " + toName);
            return false;
        }
        
        // Get party
        SwarmArenaParty party = GetParty(fromSteamId);
        if (!party)
        {
            RemoveInvite(invite);
            SwarmArenaCore.LogWarning("Party no longer exists");
            return false;
        }
        
        // Check if still room
        if (party.IsFull())
        {
            RemoveInvite(invite);
            SwarmArenaCore.LogWarning("Party is now full");
            return false;
        }
        
        // Add to party
        if (!party.AddMember(toSteamId, toName))
        {
            RemoveInvite(invite);
            return false;
        }
        
        m_PlayerToParty.Insert(toSteamId, fromSteamId);
        
        // Update player data
        SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(toSteamId);
        if (playerData)
        {
            playerData.SetParty(fromSteamId);
        }
        
        RemoveInvite(invite);
        
        // Notify all party members
        BroadcastPartyUpdate(party);
        
        SwarmArenaCore.Log("Player " + toName + " joined party of " + invite.FromName);
        return true;
    }
    
    // Decline a party invite
    void DeclineInvite(string toSteamId, string fromSteamId)
    {
        SwarmArenaPendingInvite invite = FindInvite(fromSteamId, toSteamId);
        if (invite)
        {
            RemoveInvite(invite);
            SwarmArenaCore.Log("Party invite declined");
        }
    }
    
    // Leave current party
    bool LeaveParty(string steamId)
    {
        if (!IsInParty(steamId))
        {
            return false;
        }
        
        string ownerSteamId = m_PlayerToParty.Get(steamId);
        SwarmArenaParty party = GetParty(ownerSteamId);
        
        if (!party)
        {
            m_PlayerToParty.Remove(steamId);
            return false;
        }
        
        // If owner leaves, disband party
        if (party.IsOwner(steamId))
        {
            DisbandParty(ownerSteamId);
        }
        else
        {
            // Remove member
            party.RemoveMember(steamId);
            m_PlayerToParty.Remove(steamId);
            
            // Update player data
            SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
            if (playerData)
            {
                playerData.LeaveParty();
            }
            
            // Notify remaining members
            BroadcastPartyUpdate(party);
        }
        
        return true;
    }
    
    // Kick a player from party (owner only)
    bool KickFromParty(string ownerSteamId, string targetSteamId)
    {
        SwarmArenaParty party = GetParty(ownerSteamId);
        if (!party)
        {
            return false;
        }
        
        if (!party.IsOwner(ownerSteamId))
        {
            return false;
        }
        
        if (ownerSteamId == targetSteamId)
        {
            return false; // Can't kick yourself
        }
        
        if (!party.HasMember(targetSteamId))
        {
            return false;
        }
        
        party.RemoveMember(targetSteamId);
        m_PlayerToParty.Remove(targetSteamId);
        
        // Update kicked player data
        SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(targetSteamId);
        if (playerData)
        {
            playerData.LeaveParty();
        }
        
        // Notify kicked player
        SendKickNotification(targetSteamId);
        
        // Notify remaining members
        BroadcastPartyUpdate(party);
        
        return true;
    }
    
    // Disband entire party
    void DisbandParty(string ownerSteamId)
    {
        SwarmArenaParty party = GetParty(ownerSteamId);
        if (!party)
        {
            return;
        }
        
        // Leave queue if in one
        if (party.IsInQueue)
        {
            SwarmArenaMatchmaker.Get().RemovePartyFromQueue(ownerSteamId);
        }
        
        // Notify all members and clean up
        foreach (string memberSteamId : party.MemberSteamIds)
        {
            m_PlayerToParty.Remove(memberSteamId);
            
            SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(memberSteamId);
            if (playerData)
            {
                playerData.LeaveParty();
            }
            
            // Send disband notification
            SendDisbandNotification(memberSteamId);
        }
        
        m_Parties.Remove(ownerSteamId);
        SwarmArenaCore.Log("Party disbanded by owner " + ownerSteamId);
    }
    
    // Get party by owner steam id
    SwarmArenaParty GetParty(string ownerSteamId)
    {
        if (m_Parties.Contains(ownerSteamId))
        {
            return m_Parties.Get(ownerSteamId);
        }
        return null;
    }
    
    // Get party that a player is in
    SwarmArenaParty GetPlayerParty(string steamId)
    {
        if (!m_PlayerToParty.Contains(steamId))
        {
            return null;
        }
        
        string ownerSteamId = m_PlayerToParty.Get(steamId);
        return GetParty(ownerSteamId);
    }
    
    // Check if player is in any party
    bool IsInParty(string steamId)
    {
        return m_PlayerToParty.Contains(steamId);
    }
    
    // Check if player is a party owner
    bool IsPartyOwner(string steamId)
    {
        return m_Parties.Contains(steamId);
    }
    
    // Handle player disconnect
    void OnPlayerDisconnect(string steamId)
    {
        // Remove any pending invites involving this player
        CleanupInvitesForPlayer(steamId);
        
        // Leave party if in one
        LeaveParty(steamId);
    }
    
    // Clean up expired invites (call periodically)
    void CleanupExpiredInvites()
    {
        for (int i = m_PendingInvites.Count() - 1; i >= 0; i--)
        {
            if (m_PendingInvites[i].IsExpired())
            {
                m_PendingInvites.Remove(i);
            }
        }
    }
    
    // --- Helper methods ---
    
    protected bool HasPendingInvite(string fromSteamId, string toSteamId)
    {
        foreach (SwarmArenaPendingInvite invite : m_PendingInvites)
        {
            if (invite.FromSteamId == fromSteamId && invite.ToSteamId == toSteamId)
            {
                return true;
            }
        }
        return false;
    }
    
    protected SwarmArenaPendingInvite FindInvite(string fromSteamId, string toSteamId)
    {
        foreach (SwarmArenaPendingInvite invite : m_PendingInvites)
        {
            if (invite.FromSteamId == fromSteamId && invite.ToSteamId == toSteamId)
            {
                return invite;
            }
        }
        return null;
    }
    
    protected void RemoveInvite(SwarmArenaPendingInvite invite)
    {
        int index = m_PendingInvites.Find(invite);
        if (index >= 0)
        {
            m_PendingInvites.Remove(index);
        }
    }
    
    protected void CleanupInvitesForPlayer(string steamId)
    {
        for (int i = m_PendingInvites.Count() - 1; i >= 0; i--)
        {
            SwarmArenaPendingInvite invite = m_PendingInvites[i];
            if (invite.FromSteamId == steamId || invite.ToSteamId == steamId)
            {
                m_PendingInvites.Remove(i);
            }
        }
    }
    
    // --- RPC sending methods (to be implemented with actual RPC) ---
    
    protected void SendInviteToClient(string toSteamId, string fromSteamId, string fromName)
    {
        // TODO: Send RPC Party_InviteReceived to target client
    }
    
    protected void BroadcastPartyUpdate(SwarmArenaParty party)
    {
        // TODO: Send RPC Party_Update to all party members
    }
    
    protected void SendKickNotification(string targetSteamId)
    {
        // TODO: Send RPC Party_Disbanded to kicked player
    }
    
    protected void SendDisbandNotification(string memberSteamId)
    {
        // TODO: Send RPC Party_Disbanded to member
    }
}

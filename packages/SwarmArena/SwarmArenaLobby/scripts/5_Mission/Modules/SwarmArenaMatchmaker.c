// SwarmArenaMatchmaker - Queue management and matchmaking logic

class SwarmArenaMatchmaker
{
    private static ref SwarmArenaMatchmaker s_Instance;
    
    // Solo queues (individual players)
    protected ref array<ref SwarmArenaQueueEntry> m_Queue1v1 = new array<ref SwarmArenaQueueEntry>();
    protected ref array<ref SwarmArenaQueueEntry> m_Queue2v2Solo = new array<ref SwarmArenaQueueEntry>();
    protected ref array<ref SwarmArenaQueueEntry> m_Queue3v3Solo = new array<ref SwarmArenaQueueEntry>();
    
    // Party queues (groups only)
    protected ref array<ref SwarmArenaParty> m_PartyQueue2v2 = new array<ref SwarmArenaParty>();
    protected ref array<ref SwarmArenaParty> m_PartyQueue3v3 = new array<ref SwarmArenaParty>();
    
    // Check interval
    protected float m_CheckTimer = 0;
    protected const float CHECK_INTERVAL = 1.0; // Check every second
    
    static SwarmArenaMatchmaker GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaMatchmaker();
        }
        return s_Instance;
    }
    
    static SwarmArenaMatchmaker Get()
    {
        return GetInstance();
    }
    
    void SwarmArenaMatchmaker()
    {
        SwarmArenaCore.Log("Matchmaker initialized");
    }
    
    // Join queue as solo player
    bool JoinQueue(string steamId, string playerName, SwarmArenaQueueType queueType)
    {
        // Check if player is in a party
        SwarmArenaParty party = SwarmArenaPartyManager.Get().GetPlayerParty(steamId);
        
        if (party)
        {
            // Party member - only owner can queue
            if (!party.IsOwner(steamId))
            {
                SwarmArenaCore.LogWarning("Only party owner can queue");
                return false;
            }
            
            // Use party queue
            return JoinQueueAsParty(party, queueType);
        }
        
        // Solo player - validate queue type
        bool validSoloQueue = (queueType == SwarmArenaQueueType.SOLO_1V1 || queueType == SwarmArenaQueueType.SOLO_2V2 || queueType == SwarmArenaQueueType.SOLO_3V3);
        if (!validSoloQueue)
        {
            SwarmArenaCore.LogWarning("Invalid queue type for solo player");
            return false;
        }
        
        // Check if already in queue
        if (IsInAnyQueue(steamId))
        {
            SwarmArenaCore.LogWarning("Player " + playerName + " already in queue");
            return false;
        }
        
        // Create queue entry
        SwarmArenaQueueEntry entry = new SwarmArenaQueueEntry();
        entry.Set(steamId, playerName, queueType);
        
        // Add to appropriate queue
        switch (queueType)
        {
            case SwarmArenaQueueType.SOLO_1V1:
                m_Queue1v1.Insert(entry);
                break;
            case SwarmArenaQueueType.SOLO_2V2:
                m_Queue2v2Solo.Insert(entry);
                break;
            case SwarmArenaQueueType.SOLO_3V3:
                m_Queue3v3Solo.Insert(entry);
                break;
        }
        
        // Update player data
        SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
        if (playerData)
        {
            playerData.IsInQueue = true;
            playerData.QueueType = queueType;
        }
        
        SwarmArenaCore.Log("Player " + playerName + " joined queue " + typename.EnumToString(SwarmArenaQueueType, queueType));
        
        // Try to find a match immediately
        TryMatch(queueType);
        
        return true;
    }
    
    // Join queue as a party
    bool JoinQueueAsParty(SwarmArenaParty party, SwarmArenaQueueType queueType)
    {
        if (!party)
        {
            return false;
        }
        
        // Validate party can join this queue type
        if (!party.CanJoinQueue(queueType))
        {
            SwarmArenaCore.LogWarning("Party cannot join queue type " + typename.EnumToString(SwarmArenaQueueType, queueType));
            return false;
        }
        
        // Check if party already in queue
        if (party.IsInQueue)
        {
            SwarmArenaCore.LogWarning("Party already in queue");
            return false;
        }
        
        // Add to appropriate party queue
        switch (queueType)
        {
            case SwarmArenaQueueType.PARTY_2V2:
                m_PartyQueue2v2.Insert(party);
                break;
            case SwarmArenaQueueType.PARTY_3V3:
                m_PartyQueue3v3.Insert(party);
                break;
            default:
                SwarmArenaCore.LogWarning("Invalid queue type for party");
                return false;
        }
        
        party.SetQueueState(true, queueType);
        
        // Update all party members' player data
        foreach (string memberSteamId : party.MemberSteamIds)
        {
            SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(memberSteamId);
            if (playerData)
            {
                playerData.IsInQueue = true;
                playerData.QueueType = queueType;
            }
        }
        
        SwarmArenaCore.Log("Party of " + party.GetMemberCount().ToString() + " joined queue " + typename.EnumToString(SwarmArenaQueueType, queueType));
        
        // Try to find a match immediately
        TryMatch(queueType);
        
        return true;
    }
    
    // Leave queue
    bool LeaveQueue(string steamId)
    {
        bool removed = false;
        
        // Check if in party
        SwarmArenaParty party = SwarmArenaPartyManager.Get().GetPlayerParty(steamId);
        if (party && party.IsOwner(steamId) && party.IsInQueue)
        {
            removed = RemovePartyFromQueue(party.OwnerSteamId);
        }
        else
        {
            removed = RemoveFromAllQueues(steamId);
        }
        
        if (removed)
        {
            // Update player data
            SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
            if (playerData)
            {
                playerData.IsInQueue = false;
                playerData.QueueType = SwarmArenaQueueType.INVALID;
            }
        }
        
        return removed;
    }
    
    // Remove player from all solo queues
    bool RemoveFromAllQueues(string steamId)
    {
        bool removed = false;
        
        // Check 1v1 queue
        for (int i = m_Queue1v1.Count() - 1; i >= 0; i--)
        {
            if (m_Queue1v1[i].SteamId == steamId)
            {
                m_Queue1v1.Remove(i);
                removed = true;
            }
        }
        
        // Check 2v2 solo queue
        for (int j = m_Queue2v2Solo.Count() - 1; j >= 0; j--)
        {
            if (m_Queue2v2Solo[j].SteamId == steamId)
            {
                m_Queue2v2Solo.Remove(j);
                removed = true;
            }
        }
        
        // Check 3v3 solo queue
        for (int k = m_Queue3v3Solo.Count() - 1; k >= 0; k--)
        {
            if (m_Queue3v3Solo[k].SteamId == steamId)
            {
                m_Queue3v3Solo.Remove(k);
                removed = true;
            }
        }
        
        if (removed)
        {
            SwarmArenaCore.Log("Player " + steamId + " removed from queue");
        }
        
        return removed;
    }
    
    // Remove party from queue
    bool RemovePartyFromQueue(string ownerSteamId)
    {
        SwarmArenaParty party = SwarmArenaPartyManager.Get().GetParty(ownerSteamId);
        if (!party || !party.IsInQueue)
        {
            return false;
        }
        
        bool removed = false;
        
        // Check 2v2 party queue
        for (int i = m_PartyQueue2v2.Count() - 1; i >= 0; i--)
        {
            if (m_PartyQueue2v2[i].OwnerSteamId == ownerSteamId)
            {
                m_PartyQueue2v2.Remove(i);
                removed = true;
            }
        }
        
        // Check 3v3 party queue
        for (int j = m_PartyQueue3v3.Count() - 1; j >= 0; j--)
        {
            if (m_PartyQueue3v3[j].OwnerSteamId == ownerSteamId)
            {
                m_PartyQueue3v3.Remove(j);
                removed = true;
            }
        }
        
        if (removed)
        {
            party.LeaveQueue();
            
            // Update all party members' player data
            foreach (string memberSteamId : party.MemberSteamIds)
            {
                SwarmArenaPlayerData playerData = SwarmArenaLobbyManager.Get().GetPlayerData(memberSteamId);
                if (playerData)
                {
                    playerData.IsInQueue = false;
                    playerData.QueueType = SwarmArenaQueueType.INVALID;
                }
            }
            
            SwarmArenaCore.Log("Party of " + ownerSteamId + " removed from queue");
        }
        
        return removed;
    }
    
    // Check if player is in any queue
    bool IsInAnyQueue(string steamId)
    {
        // Check solo queues
        foreach (SwarmArenaQueueEntry entry : m_Queue1v1)
        {
            if (entry.SteamId == steamId) return true;
        }
        
        foreach (SwarmArenaQueueEntry entry2 : m_Queue2v2Solo)
        {
            if (entry2.SteamId == steamId) return true;
        }
        
        foreach (SwarmArenaQueueEntry entry3 : m_Queue3v3Solo)
        {
            if (entry3.SteamId == steamId) return true;
        }
        
        // Check if in party that's in queue
        SwarmArenaParty party = SwarmArenaPartyManager.Get().GetPlayerParty(steamId);
        if (party && party.IsInQueue)
        {
            return true;
        }
        
        return false;
    }
    
    // Get queue position for a player
    int GetQueuePosition(string steamId, SwarmArenaQueueType queueType)
    {
        array<ref SwarmArenaQueueEntry> queue;
        
        switch (queueType)
        {
            case SwarmArenaQueueType.SOLO_1V1:
                queue = m_Queue1v1;
                break;
            case SwarmArenaQueueType.SOLO_2V2:
                queue = m_Queue2v2Solo;
                break;
            case SwarmArenaQueueType.SOLO_3V3:
                queue = m_Queue3v3Solo;
                break;
            default:
                return -1;
        }
        
        for (int i = 0; i < queue.Count(); i++)
        {
            if (queue[i].SteamId == steamId)
            {
                return i + 1;
            }
        }
        
        return -1;
    }
    
    // Try to create a match for the given queue type
    void TryMatch(SwarmArenaQueueType queueType)
    {
        switch (queueType)
        {
            case SwarmArenaQueueType.SOLO_1V1:
                TryMatch1v1();
                break;
            case SwarmArenaQueueType.SOLO_2V2:
                TryMatch2v2Solo();
                break;
            case SwarmArenaQueueType.SOLO_3V3:
                TryMatch3v3Solo();
                break;
            case SwarmArenaQueueType.PARTY_2V2:
                TryMatch2v2Party();
                break;
            case SwarmArenaQueueType.PARTY_3V3:
                TryMatch3v3Party();
                break;
        }
    }
    
    // Try to create 1v1 match
    protected void TryMatch1v1()
    {
        if (m_Queue1v1.Count() >= 2)
        {
            // Pop first 2 players
            SwarmArenaQueueEntry player1 = m_Queue1v1[0];
            SwarmArenaQueueEntry player2 = m_Queue1v1[1];
            
            m_Queue1v1.Remove(0);
            m_Queue1v1.Remove(0);
            
            // Create match
            array<string> team1 = new array<string>();
            team1.Insert(player1.SteamId);
            
            array<string> team2 = new array<string>();
            team2.Insert(player2.SteamId);
            
            OnMatchFound(team1, team2, SwarmArenaQueueType.SOLO_1V1);
        }
    }
    
    // Try to create 2v2 solo match
    protected void TryMatch2v2Solo()
    {
        if (m_Queue2v2Solo.Count() >= 4)
        {
            array<string> team1 = new array<string>();
            array<string> team2 = new array<string>();
            
            // Pop 4 players
            for (int i = 0; i < 4; i++)
            {
                SwarmArenaQueueEntry entry = m_Queue2v2Solo[0];
                m_Queue2v2Solo.Remove(0);
                
                if (i < 2)
                    team1.Insert(entry.SteamId);
                else
                    team2.Insert(entry.SteamId);
            }
            
            OnMatchFound(team1, team2, SwarmArenaQueueType.SOLO_2V2);
        }
    }
    
    // Try to create 3v3 solo match
    protected void TryMatch3v3Solo()
    {
        if (m_Queue3v3Solo.Count() >= 6)
        {
            array<string> team1 = new array<string>();
            array<string> team2 = new array<string>();
            
            // Pop 6 players
            for (int i = 0; i < 6; i++)
            {
                SwarmArenaQueueEntry entry = m_Queue3v3Solo[0];
                m_Queue3v3Solo.Remove(0);
                
                if (i < 3)
                    team1.Insert(entry.SteamId);
                else
                    team2.Insert(entry.SteamId);
            }
            
            OnMatchFound(team1, team2, SwarmArenaQueueType.SOLO_3V3);
        }
    }
    
    // Try to create 2v2 party match
    protected void TryMatch2v2Party()
    {
        if (m_PartyQueue2v2.Count() >= 2)
        {
            SwarmArenaParty party1 = m_PartyQueue2v2[0];
            SwarmArenaParty party2 = m_PartyQueue2v2[1];
            
            m_PartyQueue2v2.Remove(0);
            m_PartyQueue2v2.Remove(0);
            
            party1.LeaveQueue();
            party2.LeaveQueue();
            
            // Create teams from party members
            array<string> team1 = new array<string>();
            foreach (string member1 : party1.MemberSteamIds)
            {
                team1.Insert(member1);
            }
            
            array<string> team2 = new array<string>();
            foreach (string member2 : party2.MemberSteamIds)
            {
                team2.Insert(member2);
            }
            
            OnMatchFound(team1, team2, SwarmArenaQueueType.PARTY_2V2);
        }
    }
    
    // Try to create 3v3 party match
    protected void TryMatch3v3Party()
    {
        if (m_PartyQueue3v3.Count() >= 2)
        {
            SwarmArenaParty party1 = m_PartyQueue3v3[0];
            SwarmArenaParty party2 = m_PartyQueue3v3[1];
            
            m_PartyQueue3v3.Remove(0);
            m_PartyQueue3v3.Remove(0);
            
            party1.LeaveQueue();
            party2.LeaveQueue();
            
            // Create teams from party members
            array<string> team1 = new array<string>();
            foreach (string member1 : party1.MemberSteamIds)
            {
                team1.Insert(member1);
            }
            
            array<string> team2 = new array<string>();
            foreach (string member2 : party2.MemberSteamIds)
            {
                team2.Insert(member2);
            }
            
            OnMatchFound(team1, team2, SwarmArenaQueueType.PARTY_3V3);
        }
    }
    
    // Called when a match is found
    protected void OnMatchFound(array<string> team1, array<string> team2, SwarmArenaQueueType queueType)
    {
        string queueName = typename.EnumToString(SwarmArenaQueueType, queueType);
        string logMsg = "Match found! " + queueName + " - Team1: " + team1.Count().ToString() + " vs Team2: " + team2.Count().ToString();
        SwarmArenaCore.Log(logMsg);
        
        // Update player data - no longer in queue
        foreach (string steamId : team1)
        {
            SwarmArenaPlayerData data = SwarmArenaLobbyManager.Get().GetPlayerData(steamId);
            if (data)
            {
                data.IsInQueue = false;
                data.QueueType = SwarmArenaQueueType.INVALID;
            }
            SwarmArenaLobbyManager.Get().LeaveLobby(steamId);
        }
        
        foreach (string steamId2 : team2)
        {
            SwarmArenaPlayerData data2 = SwarmArenaLobbyManager.Get().GetPlayerData(steamId2);
            if (data2)
            {
                data2.IsInQueue = false;
                data2.QueueType = SwarmArenaQueueType.INVALID;
            }
            SwarmArenaLobbyManager.Get().LeaveLobby(steamId2);
        }
        
        // Create match via InstanceManager (implemented in SwarmArenaMatch PBO)
        // SwarmArenaInstanceManager.Get().CreateMatch(team1, team2);
        
        // TODO: Notify all players that match was found
    }
    
    // Periodic update to check queues
    void Update(float timeslice)
    {
        m_CheckTimer += timeslice;
        
        if (m_CheckTimer >= CHECK_INTERVAL)
        {
            m_CheckTimer = 0;
            
            // Try matching all queue types
            TryMatch1v1();
            TryMatch2v2Solo();
            TryMatch3v3Solo();
            TryMatch2v2Party();
            TryMatch3v3Party();
            
            // Cleanup expired invites
            SwarmArenaPartyManager.Get().CleanupExpiredInvites();
        }
    }
    
    // Get queue counts for statistics
    int GetQueueCount(SwarmArenaQueueType queueType)
    {
        switch (queueType)
        {
            case SwarmArenaQueueType.SOLO_1V1:
                return m_Queue1v1.Count();
            case SwarmArenaQueueType.SOLO_2V2:
                return m_Queue2v2Solo.Count();
            case SwarmArenaQueueType.SOLO_3V3:
                return m_Queue3v3Solo.Count();
            case SwarmArenaQueueType.PARTY_2V2:
                return m_PartyQueue2v2.Count();
            case SwarmArenaQueueType.PARTY_3V3:
                return m_PartyQueue3v3.Count();
        }
        return 0;
    }
}

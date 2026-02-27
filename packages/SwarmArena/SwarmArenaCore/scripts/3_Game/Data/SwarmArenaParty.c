// SwarmArenaParty - Party with owner and members for group queuing

class SwarmArenaParty
{
    string OwnerSteamId;
    string OwnerName;
    ref array<string> MemberSteamIds = new array<string>();
    ref array<string> MemberNames = new array<string>();
    bool IsInQueue;
    SwarmArenaQueueType QueuedFor;
    int CreatedTime;
    
    static const int MAX_MEMBERS = 3;
    
    void SwarmArenaParty()
    {
        OwnerSteamId = "";
        OwnerName = "";
        IsInQueue = false;
        QueuedFor = SwarmArenaQueueType.INVALID;
        CreatedTime = 0;
    }
    
    void Create(string ownerSteamId, string ownerName)
    {
        OwnerSteamId = ownerSteamId;
        OwnerName = ownerName;
        MemberSteamIds.Clear();
        MemberSteamIds.Insert(ownerSteamId);
        MemberNames.Clear();
        MemberNames.Insert(ownerName);
        IsInQueue = false;
        QueuedFor = SwarmArenaQueueType.INVALID;
        CreatedTime = GetGame().GetTime();
    }
    
    bool AddMember(string steamId, string name)
    {
        if (MemberSteamIds.Count() >= MAX_MEMBERS)
        {
            return false;
        }
        
        if (HasMember(steamId))
        {
            return false;
        }
        
        MemberSteamIds.Insert(steamId);
        MemberNames.Insert(name);
        return true;
    }
    
    bool RemoveMember(string steamId)
    {
        int index = MemberSteamIds.Find(steamId);
        if (index >= 0)
        {
            MemberSteamIds.Remove(index);
            MemberNames.Remove(index);
            return true;
        }
        return false;
    }
    
    bool HasMember(string steamId)
    {
        return MemberSteamIds.Find(steamId) >= 0;
    }
    
    bool IsOwner(string steamId)
    {
        return OwnerSteamId == steamId;
    }
    
    int GetMemberCount()
    {
        return MemberSteamIds.Count();
    }
    
    bool IsFull()
    {
        return MemberSteamIds.Count() >= MAX_MEMBERS;
    }
    
    // Check if party can join a specific queue type
    bool CanJoinQueue(SwarmArenaQueueType queueType)
    {
        if (IsInQueue)
        {
            return false;
        }
        
        int memberCount = GetMemberCount();
        
        switch (queueType)
        {
            case SwarmArenaQueueType.SOLO_1V1:
                return memberCount == 1;
            case SwarmArenaQueueType.SOLO_2V2:
                return memberCount == 1;
            case SwarmArenaQueueType.SOLO_3V3:
                return memberCount == 1;
            case SwarmArenaQueueType.PARTY_2V2:
                return memberCount == 2;
            case SwarmArenaQueueType.PARTY_3V3:
                return memberCount == 3;
        }
        
        return false;
    }
    
    // Get valid queue types for this party size
    void GetValidQueueTypes(out array<SwarmArenaQueueType> validTypes)
    {
        validTypes = new array<SwarmArenaQueueType>();
        int memberCount = GetMemberCount();
        
        if (memberCount == 1)
        {
            validTypes.Insert(SwarmArenaQueueType.SOLO_1V1);
            validTypes.Insert(SwarmArenaQueueType.SOLO_2V2);
            validTypes.Insert(SwarmArenaQueueType.SOLO_3V3);
        }
        else if (memberCount == 2)
        {
            validTypes.Insert(SwarmArenaQueueType.PARTY_2V2);
        }
        else if (memberCount == 3)
        {
            validTypes.Insert(SwarmArenaQueueType.PARTY_3V3);
        }
    }
    
    void SetQueueState(bool inQueue, SwarmArenaQueueType queueType)
    {
        IsInQueue = inQueue;
        QueuedFor = queueType;
    }
    
    void LeaveQueue()
    {
        IsInQueue = false;
        QueuedFor = SwarmArenaQueueType.INVALID;
    }
}

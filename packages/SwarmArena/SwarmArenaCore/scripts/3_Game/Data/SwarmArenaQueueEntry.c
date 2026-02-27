// SwarmArenaQueueEntry - Player entry in matchmaking queue

class SwarmArenaQueueEntry
{
    string SteamId;
    string PlayerName;
    int JoinTime;           // Timestamp when joined queue
    SwarmArenaQueueType QueueType;
    
    void SwarmArenaQueueEntry()
    {
        SteamId = "";
        PlayerName = "";
        JoinTime = 0;
        QueueType = SwarmArenaQueueType.INVALID;
    }
    
    void Set(string steamId, string playerName, SwarmArenaQueueType queueType)
    {
        SteamId = steamId;
        PlayerName = playerName;
        QueueType = queueType;
        JoinTime = GetGame().GetTime();
    }
    
    int GetWaitTimeSeconds()
    {
        return (GetGame().GetTime() - JoinTime) / 1000;
    }
}

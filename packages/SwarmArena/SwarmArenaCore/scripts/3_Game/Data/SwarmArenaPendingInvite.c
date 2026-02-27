// SwarmArenaPendingInvite - Pending party invitation

class SwarmArenaPendingInvite
{
    string FromSteamId;
    string FromName;
    string ToSteamId;
    string ToName;
    int SentTime;
    
    static const int INVITE_TIMEOUT_MS = 60000; // 60 seconds
    
    void SwarmArenaPendingInvite()
    {
        FromSteamId = "";
        FromName = "";
        ToSteamId = "";
        ToName = "";
        SentTime = 0;
    }
    
    void Set(string fromSteamId, string fromName, string toSteamId, string toName)
    {
        FromSteamId = fromSteamId;
        FromName = fromName;
        ToSteamId = toSteamId;
        ToName = toName;
        SentTime = GetGame().GetTime();
    }
    
    bool IsExpired()
    {
        return (GetGame().GetTime() - SentTime) > INVITE_TIMEOUT_MS;
    }
    
    int GetRemainingTimeSeconds()
    {
        int elapsed = GetGame().GetTime() - SentTime;
        int remaining = INVITE_TIMEOUT_MS - elapsed;
        
        if (remaining < 0)
        {
            return 0;
        }
        
        return remaining / 1000;
    }
}

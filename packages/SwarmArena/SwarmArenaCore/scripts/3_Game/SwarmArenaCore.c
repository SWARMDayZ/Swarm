// SwarmArenaCore - Core module initialization
// This module provides shared functionality for all SwarmArena sub-modules

class SwarmArenaCore
{
    private static ref SwarmArenaCore s_Instance;
    
    static SwarmArenaCore GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaCore();
        }
        return s_Instance;
    }
    
    void SwarmArenaCore()
    {
        Print("[SwarmArenaCore] Core module initialized");
    }
    
    static void Log(string message)
    {
        Print("[SwarmArena] " + message);
    }
    
    static void LogError(string message)
    {
        Print("[SwarmArena] ERROR: " + message);
    }
    
    static void LogWarning(string message)
    {
        Print("[SwarmArena] WARNING: " + message);
    }
}

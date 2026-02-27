// SwarmArenaGUIInputHandler - Handles keybinds for opening arena menus

class SwarmArenaGUIInputHandler
{
    protected static ref SwarmArenaGUIInputHandler s_Instance;
    
    // State
    protected bool m_InLobby = false;
    protected bool m_InMatch = false;
    protected bool m_InFreezeTime = false;
    
    static SwarmArenaGUIInputHandler GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaGUIInputHandler();
        }
        return s_Instance;
    }
    
    static SwarmArenaGUIInputHandler Get()
    {
        return GetInstance();
    }
    
    // --- State setters ---
    
    void SetInLobby(bool inLobby)
    {
        m_InLobby = inLobby;
    }
    
    void SetInMatch(bool inMatch)
    {
        m_InMatch = inMatch;
    }
    
    void SetInFreezeTime(bool inFreezeTime)
    {
        m_InFreezeTime = inFreezeTime;
    }
    
    // --- Input handling ---
    // Called from MissionGameplay OnUpdate
    
    void OnUpdate(float timeslice)
    {
        Input input = GetGame().GetInput();
        if (!input)
        {
            return;
        }
        
        // V key - Lobby menu (only in lobby, not in match)
        if (input.LocalPress("UASwarmArenaLobbyMenu"))
        {
            if (m_InLobby && !m_InMatch)
            {
                ToggleLobbyMenu();
            }
        }
        
        // B key - Buy menu (only during freeze time in match)
        if (input.LocalPress("UASwarmArenaBuyMenu"))
        {
            if (m_InMatch && m_InFreezeTime)
            {
                ToggleBuyMenu();
            }
        }
    }
    
    protected void ToggleLobbyMenu()
    {
        UIManager uiMgr = GetGame().GetUIManager();
        if (!uiMgr)
        {
            return;
        }
        
        UIScriptedMenu currentMenu = uiMgr.GetMenu();
        
        if (currentMenu && currentMenu.GetID() == MENU_SWARMARENA_LOBBY)
        {
            // Close if already open
            currentMenu.Close();
        }
        else if (!currentMenu)
        {
            // Open lobby menu
            uiMgr.EnterScriptedMenu(MENU_SWARMARENA_LOBBY, null);
        }
    }
    
    protected void ToggleBuyMenu()
    {
        UIManager uiMgr = GetGame().GetUIManager();
        if (!uiMgr)
        {
            return;
        }
        
        UIScriptedMenu currentMenu = uiMgr.GetMenu();
        
        if (currentMenu && currentMenu.GetID() == MENU_SWARMARENA_BUY)
        {
            // Close if already open
            currentMenu.Close();
        }
        else if (!currentMenu)
        {
            // Open buy menu
            uiMgr.EnterScriptedMenu(MENU_SWARMARENA_BUY, null);
        }
    }
    
    // --- Menu registration ---
    // Call this from Mission init to register menu types
    
    static void RegisterMenus()
    {
        // Menus are registered via their class name and ID - no explicit registration needed in DayZ
        SwarmArenaCore.Log("Arena menus ready");
    }
}

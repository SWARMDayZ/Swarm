// SwarmArenaMatchHUD - In-match heads-up display

class SwarmArenaMatchHUD extends ScriptedWidgetEventHandler
{
    protected const string LAYOUT_PATH = "SwarmArenaGUI/data/layouts/arena_match_hud.layout";
    
    // Singleton
    protected static ref SwarmArenaMatchHUD s_Instance;
    
    // Root widget
    protected Widget m_Root;
    
    // Top panel widgets
    protected TextWidget m_RoundText;
    protected TextWidget m_TeamAScore;
    protected TextWidget m_TeamBScore;
    protected TextWidget m_TimerText;
    
    // Team player widgets
    protected ref array<TextWidget> m_TeamAPlayers = new array<TextWidget>();
    protected ref array<TextWidget> m_TeamBPlayers = new array<TextWidget>();
    
    // Money display
    protected TextWidget m_MoneyValue;
    
    // Buy prompt
    protected Widget m_BuyPrompt;
    
    // Spectator panel
    protected Widget m_SpectatorPanel;
    protected TextWidget m_SpectateTarget;
    
    // Announcements
    protected Widget m_AnnouncementPanel;
    protected TextWidget m_AnnouncementMain;
    protected TextWidget m_AnnouncementSub;
    
    // Kill feed
    protected Widget m_KillFeedList;
    protected ref array<Widget> m_KillFeedEntries = new array<Widget>();
    
    // State
    protected bool m_IsVisible = false;
    protected bool m_IsSpectating = false;
    protected float m_AnnouncementTimer = 0;
    
    // --- Singleton ---
    
    static SwarmArenaMatchHUD GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new SwarmArenaMatchHUD();
        }
        return s_Instance;
    }
    
    static SwarmArenaMatchHUD Get()
    {
        return GetInstance();
    }
    
    // --- Lifecycle ---
    
    void SwarmArenaMatchHUD()
    {
    }
    
    void ~SwarmArenaMatchHUD()
    {
        Hide();
        
        if (s_Instance == this)
        {
            s_Instance = null;
        }
    }
    
    void Init()
    {
        if (m_Root)
        {
            return;
        }
        
        m_Root = GetGame().GetWorkspace().CreateWidgets(LAYOUT_PATH);
        m_Root.SetHandler(this);
        
        // Find top panel widgets
        m_RoundText = TextWidget.Cast(m_Root.FindAnyWidget("RoundText"));
        m_TeamAScore = TextWidget.Cast(m_Root.FindAnyWidget("TeamAScore"));
        m_TeamBScore = TextWidget.Cast(m_Root.FindAnyWidget("TeamBScore"));
        m_TimerText = TextWidget.Cast(m_Root.FindAnyWidget("TimerText"));
        
        // Find team player widgets
        m_TeamAPlayers.Insert(TextWidget.Cast(m_Root.FindAnyWidget("TeamAPlayer1")));
        m_TeamAPlayers.Insert(TextWidget.Cast(m_Root.FindAnyWidget("TeamAPlayer2")));
        m_TeamAPlayers.Insert(TextWidget.Cast(m_Root.FindAnyWidget("TeamAPlayer3")));
        
        m_TeamBPlayers.Insert(TextWidget.Cast(m_Root.FindAnyWidget("TeamBPlayer1")));
        m_TeamBPlayers.Insert(TextWidget.Cast(m_Root.FindAnyWidget("TeamBPlayer2")));
        m_TeamBPlayers.Insert(TextWidget.Cast(m_Root.FindAnyWidget("TeamBPlayer3")));
        
        // Find money display
        m_MoneyValue = TextWidget.Cast(m_Root.FindAnyWidget("MoneyValue"));
        
        // Find buy prompt
        m_BuyPrompt = m_Root.FindAnyWidget("BuyPrompt");
        
        // Find spectator panel
        m_SpectatorPanel = m_Root.FindAnyWidget("SpectatorPanel");
        m_SpectateTarget = TextWidget.Cast(m_Root.FindAnyWidget("SpectateTarget"));
        
        // Find announcements
        m_AnnouncementPanel = m_Root.FindAnyWidget("AnnouncementPanel");
        m_AnnouncementMain = TextWidget.Cast(m_Root.FindAnyWidget("AnnouncementMain"));
        m_AnnouncementSub = TextWidget.Cast(m_Root.FindAnyWidget("AnnouncementSub"));
        
        // Find kill feed
        m_KillFeedList = m_Root.FindAnyWidget("KillFeedList");
        
        // Initial state
        m_SpectatorPanel.Show(false);
        m_BuyPrompt.Show(false);
        m_AnnouncementPanel.Show(false);
        
        m_Root.Show(false);
    }
    
    void Show()
    {
        if (!m_Root)
        {
            Init();
        }
        
        m_Root.Show(true);
        m_IsVisible = true;
    }
    
    void Hide()
    {
        if (m_Root)
        {
            m_Root.Show(false);
        }
        m_IsVisible = false;
    }
    
    bool IsVisible()
    {
        return m_IsVisible;
    }
    
    // --- Update ---
    
    void Update(float timeslice)
    {
        if (!m_IsVisible)
        {
            return;
        }
        
        // Handle announcement timeout
        if (m_AnnouncementTimer > 0)
        {
            m_AnnouncementTimer -= timeslice;
            if (m_AnnouncementTimer <= 0)
            {
                m_AnnouncementPanel.Show(false);
            }
        }
    }
    
    // --- State Updates ---
    
    void SetRound(int roundNumber)
    {
        if (m_RoundText)
        {
            m_RoundText.SetText("ROUND " + roundNumber.ToString());
        }
    }
    
    void SetScore(int teamAScore, int teamBScore)
    {
        if (m_TeamAScore)
        {
            m_TeamAScore.SetText(teamAScore.ToString());
        }
        if (m_TeamBScore)
        {
            m_TeamBScore.SetText(teamBScore.ToString());
        }
    }
    
    void SetTimer(int seconds)
    {
        if (m_TimerText)
        {
            int mins = seconds / 60;
            int secs = seconds % 60;
            string secStr = secs.ToString();
            if (secs < 10) secStr = "0" + secStr;
            string timeStr = mins.ToString() + ":" + secStr;
            m_TimerText.SetText(timeStr);
        }
    }
    
    void SetMoney(int amount)
    {
        if (m_MoneyValue)
        {
            m_MoneyValue.SetText(amount.ToString());
            m_MoneyValue.SetColor(SwarmArenaGUIColors.GetMoneyColor(amount));
        }
    }
    
    void SetTeamAPlayers(array<string> names, array<bool> alive)
    {
        for (int i = 0; i < m_TeamAPlayers.Count(); i++)
        {
            TextWidget widget = m_TeamAPlayers[i];
            if (widget)
            {
                if (i < names.Count())
                {
                    widget.SetText(names[i]);
                    bool isAlive = i < alive.Count() && alive[i];
                    widget.SetColor(SwarmArenaGUIColors.GetStatusColor(isAlive));
                }
                else
                {
                    widget.SetText("");
                }
            }
        }
    }
    
    void SetTeamBPlayers(array<string> names, array<bool> alive)
    {
        for (int i = 0; i < m_TeamBPlayers.Count(); i++)
        {
            TextWidget widget = m_TeamBPlayers[i];
            if (widget)
            {
                if (i < names.Count())
                {
                    widget.SetText(names[i]);
                    bool isAlive = i < alive.Count() && alive[i];
                    widget.SetColor(SwarmArenaGUIColors.GetStatusColor(isAlive));
                }
                else
                {
                    widget.SetText("");
                }
            }
        }
    }
    
    void SetBuyPromptVisible(bool visible)
    {
        if (m_BuyPrompt)
        {
            m_BuyPrompt.Show(visible);
        }
    }
    
    // --- Spectator ---
    
    void SetSpectating(bool spectating, string targetName = "", int targetIndex = 0, int totalTargets = 0)
    {
        m_IsSpectating = spectating;
        
        if (m_SpectatorPanel)
        {
            m_SpectatorPanel.Show(spectating);
        }
        
        if (spectating && m_SpectateTarget)
        {
            string indexStr = "(" + (targetIndex + 1).ToString() + "/" + totalTargets.ToString() + ")";
            m_SpectateTarget.SetText(targetName + " " + indexStr);
        }
    }
    
    // --- Announcements ---
    
    void ShowAnnouncement(string mainText, string subText = "", float duration = 3.0)
    {
        if (m_AnnouncementMain)
        {
            m_AnnouncementMain.SetText(mainText);
        }
        if (m_AnnouncementSub)
        {
            m_AnnouncementSub.SetText(subText);
        }
        if (m_AnnouncementPanel)
        {
            m_AnnouncementPanel.Show(true);
        }
        
        m_AnnouncementTimer = duration;
    }
    
    void ShowRoundStart(int roundNumber)
    {
        ShowAnnouncement("ROUND " + roundNumber.ToString(), "FIGHT!", 2.0);
    }
    
    void ShowRoundWin(bool yourTeamWon)
    {
        if (yourTeamWon)
        {
            ShowAnnouncement("ROUND WON", "", 2.5);
        }
        else
        {
            ShowAnnouncement("ROUND LOST", "", 2.5);
        }
    }
    
    void ShowMatchWin(bool yourTeamWon, int teamAScore, int teamBScore)
    {
        string scoreStr = teamAScore.ToString() + " - " + teamBScore.ToString();
        
        if (yourTeamWon)
        {
            ShowAnnouncement("VICTORY", scoreStr, 5.0);
        }
        else
        {
            ShowAnnouncement("DEFEAT", scoreStr, 5.0);
        }
    }
    
    void ShowFreezeTime(int seconds)
    {
        ShowAnnouncement("BUY TIME", seconds.ToString() + " seconds", 1.5);
    }
    
    // --- Kill Feed ---
    
    void AddKillFeedEntry(string killerName, string victimName, string weaponName)
    {
        // TODO: Create kill feed entry widget dynamically
        // Format: "KillerName [weapon] VictimName"
        
        SwarmArenaCore.Log("Kill: " + killerName + " -> " + victimName + " with " + weaponName);
        
        // Limit to 5 entries
        while (m_KillFeedEntries.Count() >= 5)
        {
            Widget oldest = m_KillFeedEntries[0];
            m_KillFeedEntries.Remove(0);
            if (oldest)
            {
                oldest.Unlink();
            }
        }
    }
}

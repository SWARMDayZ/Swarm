// SwarmArenaLobbyMenu - Main lobby interface for queue and party management

class SwarmArenaLobbyMenu extends UIScriptedMenu
{
    // Layout path
    protected const string LAYOUT_PATH = "SwarmArenaGUI/data/layouts/arena_lobby_menu.layout";
    
    // UI widgets
    protected Widget m_Root;
    
    // Queue buttons
    protected ButtonWidget m_Btn1v1;
    protected ButtonWidget m_Btn2v2;
    protected ButtonWidget m_Btn3v3;
    protected ButtonWidget m_BtnLeaveQueue;
    protected TextWidget m_QueueStatus;
    
    // Player info
    protected TextWidget m_PlayerName;
    protected TextWidget m_ValBalance;
    protected TextWidget m_ValWins;
    protected TextWidget m_ValLosses;
    protected TextWidget m_ValKills;
    
    // Party widgets
    protected TextWidget m_Member1;
    protected TextWidget m_Member2;
    protected TextWidget m_Member3;
    protected EditBoxWidget m_InviteInput;
    protected ButtonWidget m_BtnInvite;
    protected ButtonWidget m_BtnLeaveParty;
    
    // Invite panel
    protected Widget m_InvitesPanel;
    protected TextWidget m_InviteText;
    protected ButtonWidget m_BtnAcceptInvite;
    protected ButtonWidget m_BtnDeclineInvite;
    
    // Navigation
    protected ButtonWidget m_BtnPresetEditor;
    protected ButtonWidget m_BtnClose;
    
    // State
    protected bool m_IsInQueue = false;
    protected int m_CurrentQueueType = -1;
    protected ref SwarmArenaPendingInvite m_PendingInvite;
    
    // --- Lifecycle ---
    
    void SwarmArenaLobbyMenu()
    {
    }
    
    void ~SwarmArenaLobbyMenu()
    {
    }
    
    override Widget Init()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets(LAYOUT_PATH);
        
        // Find queue widgets
        m_Btn1v1 = ButtonWidget.Cast(m_Root.FindAnyWidget("Btn1v1"));
        m_Btn2v2 = ButtonWidget.Cast(m_Root.FindAnyWidget("Btn2v2"));
        m_Btn3v3 = ButtonWidget.Cast(m_Root.FindAnyWidget("Btn3v3"));
        m_BtnLeaveQueue = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnLeaveQueue"));
        m_QueueStatus = TextWidget.Cast(m_Root.FindAnyWidget("QueueStatus"));
        
        // Find player info widgets
        m_PlayerName = TextWidget.Cast(m_Root.FindAnyWidget("PlayerName"));
        m_ValBalance = TextWidget.Cast(m_Root.FindAnyWidget("ValBalance"));
        m_ValWins = TextWidget.Cast(m_Root.FindAnyWidget("ValWins"));
        m_ValLosses = TextWidget.Cast(m_Root.FindAnyWidget("ValLosses"));
        m_ValKills = TextWidget.Cast(m_Root.FindAnyWidget("ValKills"));
        
        // Find party widgets
        m_Member1 = TextWidget.Cast(m_Root.FindAnyWidget("Member1"));
        m_Member2 = TextWidget.Cast(m_Root.FindAnyWidget("Member2"));
        m_Member3 = TextWidget.Cast(m_Root.FindAnyWidget("Member3"));
        m_InviteInput = EditBoxWidget.Cast(m_Root.FindAnyWidget("InviteInput"));
        m_BtnInvite = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnInvite"));
        m_BtnLeaveParty = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnLeaveParty"));
        
        // Find invite widgets
        m_InvitesPanel = m_Root.FindAnyWidget("InvitesPanel");
        m_InviteText = TextWidget.Cast(m_Root.FindAnyWidget("InviteText"));
        m_BtnAcceptInvite = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnAcceptInvite"));
        m_BtnDeclineInvite = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnDeclineInvite"));
        
        // Find navigation
        m_BtnPresetEditor = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnPresetEditor"));
        m_BtnClose = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnClose"));
        
        // Initial state
        m_BtnLeaveQueue.Show(false);
        m_InvitesPanel.Show(false);
        
        RefreshUI();
        
        return m_Root;
    }
    
    override void OnShow()
    {
        super.OnShow();
        
        GetGame().GetInput().ChangeGameFocus(1);
        GetGame().GetUIManager().ShowCursor(true);
        
        RefreshUI();
    }
    
    override void OnHide()
    {
        super.OnHide();
        
        GetGame().GetInput().ChangeGameFocus(-1);
        GetGame().GetUIManager().ShowCursor(false);
    }
    
    // --- UI Refresh ---
    
    void RefreshUI()
    {
        RefreshPlayerInfo();
        RefreshPartyInfo();
        RefreshQueueState();
        RefreshInvites();
    }
    
    protected void RefreshPlayerInfo()
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player || !player.GetIdentity())
        {
            return;
        }
        
        m_PlayerName.SetText(player.GetIdentity().GetName());
        
        // Get player data from lobby manager
        // TODO: This would come from actual player data
        m_ValBalance.SetText("$800");
        m_ValWins.SetText("0");
        m_ValLosses.SetText("0");
        m_ValKills.SetText("0");
    }
    
    protected void RefreshPartyInfo()
    {
        // Clear member slots
        m_Member1.SetText("");
        m_Member2.SetText("");
        m_Member3.SetText("");
        
        // TODO: Get actual party info from SwarmArenaPartyManager
        // For now, show player as solo
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player && player.GetIdentity())
        {
            m_Member1.SetText("1. " + player.GetIdentity().GetName() + " (Leader)");
        }
        
        // Update party button visibility
        m_BtnLeaveParty.Show(false); // Hide if solo
    }
    
    protected void RefreshQueueState()
    {
        if (m_IsInQueue)
        {
            string queueName;
            switch (m_CurrentQueueType)
            {
                case SwarmArenaQueueType.SOLO_1V1: queueName = "1v1"; break;
                case SwarmArenaQueueType.SOLO_2V2: queueName = "2v2"; break;
                case SwarmArenaQueueType.SOLO_3V3: queueName = "3v3"; break;
            }
            m_QueueStatus.SetText("In queue: " + queueName + " ...");
            m_BtnLeaveQueue.Show(true);
            
            // Disable queue buttons
            m_Btn1v1.Enable(false);
            m_Btn2v2.Enable(false);
            m_Btn3v3.Enable(false);
        }
        else
        {
            m_QueueStatus.SetText("Not in queue");
            m_BtnLeaveQueue.Show(false);
            
            // Enable appropriate queue buttons based on party size
            m_Btn1v1.Enable(true); // Solo only
            m_Btn2v2.Enable(true);
            m_Btn3v3.Enable(true);
        }
    }
    
    protected void RefreshInvites()
    {
        if (m_PendingInvite)
        {
            m_InviteText.SetText("Party invite from: " + m_PendingInvite.FromName);
            m_InvitesPanel.Show(true);
        }
        else
        {
            m_InvitesPanel.Show(false);
        }
    }
    
    // --- Button Handlers ---
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_Btn1v1)
        {
            OnQueue1v1Click();
            return true;
        }
        if (w == m_Btn2v2)
        {
            OnQueue2v2Click();
            return true;
        }
        if (w == m_Btn3v3)
        {
            OnQueue3v3Click();
            return true;
        }
        if (w == m_BtnLeaveQueue)
        {
            OnLeaveQueueClick();
            return true;
        }
        if (w == m_BtnInvite)
        {
            OnInviteClick();
            return true;
        }
        if (w == m_BtnLeaveParty)
        {
            OnLeavePartyClick();
            return true;
        }
        if (w == m_BtnAcceptInvite)
        {
            OnAcceptInviteClick();
            return true;
        }
        if (w == m_BtnDeclineInvite)
        {
            OnDeclineInviteClick();
            return true;
        }
        if (w == m_BtnPresetEditor)
        {
            OnPresetEditorClick();
            return true;
        }
        if (w == m_BtnClose)
        {
            Close();
            return true;
        }
        
        return false;
    }
    
    protected void OnQueue1v1Click()
    {
        SendQueueRequest(SwarmArenaQueueType.SOLO_1V1);
    }
    
    protected void OnQueue2v2Click()
    {
        SendQueueRequest(SwarmArenaQueueType.SOLO_2V2);
    }
    
    protected void OnQueue3v3Click()
    {
        SendQueueRequest(SwarmArenaQueueType.SOLO_3V3);
    }
    
    protected void OnLeaveQueueClick()
    {
        SendLeaveQueueRequest();
    }
    
    protected void OnInviteClick()
    {
        string playerName = m_InviteInput.GetText();
        if (playerName != "")
        {
            SendPartyInvite(playerName);
            m_InviteInput.SetText("");
        }
    }
    
    protected void OnLeavePartyClick()
    {
        SendLeavePartyRequest();
    }
    
    protected void OnAcceptInviteClick()
    {
        if (m_PendingInvite)
        {
            SendAcceptInvite(m_PendingInvite.FromSteamId);
            m_PendingInvite = null;
            RefreshInvites();
        }
    }
    
    protected void OnDeclineInviteClick()
    {
        if (m_PendingInvite)
        {
            SendDeclineInvite(m_PendingInvite.FromSteamId);
            m_PendingInvite = null;
            RefreshInvites();
        }
    }
    
    protected void OnPresetEditorClick()
    {
        // Close this menu and open preset editor
        Close();
        GetGame().GetUIManager().EnterScriptedMenu(MENU_SWARMARENA_PRESET_EDITOR, null);
    }
    
    // --- RPC Sending ---
    
    protected void SendQueueRequest(int queueType)
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(queueType);
        rpc.Send(null, SwarmArenaRPC.Lobby_JoinQueue, true, null);
        
        m_IsInQueue = true;
        m_CurrentQueueType = queueType;
        RefreshQueueState();
    }
    
    protected void SendLeaveQueueRequest()
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Send(null, SwarmArenaRPC.Lobby_LeaveQueue, true, null);
        
        m_IsInQueue = false;
        m_CurrentQueueType = -1;
        RefreshQueueState();
    }
    
    protected void SendPartyInvite(string playerName)
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(playerName);
        rpc.Send(null, SwarmArenaRPC.Party_Invite, true, null);
    }
    
    protected void SendLeavePartyRequest()
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Send(null, SwarmArenaRPC.Party_Leave, true, null);
    }
    
    protected void SendAcceptInvite(string fromSteamId)
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(fromSteamId);
        rpc.Send(null, SwarmArenaRPC.Party_Accept, true, null);
    }
    
    protected void SendDeclineInvite(string fromSteamId)
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(fromSteamId);
        rpc.Send(null, SwarmArenaRPC.Party_Decline, true, null);
    }
    
    // --- External updates ---
    
    void SetPendingInvite(SwarmArenaPendingInvite invite)
    {
        m_PendingInvite = invite;
        RefreshInvites();
    }
    
    void OnQueueStateChanged(bool inQueue, int queueType)
    {
        m_IsInQueue = inQueue;
        m_CurrentQueueType = queueType;
        RefreshQueueState();
    }
}

// Menu type constant
const int MENU_SWARMARENA_LOBBY = 45001;
const int MENU_SWARMARENA_PRESET_EDITOR = 45002;
const int MENU_SWARMARENA_BUY = 45003;

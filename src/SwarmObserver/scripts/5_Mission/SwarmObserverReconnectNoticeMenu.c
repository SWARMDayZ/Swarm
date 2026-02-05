class SwarmObserverReconnectNoticeMenu extends UIScriptedMenu
{
	private TextWidget m_TitleText;
	private MultilineTextWidget m_MessageText;
	private ButtonWidget m_OkButton;
	private int m_ViolationCount;
	
	void SwarmObserverReconnectNoticeMenu()
	{
		// Add input excludes and hide HUD (like WarningMenuBase)
		if (GetGame().GetMission())
		{
			GetGame().GetMission().AddActiveInputExcludes({"menu"});
			GetGame().GetMission().GetHud().ShowHudUI(false);
			GetGame().GetMission().GetHud().ShowQuickbarUI(false);
		}
	}
	
	void ~SwarmObserverReconnectNoticeMenu()
	{
		// Remove input excludes and show HUD (like WarningMenuBase)
		if (GetGame() && GetGame().GetMission())
		{
			GetGame().GetMission().RemoveActiveInputExcludes({"menu"}, true);
			GetGame().GetMission().GetHud().ShowHudUI(true);
			GetGame().GetMission().GetHud().ShowQuickbarUI(true);
		}
	}
	
	void SetViolationCount(int count)
	{
		m_ViolationCount = count;
		UpdateMessage();
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("SwarmObserver/data/reconnect_notice.layout");
		
		// Get widget references
		m_TitleText = TextWidget.Cast(layoutRoot.FindAnyWidget("TitleText"));
		m_MessageText = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("MessageText"));
		m_OkButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("OkButton"));
		
		// Set title
		if (m_TitleText)
			m_TitleText.SetText(Widget.TranslateString("#STR_SWARM_OBSERVER_NOTICE_TITLE"));
		
		// Read violation count from PlayerBase static variable
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			m_ViolationCount = PlayerBase.GetSwarmObserverViolationCount();
			UpdateMessage();
		}
		
		return layoutRoot;
	}
	
	private void UpdateMessage()
	{
		if (m_MessageText)
		{
			string message = Widget.TranslateString("#STR_SWARM_OBSERVER_NOTICE_MESSAGE_1") + "\n\n";
			message += Widget.TranslateString("#STR_SWARM_OBSERVER_NOTICE_MESSAGE_2") + "\n\n";
			
			// Use singular or plural form based on count
			if (m_ViolationCount == 1)
			{
				message += Widget.TranslateString("#STR_SWARM_OBSERVER_NOTICE_MESSAGE_3_SINGLE");
			}
			else
			{
				string pluralMessage = Widget.TranslateString("#STR_SWARM_OBSERVER_NOTICE_MESSAGE_3_PLURAL");
				pluralMessage.Replace("%1", m_ViolationCount.ToString());
				message += pluralMessage;
			}
			
			m_MessageText.SetText(message);
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_OkButton)
		{
			OnOkClicked();
			return true;
		}
		
		return false;
	}
	
	private void OnOkClicked()
	{
		Print("[SwarmObserver] Player acknowledged reconnect notice");
		
		// Send acknowledgement RPC to server
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			GetGame().RPCSingleParam(player, SwarmObserverRPC.RPC_SWARM_OBSERVER_ACKNOWLEDGE_NOTICE, null, true, player.GetIdentity());
		}
		
		Close();
	}
}

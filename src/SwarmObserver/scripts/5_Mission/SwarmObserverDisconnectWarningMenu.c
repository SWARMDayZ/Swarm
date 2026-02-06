class SwarmObserverDisconnectWarningMenu extends UIScriptedMenu
{	
	private TextWidget m_TitleText;
	private MultilineTextWidget m_MessageText;
	private ButtonWidget m_bOk;
	private ButtonWidget m_bCancel;
	private int m_iSafetyTime;
	private bool m_bSafetyEnabled;
	
	void SwarmObserverDisconnectWarningMenu()
	{
		m_iSafetyTime = 10;
		m_bSafetyEnabled = false;
	}
	
	void ~SwarmObserverDisconnectWarningMenu()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.UpdateSafetyTimer);
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("SwarmObserver/data/disconnect_warning.layout");
		
		m_TitleText = TextWidget.Cast(layoutRoot.FindAnyWidget("TitleText"));
		m_MessageText = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("MessageText"));
		m_bOk = ButtonWidget.Cast(layoutRoot.FindAnyWidget("OkButton"));
		m_bCancel = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CancelButton"));
		
		UpdateContent();
		UpdateSafetyButton();
		
		// Start safety timer countdown
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.UpdateSafetyTimer, 1000, true);
		
		return layoutRoot;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_bOk && m_bSafetyEnabled)
		{
			Confirm();
			return true;
		}
		else if (w == m_bCancel)
		{
			Cancel();
			return true;
		}

		return false;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Cancel();
		}
	}
	
	void UpdateContent()
	{
		if (m_TitleText)
			m_TitleText.SetText(Widget.TranslateString("#STR_SWARM_OBSERVER_WARNING_TITLE"));
		
		if (m_MessageText)
		{
			string message = Widget.TranslateString("#STR_SWARM_OBSERVER_WARNING_MESSAGE_1") + "\n\n";
			message += Widget.TranslateString("#STR_SWARM_OBSERVER_WARNING_MESSAGE_2") + "\n\n";
			message += Widget.TranslateString("#STR_SWARM_OBSERVER_WARNING_MESSAGE_3");
			m_MessageText.SetText(message);
		}
	}
	
	void UpdateSafetyTimer()
	{
		if (m_iSafetyTime > 0)
		{
			m_iSafetyTime--;
			UpdateSafetyButton();
		}
		else if (!m_bSafetyEnabled)
		{
			m_bSafetyEnabled = true;
			UpdateSafetyButton();
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.UpdateSafetyTimer);
		}
	}
	
	void UpdateSafetyButton()
	{
		if (!m_bOk)
			return;
			
		if (m_bSafetyEnabled)
		{
			m_bOk.Enable(true);
			m_bOk.SetText(Widget.TranslateString("#STR_SWARM_OBSERVER_BUTTON_OK"));
		}
		else
		{
			m_bOk.Enable(false);
			m_bOk.SetText(Widget.TranslateString("#STR_SWARM_OBSERVER_BUTTON_OK") + " (" + m_iSafetyTime.ToString() + ")");
		}
	}

	void Confirm()
	{
		Print("[SwarmObserver] Player confirmed disconnect from restricted area");
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.UpdateSafetyTimer);
		
		// Get parent menu reference BEFORE closing this menu
		UIScriptedMenu parentMenu = UIScriptedMenu.Cast(GetParentMenu());
		
		// Close this warning menu first
		Close();
		
		// Close the parent InGameMenu if it exists
		if (parentMenu)
		{
			parentMenu.Close();
		}
		
		// Open logout menu
		GetGame().GetUIManager().EnterScriptedMenu(SwarmObserverConstants.MENU_SWARM_OBSERVER_LOGOUT, null);
	}
	
	void Cancel()
	{
		Print("[SwarmObserver] Player cancelled disconnect from restricted area");
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.UpdateSafetyTimer);
		
		// Close warning - InGameMenu (parent) becomes active again
		Close();
	}
}

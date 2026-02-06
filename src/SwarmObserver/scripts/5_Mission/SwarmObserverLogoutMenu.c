class SwarmObserverLogoutMenu extends UIScriptedMenu
{	
	private TextWidget m_LogoutTimeText;
	private TextWidget m_DescriptionText;
	private ButtonWidget m_bLogoutNow;
	private ButtonWidget m_bCancel;
	private int m_iTime;
	private ref FullTimeData m_FullTime;

	void SwarmObserverLogoutMenu()
	{
		m_iTime = 15; // Default logout time in seconds
		g_Game.SetKeyboardHandle(this);
		m_FullTime = new FullTimeData();
	}

	void ~SwarmObserverLogoutMenu()
	{
		g_Game.SetKeyboardHandle(null);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
		
		// Re-enable player controls (in case menu is closed unexpectedly)
		if (GetGame().GetMission())
		{
			GetGame().GetMission().PlayerControlEnable(true);
		}
		GetGame().GetUIManager().ShowUICursor(false);
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager()) 
		{
			player.GetEmoteManager().SetClientLoggingOut(false);
		}
	}
	
	override Widget Init()
	{
		// Use vanilla logout dialog layout
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_logout_dialog.layout");
		
		m_LogoutTimeText = TextWidget.Cast(layoutRoot.FindAnyWidget("txtLogoutTime"));
		m_DescriptionText = TextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
		m_bLogoutNow = ButtonWidget.Cast(layoutRoot.FindAnyWidget("bLogoutNow"));
		m_bCancel = ButtonWidget.Cast(layoutRoot.FindAnyWidget("bCancel"));
		
		#ifdef PLATFORM_CONSOLE
		ButtonWidget cancelConsole = ButtonWidget.Cast(layoutRoot.FindAnyWidget("bCancelConsole"));
		if (cancelConsole) cancelConsole.Show(false);
		if (m_bCancel) m_bCancel.Show(false);
		if (m_bLogoutNow) m_bLogoutNow.Show(false);
		#else
		if (m_bCancel) m_bCancel.Show(true);
		if (m_bLogoutNow) m_bLogoutNow.Show(true);
		Widget toolbar = layoutRoot.FindAnyWidget("toolbar_bg");
		if (toolbar) toolbar.Show(false);
		#endif
		
		UpdateInfo();
		
		// Player should sit down if possible
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().SetClientLoggingOut(true);
		}
		
		// Lock controls while this menu is open
		if (GetGame().GetMission())
			GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		GetGame().GetUIManager().ShowUICursor(true);
		
		// Start with fixed logout time and begin countdown immediately
		SetTime(m_iTime);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.UpdateTime, 1000, true);
		
		return layoutRoot;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_bLogoutNow)
		{
			Exit();
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
		
		// Check for ESC/Back - consume the input to prevent InGameMenu from opening
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Cancel();
			// Input is consumed by closing the menu
		}
	}
	
	// Prevent ESC from opening other menus while we're active
	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		super.OnKeyPress(w, x, y, key);
		
		// Block all key presses while this menu is open
		return true;
	}
	
	void SetTime(int time)
	{
		m_iTime = time;
		string text = "#layout_logout_dialog_until_logout_";

		TimeConversions.ConvertSecondsToFullTime(time, m_FullTime);
		
		if (m_FullTime.m_Days > 0)
			text += "dhms";
		else if (m_FullTime.m_Hours > 0)
			text += "hms";
		else if (m_FullTime.m_Minutes > 0)
			text += "ms";
		else
			text += "s";
		
		text = Widget.TranslateString(text);
		text = string.Format(text, m_FullTime.m_Seconds, m_FullTime.m_Minutes, m_FullTime.m_Hours, m_FullTime.m_Days);
		
		if (m_LogoutTimeText)
			m_LogoutTimeText.SetText(text);
	}
		
	void UpdateTime()
	{
		if (m_iTime > 0)
		{
			SetTime(--m_iTime);
		}
		else
		{
			Exit();
		}
	}
	
	void UpdateInfo()
	{
		if (!m_DescriptionText)
			return;
			
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && (player.IsRestrained() || player.IsUnconscious()))
		{
			m_DescriptionText.SetText("#layout_logout_dialog_note_killed");
		}
		else
		{
			m_DescriptionText.SetText("#layout_logout_dialog_note");
		}
	}

	void Exit()
	{		
		// Stop countdown
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
		
		// Go back to main menu
		GetGame().GetMission().AbortMission();
	}

	void Cancel()
	{
		Print("[SwarmObserver] Logout cancelled");
		
		// Stop countdown
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
		
		// Re-enable player controls
		GetGame().GetMission().PlayerControlEnable(true);
		GetGame().GetUIManager().ShowUICursor(false);
		
		// Close this menu
		Close();
	}
}

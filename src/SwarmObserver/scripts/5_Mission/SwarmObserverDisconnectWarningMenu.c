class SwarmObserverDisconnectWarningMenu extends UIScriptedMenu
{
	private TextWidget m_TitleText;
	private MultilineTextWidget m_MessageText;
	private ButtonWidget m_OkButton;
	private ButtonWidget m_CancelButton;
	private int m_OkButtonEnableTime;
	private bool m_OkButtonEnabled;
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("SwarmObserver/data/disconnect_warning.layout");
		
		// Get widget references
		m_TitleText = TextWidget.Cast(layoutRoot.FindAnyWidget("TitleText"));
		m_MessageText = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("MessageText"));
		m_OkButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("OkButton"));
		m_CancelButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CancelButton"));
		
		// Set content from localized strings
		if (m_TitleText)
			m_TitleText.SetText(Widget.TranslateString("#STR_SWARM_OBSERVER_WARNING_TITLE"));
		
		if (m_MessageText)
		{
			string message = Widget.TranslateString("#STR_SWARM_OBSERVER_WARNING_MESSAGE_1") + "\n\n";
			message += Widget.TranslateString("#STR_SWARM_OBSERVER_WARNING_MESSAGE_2") + "\n\n";
			message += Widget.TranslateString("#STR_SWARM_OBSERVER_WARNING_MESSAGE_3");
			m_MessageText.SetText(message);
		}
		
		// Disable OK button for 10 seconds
		m_OkButtonEnabled = false;
		m_OkButtonEnableTime = GetGame().GetTime() + 10000; // 10 seconds
		
		if (m_OkButton)
		{
			m_OkButton.Enable(false);
			m_OkButton.SetText(Widget.TranslateString("#STR_SWARM_OBSERVER_BUTTON_OK") + " (10)");
		}
		
		return layoutRoot;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		// Update OK button countdown
		if (!m_OkButtonEnabled && m_OkButton)
		{
			int currentTime = GetGame().GetTime();
			int remainingTime = (m_OkButtonEnableTime - currentTime) / 1000;
			
			if (remainingTime <= 0)
			{
				m_OkButtonEnabled = true;
				m_OkButton.Enable(true);
				m_OkButton.SetText(Widget.TranslateString("#STR_SWARM_OBSERVER_BUTTON_OK"));
			}
			else
			{
				m_OkButton.SetText(Widget.TranslateString("#STR_SWARM_OBSERVER_BUTTON_OK") + " (" + remainingTime + ")");
			}
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_OkButton && m_OkButtonEnabled)
		{
			// Player confirmed disconnect
			OnOkClicked();
			return true;
		}
		else if (w == m_CancelButton)
		{
			// Player cancelled disconnect
			OnCancelClicked();
			return true;
		}
		
		return false;
	}
	
	private void OnOkClicked()
	{
		Print("[SwarmObserver] Player confirmed disconnect from restricted area");
		
		// Close menu
		Close();
		
		// Proceed with disconnect
		GetGame().GetMission().AbortMission();
	}
	
	private void OnCancelClicked()
	{
		Print("[SwarmObserver] Player cancelled disconnect from restricted area");
		
		// Close menu
		Close();
	}
}

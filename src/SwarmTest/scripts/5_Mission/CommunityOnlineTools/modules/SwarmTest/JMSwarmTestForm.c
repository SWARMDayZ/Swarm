// SwarmTest Form for Community Online Tools

// Wrapper class to store animation data in UIActionButtons
class SwarmTestAnimUIData : UIActionData
{
	ref SwarmTestAnimInfo AnimInfo;
	
	void SwarmTestAnimUIData(SwarmTestAnimInfo animInfo)
	{
		AnimInfo = animInfo;
	}
}

class JMSwarmTestForm: JMFormBase
{
	protected UIActionScroller m_Scroller;
	protected Widget m_ActionsParent;
	protected UIActionEditableText m_SearchBox;
	protected ref ScriptInvoker m_OnSearchChanged;
	
	protected Widget m_AnimationWrapper;
	protected ref array<ref UIActionButton> m_AnimationButtons;
	protected ref array<ref SwarmTestAnimInfo> m_AllAnimations;
	protected ref array<ref SwarmTestAnimInfo> m_FilteredAnimations;
	
	protected int m_CurrentPage = 0;
	protected int m_AnimsPerPage = 20;
	
	protected UIActionButton m_PrevPageButton;
	protected UIActionButton m_NextPageButton;
	protected UIActionText m_PageInfo;
	protected UIActionButton m_StopButton;
	
	protected JMSwarmTestModule m_Module;

	void JMSwarmTestForm()
	{
		m_AllAnimations = SwarmTestConstants.GetAllAnimations();
		m_FilteredAnimations = new array<ref SwarmTestAnimInfo>;
		m_AnimationButtons = new array<ref UIActionButton>;
		m_OnSearchChanged = new ScriptInvoker();
		
		Print("[SwarmTest] Loaded " + m_AllAnimations.Count() + " animations");
	}

	protected override bool SetModule(JMRenderableModuleBase mdl)
	{
		return Class.CastTo(m_Module, mdl);
	}

	override void OnInit()
	{
		m_Scroller = UIActionManager.CreateScroller(layoutRoot.FindAnyWidget("panel"));
		m_ActionsParent = m_Scroller.GetContentWidget();
		
		// Search box
		m_SearchBox = UIActionManager.CreateEditableText(m_ActionsParent, "Search:", this, "OnChange_Search");
		
		// Animation wrapper - will hold the buttons
		m_AnimationWrapper = UIActionManager.CreateActionRows(m_ActionsParent);
		
		// Navigation
		Widget navGrid = UIActionManager.CreateGridSpacer(m_ActionsParent, 1, 3);
		m_PrevPageButton = UIActionManager.CreateButton(navGrid, "Previous", this, "OnClick_PrevPage");
		m_PageInfo = UIActionManager.CreateText(navGrid, "", "");
		m_NextPageButton = UIActionManager.CreateButton(navGrid, "Next", this, "OnClick_NextPage");
		
		// Stop button
		m_StopButton = UIActionManager.CreateButton(m_ActionsParent, "Stop Current Animation", this, "OnClick_StopAnimation");
		
		RefreshAnimationList();
	}

	void RefreshAnimationList()
	{
		m_FilteredAnimations.Clear();
		
		string searchTerm = "";
		if (m_SearchBox)
		{
			searchTerm = m_SearchBox.GetText();
			searchTerm.ToLower();
		}
		
		Print("[SwarmTest] Refreshing list - Search: " + searchTerm);
		
		// Filter animations
		foreach (SwarmTestAnimInfo anim: m_AllAnimations)
		{
			// Filter by search term
			if (searchTerm != "")
			{
				string animName = anim.m_Name;
				animName.ToLower();
				string animDesc = anim.m_Description;
				animDesc.ToLower();
				
				if (!animName.Contains(searchTerm) && !animDesc.Contains(searchTerm))
					continue;
			}
			
			m_FilteredAnimations.Insert(anim);
		}
		
		Print("[SwarmTest] Filtered: " + m_FilteredAnimations.Count() + " / " + m_AllAnimations.Count() + " animations");
		
		// Reset to first page if we have fewer results
		int maxPages = Math.Ceil(m_FilteredAnimations.Count() / m_AnimsPerPage);
		if (m_CurrentPage >= maxPages)
			m_CurrentPage = 0;
		
		UpdateAnimationButtons();
		UpdatePageInfo();
	}
	
	void UpdateAnimationButtons()
	{
		// Clear existing buttons by deleting children of wrapper
		Widget child = m_AnimationWrapper.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			delete child;
			child = next;
		}
		m_AnimationButtons.Clear();
		
		// Calculate visible range
		int startIdx = m_CurrentPage * m_AnimsPerPage;
		int endIdx = Math.Min(startIdx + m_AnimsPerPage, m_FilteredAnimations.Count());
		
		Print("[SwarmTest] Creating buttons from " + startIdx + " to " + endIdx + " (page " + m_CurrentPage + ")");
		
		// Create buttons for current page directly in wrapper
		for (int i = startIdx; i < endIdx; i++)
		{
			SwarmTestAnimInfo anim = m_FilteredAnimations[i];
			string buttonText = "[" + anim.GetTypeName() + "] " + anim.m_Name;
			UIActionButton btn = UIActionManager.CreateButton(m_AnimationWrapper, buttonText, this, "OnClick_PlayAnimation");
			btn.SetData(new SwarmTestAnimUIData(anim));
			m_AnimationButtons.Insert(btn);
		}
		
		// Refresh scroller layout
		m_Scroller.UpdateScroller();
	}
	
	void UpdatePageInfo()
	{
		int totalPages = Math.Max(1, Math.Ceil(m_FilteredAnimations.Count() / m_AnimsPerPage));
		string pageText = string.Format("%1/%2 (%3)", m_CurrentPage + 1, totalPages, m_FilteredAnimations.Count());
		m_PageInfo.SetText(pageText);
		
		if (m_CurrentPage > 0)
			m_PrevPageButton.Enable();
		else
			m_PrevPageButton.Disable();
			
		if (m_CurrentPage < totalPages - 1)
			m_NextPageButton.Enable();
		else
			m_NextPageButton.Disable();
	}

	void OnChange_Search(UIEvent eid, UIActionBase action)
	{
		if (eid != UIEvent.CHANGE)
			return;
		
		m_CurrentPage = 0;
		RefreshAnimationList();
	}
	
	void OnClick_PlayAnimation(UIEvent eid, UIActionBase action)
	{
		Print("[SwarmTest] OnClick_PlayAnimation - eid: " + eid);
		
		if (eid != UIEvent.CLICK)
			return;
		
		UIActionButton btn = UIActionButton.Cast(action);
		if (!btn)
		{
			Print("[SwarmTest] btn is null");
			return;
		}
		
		SwarmTestAnimUIData data = SwarmTestAnimUIData.Cast(btn.GetData());
		if (!data || !data.AnimInfo)
		{
			Print("[SwarmTest] data is null or AnimInfo is null - data: " + data);
			return;
		}
		
		SwarmTestAnimInfo anim = data.AnimInfo;
		Print("[SwarmTest] Playing anim: " + anim.m_Name + " commandID: " + anim.m_CommandID + " type: " + anim.GetTypeName());
		
		// Get selected player GUIDs or use self
		array<string> guids = JM_GetSelected().GetPlayersOrSelf();
		Print("[SwarmTest] Target players: " + guids.Count());
		
		if (guids.Count() > 0)
		{
			m_Module.PlayAnimation(anim.m_CommandID, anim.m_Type, anim.m_StanceMask, guids);
		}
	}
	
	void OnClick_StopAnimation(UIEvent eid, UIActionBase action)
	{
		if (eid != UIEvent.CLICK)
			return;
		
		// Get selected player GUIDs or use self
		array<string> guids = JM_GetSelected().GetPlayersOrSelf();
		
		if (guids.Count() > 0)
		{
			m_Module.StopAnimation(guids);
		}
	}
	
	void OnClick_PrevPage(UIEvent eid, UIActionBase action)
	{
		if (eid != UIEvent.CLICK)
			return;
		
		if (m_CurrentPage > 0)
		{
			m_CurrentPage--;
			UpdateAnimationButtons();
			UpdatePageInfo();
		}
	}
	
	void OnClick_NextPage(UIEvent eid, UIActionBase action)
	{
		if (eid != UIEvent.CLICK)
			return;
		
		int maxPages = Math.Ceil(m_FilteredAnimations.Count() / m_AnimsPerPage);
		if (m_CurrentPage < maxPages - 1)
		{
			m_CurrentPage++;
			UpdateAnimationButtons();
			UpdatePageInfo();
		}
	}
}

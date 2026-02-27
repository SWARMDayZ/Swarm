// SwarmArenaPresetEditorMenu - Loadout preset configuration

class SwarmArenaPresetEditorMenu extends UIScriptedMenu
{
    protected const string LAYOUT_PATH = "SwarmArenaGUI/data/layouts/arena_preset_editor.layout";
    
    // Current state
    protected int m_CurrentPresetIndex = 0;
    protected ref array<ref SwarmArenaPreset> m_Presets = new array<ref SwarmArenaPreset>();
    protected ref SwarmArenaPreset m_EditingPreset;
    
    // Item catalog
    protected ref array<ref SwarmArenaPresetItem> m_AvailableItems = new array<ref SwarmArenaPresetItem>();
    protected string m_CurrentCategory = "WEAPONS";
    
    // UI widgets
    protected Widget m_Root;
    protected ButtonWidget m_BtnBack;
    protected ref array<ButtonWidget> m_PresetTabs = new array<ButtonWidget>();
    protected TextWidget m_ValCost;
    protected TextWidget m_CostWarning;
    
    // Category tabs
    protected ButtonWidget m_CatPrimary;
    protected ButtonWidget m_CatSecondary;
    protected ButtonWidget m_CatAttach;
    protected ButtonWidget m_CatMisc;
    
    // List panels
    protected Widget m_AvailList;
    protected Widget m_PrimaryItem;
    protected Widget m_PrimaryAttachList;
    protected Widget m_SecondaryItem;
    protected Widget m_SecondaryAttachList;
    protected Widget m_MiscItems;
    
    // Transfer buttons
    protected ButtonWidget m_BtnAddItem;
    protected ButtonWidget m_BtnRemoveItem;
    
    // Save button
    protected ButtonWidget m_BtnSavePreset;
    
    // Selected items
    protected int m_SelectedAvailableIndex = -1;
    protected int m_SelectedLoadoutIndex = -1;
    
    // --- Lifecycle ---
    
    void SwarmArenaPresetEditorMenu()
    {
        // Initialize with empty presets
        for (int i = 0; i < 5; i++)
        {
            SwarmArenaPreset preset = new SwarmArenaPreset();
            preset.Index = i;
            preset.Name = "Preset " + (i + 1).ToString();
            m_Presets.Insert(preset);
        }
        
        // Build item catalog
        BuildItemCatalog();
    }
    
    void ~SwarmArenaPresetEditorMenu()
    {
    }
    
    override Widget Init()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets(LAYOUT_PATH);
        
        // Find widgets
        m_BtnBack = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnBack"));
        m_ValCost = TextWidget.Cast(m_Root.FindAnyWidget("ValCost"));
        m_CostWarning = TextWidget.Cast(m_Root.FindAnyWidget("CostWarning"));
        
        // Find preset tabs
        m_PresetTabs.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("TabPreset1")));
        m_PresetTabs.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("TabPreset2")));
        m_PresetTabs.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("TabPreset3")));
        m_PresetTabs.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("TabPreset4")));
        m_PresetTabs.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("TabPreset5")));
        
        // Find category tabs
        m_CatPrimary = ButtonWidget.Cast(m_Root.FindAnyWidget("CatPrimary"));
        m_CatSecondary = ButtonWidget.Cast(m_Root.FindAnyWidget("CatSecondary"));
        m_CatAttach = ButtonWidget.Cast(m_Root.FindAnyWidget("CatAttach"));
        m_CatMisc = ButtonWidget.Cast(m_Root.FindAnyWidget("CatMisc"));
        
        // Find lists
        m_AvailList = m_Root.FindAnyWidget("AvailList");
        m_PrimaryItem = m_Root.FindAnyWidget("PrimaryItem");
        m_PrimaryAttachList = m_Root.FindAnyWidget("PrimaryAttachList");
        m_SecondaryItem = m_Root.FindAnyWidget("SecondaryItem");
        m_SecondaryAttachList = m_Root.FindAnyWidget("SecondaryAttachList");
        m_MiscItems = m_Root.FindAnyWidget("MiscItems");
        
        // Find transfer buttons
        m_BtnAddItem = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnAddItem"));
        m_BtnRemoveItem = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnRemoveItem"));
        
        // Find save button
        m_BtnSavePreset = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSavePreset"));
        
        // Load presets from API
        LoadPresetsFromAPI();
        
        // Select first preset
        SelectPreset(0);
        
        return m_Root;
    }
    
    override void OnShow()
    {
        super.OnShow();
        
        GetGame().GetInput().ChangeGameFocus(1);
        GetGame().GetUIManager().ShowCursor(true);
    }
    
    override void OnHide()
    {
        super.OnHide();
        
        GetGame().GetInput().ChangeGameFocus(-1);
        GetGame().GetUIManager().ShowCursor(false);
    }
    
    // --- Item Catalog ---
    
    protected void BuildItemCatalog()
    {
        m_AvailableItems.Clear();
        
        // Get prices from config
        SwarmArenaItemPrices prices = SwarmArenaItemPrices.Get();
        if (!prices)
        {
            return;
        }
        
        SwarmArenaPresetItem item;
        
        // Add weapons (primary category)
        foreach (string primary, int price : prices.Items.Weapons)
        {
            item = new SwarmArenaPresetItem();
            item.Classname = primary;
            item.Price = price;
            item.Category = "WEAPONS";
            item.DisplayName = GetItemDisplayName(primary);
            m_AvailableItems.Insert(item);
        }
        
        // Add attachments
        foreach (string attach, int attPrice : prices.Items.Attachments)
        {
            item = new SwarmArenaPresetItem();
            item.Classname = attach;
            item.Price = attPrice;
            item.Category = "ATTACHMENTS";
            item.DisplayName = GetItemDisplayName(attach);
            m_AvailableItems.Insert(item);
        }
        
        // Add magazines
        foreach (string mag, int magPrice : prices.Items.Magazines)
        {
            item = new SwarmArenaPresetItem();
            item.Classname = mag;
            item.Price = magPrice;
            item.Category = "MISC";
            item.DisplayName = GetItemDisplayName(mag);
            m_AvailableItems.Insert(item);
        }
        
        // Add consumables
        foreach (string misc, int miscPrice : prices.Items.Consumables)
        {
            item = new SwarmArenaPresetItem();
            item.Classname = misc;
            item.Price = miscPrice;
            item.Category = "MISC";
            item.DisplayName = GetItemDisplayName(misc);
            m_AvailableItems.Insert(item);
        }
    }
    
    protected string GetItemDisplayName(string className)
    {
        // Try to get display name from config
        string displayName = className;
        GetGame().ConfigGetText("CfgVehicles " + className + " displayName", displayName);
        if (displayName == "")
        {
            GetGame().ConfigGetText("CfgWeapons " + className + " displayName", displayName);
        }
        if (displayName == "")
        {
            displayName = className;
        }
        return displayName;
    }
    
    // --- API Integration ---
    
    protected void LoadPresetsFromAPI()
    {
        // Get steam ID
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player || !player.GetIdentity())
        {
            return;
        }
        
        string steamId = player.GetIdentity().GetPlainId();
        
        // Request presets via RPC
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(steamId);
        rpc.Send(null, SwarmArenaRPC.Preset_Request, true, null);
    }
    
    void OnPresetsLoaded(int status, array<ref SwarmArenaPreset> presets)
    {
        if (status == 200 && presets)
        {
            m_Presets.Clear();
            foreach (SwarmArenaPreset preset : presets)
            {
                m_Presets.Insert(preset);
            }
            
            // Ensure we have 5 presets
            while (m_Presets.Count() < 5)
            {
                SwarmArenaPreset empty = new SwarmArenaPreset();
                empty.Index = m_Presets.Count();
                empty.Name = "Preset " + (m_Presets.Count() + 1).ToString();
                m_Presets.Insert(empty);
            }
            
            SelectPreset(m_CurrentPresetIndex);
        }
    }
    
    protected void SavePresetToAPI()
    {
        if (!m_EditingPreset)
        {
            return;
        }
        
        // Get steam ID
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player || !player.GetIdentity())
        {
            return;
        }
        
        string steamId = player.GetIdentity().GetPlainId();
        
        // Validate preset
        int cost = m_EditingPreset.CalculateTotalCost();
        
        // First preset max $800
        if (m_CurrentPresetIndex == 0 && cost > 800)
        {
            m_CostWarning.SetText("First preset max $800!");
            return;
        }
        
        // Save via RPC
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(steamId);
        rpc.Write(m_CurrentPresetIndex);
        rpc.Write(m_EditingPreset.Name);
        rpc.Send(null, SwarmArenaRPC.Preset_Save, true, null);
    }
    
    void OnPresetSaved(int status)
    {
        if (status == 200)
        {
            // Update local copy
            m_Presets[m_CurrentPresetIndex] = m_EditingPreset;
            SwarmArenaCore.Log("Preset saved successfully");
        }
        else
        {
            m_CostWarning.SetText("Failed to save preset!");
        }
    }
    
    // --- Preset Selection ---
    
    protected void SelectPreset(int index)
    {
        if (index < 0 || index >= m_Presets.Count())
        {
            return;
        }
        
        m_CurrentPresetIndex = index;
        m_EditingPreset = m_Presets[index].Clone();
        
        // Update tab visuals
        for (int i = 0; i < m_PresetTabs.Count(); i++)
        {
            // TODO: Set selected state visual
        }
        
        RefreshUI();
    }
    
    // --- UI Refresh ---
    
    protected void RefreshUI()
    {
        RefreshCostDisplay();
        RefreshAvailableList();
        RefreshLoadoutDisplay();
    }
    
    protected void RefreshCostDisplay()
    {
        if (!m_EditingPreset)
        {
            return;
        }
        
        int cost = m_EditingPreset.CalculateTotalCost();
        int maxCost = 99999;
        
        // First preset has $800 limit
        if (m_CurrentPresetIndex == 0)
        {
            maxCost = 800;
        }
        
        m_ValCost.SetText("$" + cost.ToString() + " / $" + maxCost.ToString());
        
        if (cost > maxCost)
        {
            m_CostWarning.SetText("Over budget!");
            m_ValCost.SetColor(ARGB(255, 255, 100, 100));
        }
        else
        {
            m_CostWarning.SetText("");
            m_ValCost.SetColor(ARGB(255, 100, 255, 100));
        }
    }
    
    protected void RefreshAvailableList()
    {
        // Clear list
        // TODO: Actually clear and populate list widgets
        
        // Filter by category
        foreach (SwarmArenaPresetItem item : m_AvailableItems)
        {
            if (item.Category == m_CurrentCategory)
            {
                // TODO: Create list entry widget
                // Entry would show: item.DisplayName + " - $" + item.Price
            }
        }
    }
    
    protected void RefreshLoadoutDisplay()
    {
        if (!m_EditingPreset)
        {
            return;
        }
        
        // TODO: Update loadout slot widgets with m_EditingPreset.Items
    }
    
    // --- Button Handlers ---
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        // Back button
        if (w == m_BtnBack)
        {
            Close();
            GetGame().GetUIManager().EnterScriptedMenu(MENU_SWARMARENA_LOBBY, null);
            return true;
        }
        
        // Preset tabs
        for (int i = 0; i < m_PresetTabs.Count(); i++)
        {
            if (w == m_PresetTabs[i])
            {
                SelectPreset(i);
                return true;
            }
        }
        
        // Category tabs
        if (w == m_CatPrimary)
        {
            m_CurrentCategory = "WEAPONS";
            RefreshAvailableList();
            return true;
        }
        if (w == m_CatSecondary)
        {
            m_CurrentCategory = "WEAPONS";
            RefreshAvailableList();
            return true;
        }
        if (w == m_CatAttach)
        {
            m_CurrentCategory = "ATTACHMENTS";
            RefreshAvailableList();
            return true;
        }
        if (w == m_CatMisc)
        {
            m_CurrentCategory = "MISC";
            RefreshAvailableList();
            return true;
        }
        
        // Transfer buttons
        if (w == m_BtnAddItem)
        {
            AddSelectedItem();
            return true;
        }
        if (w == m_BtnRemoveItem)
        {
            RemoveSelectedItem();
            return true;
        }
        
        // Save button
        if (w == m_BtnSavePreset)
        {
            SavePresetToAPI();
            return true;
        }
        
        return false;
    }
    
    protected void AddSelectedItem()
    {
        if (m_SelectedAvailableIndex < 0 || !m_EditingPreset)
        {
            return;
        }
        
        // Get selected item from filtered list
        int currentIndex = 0;
        foreach (SwarmArenaPresetItem item : m_AvailableItems)
        {
            if (item.Category == m_CurrentCategory)
            {
                if (currentIndex == m_SelectedAvailableIndex)
                {
                    // Add to preset
                    m_EditingPreset.Items.Insert(item);
                    RefreshUI();
                    return;
                }
                currentIndex++;
            }
        }
    }
    
    protected void RemoveSelectedItem()
    {
        if (m_SelectedLoadoutIndex < 0 || !m_EditingPreset)
        {
            return;
        }
        
        if (m_SelectedLoadoutIndex < m_EditingPreset.Items.Count())
        {
            m_EditingPreset.Items.Remove(m_SelectedLoadoutIndex);
            m_SelectedLoadoutIndex = -1;
            RefreshUI();
        }
    }
}

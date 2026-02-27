// SwarmArenaBuyMenu - In-match weapon/armor purchasing during freeze time

class SwarmArenaBuyMenu extends UIScriptedMenu
{
    protected const string LAYOUT_PATH = "SwarmArenaGUI/data/layouts/arena_buy_menu.layout";
    
    // UI widgets
    protected Widget m_Root;
    protected TextWidget m_MoneyText;
    protected TextWidget m_TimerText;
    protected TextWidget m_ValCurrentArmor;
    
    // Preset buttons
    protected ref array<ButtonWidget> m_PresetButtons = new array<ButtonWidget>();
    
    // Armor buttons
    protected ButtonWidget m_BtnArmorLight;
    protected ButtonWidget m_BtnArmorMedium;
    protected ButtonWidget m_BtnArmorHeavy;
    
    // Quick buy list
    protected Widget m_QuickBuyList;
    
    // Player state
    protected int m_CurrentMoney = 800;
    protected int m_FreezeTimeRemaining = 15;
    protected SwarmArenaArmorType m_CurrentArmor = SwarmArenaArmorType.NONE;
    protected ref array<ref SwarmArenaPreset> m_PlayerPresets = new array<ref SwarmArenaPreset>();
    
    // --- Lifecycle ---
    
    void SwarmArenaBuyMenu()
    {
    }
    
    void ~SwarmArenaBuyMenu()
    {
    }
    
    override Widget Init()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets(LAYOUT_PATH);
        
        // Find widgets
        m_MoneyText = TextWidget.Cast(m_Root.FindAnyWidget("MoneyText"));
        m_TimerText = TextWidget.Cast(m_Root.FindAnyWidget("TimerText"));
        m_ValCurrentArmor = TextWidget.Cast(m_Root.FindAnyWidget("ValCurrentArmor"));
        
        // Find preset buttons
        m_PresetButtons.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("BtnPreset1")));
        m_PresetButtons.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("BtnPreset2")));
        m_PresetButtons.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("BtnPreset3")));
        m_PresetButtons.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("BtnPreset4")));
        m_PresetButtons.Insert(ButtonWidget.Cast(m_Root.FindAnyWidget("BtnPreset5")));
        
        // Find armor buttons
        m_BtnArmorLight = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnArmorLight"));
        m_BtnArmorMedium = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnArmorMedium"));
        m_BtnArmorHeavy = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnArmorHeavy"));
        
        // Find quick buy list
        m_QuickBuyList = m_Root.FindAnyWidget("QuickBuyList");
        
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
    
    // --- UI Updates ---
    
    void RefreshUI()
    {
        RefreshMoneyDisplay();
        RefreshTimerDisplay();
        RefreshArmorDisplay();
        RefreshPresetButtons();
        RefreshArmorButtons();
    }
    
    protected void RefreshMoneyDisplay()
    {
        m_MoneyText.SetText("$" + m_CurrentMoney.ToString());
        m_MoneyText.SetColor(SwarmArenaGUIColors.GetMoneyColor(m_CurrentMoney));
    }
    
    protected void RefreshTimerDisplay()
    {
        m_TimerText.SetText("FREEZE: " + m_FreezeTimeRemaining.ToString() + "s");
    }
    
    protected void RefreshArmorDisplay()
    {
        string armorName;
        switch (m_CurrentArmor)
        {
            case SwarmArenaArmorType.NONE: armorName = "None"; break;
            case SwarmArenaArmorType.LIGHT: armorName = "Light"; break;
            case SwarmArenaArmorType.MEDIUM: armorName = "Medium"; break;
            case SwarmArenaArmorType.HEAVY: armorName = "Heavy"; break;
        }
        m_ValCurrentArmor.SetText(armorName);
    }
    
    protected void RefreshPresetButtons()
    {
        SwarmArenaArmorConfig armorConfig = SwarmArenaArmorConfig.Get();
        
        for (int i = 0; i < m_PresetButtons.Count(); i++)
        {
            ButtonWidget btn = m_PresetButtons[i];
            if (!btn)
            {
                continue;
            }
            
            int presetCost = 0;
            string presetName = "Preset " + (i + 1).ToString();
            
            if (i < m_PlayerPresets.Count())
            {
                SwarmArenaPreset preset = m_PlayerPresets[i];
                presetCost = preset.CalculateTotalCost();
                presetName = preset.Name;
            }
            
            btn.SetText(presetName + " - $" + presetCost.ToString());
            
            // Disable if can't afford
            btn.Enable(m_CurrentMoney >= presetCost);
        }
    }
    
    protected void RefreshArmorButtons()
    {
        SwarmArenaArmorConfig config = SwarmArenaArmorConfig.Get();
        if (!config)
        {
            return;
        }
        
        // Light armor
        int lightPrice = config.GetArmorPrice(SwarmArenaArmorType.LIGHT);
        m_BtnArmorLight.SetText("Light Armor - $" + lightPrice.ToString());
        m_BtnArmorLight.Enable(m_CurrentMoney >= lightPrice && m_CurrentArmor < SwarmArenaArmorType.LIGHT);
        
        // Medium armor
        int mediumPrice = config.GetArmorPrice(SwarmArenaArmorType.MEDIUM);
        m_BtnArmorMedium.SetText("Medium Armor - $" + mediumPrice.ToString());
        m_BtnArmorMedium.Enable(m_CurrentMoney >= mediumPrice && m_CurrentArmor < SwarmArenaArmorType.MEDIUM);
        
        // Heavy armor
        int heavyPrice = config.GetArmorPrice(SwarmArenaArmorType.HEAVY);
        m_BtnArmorHeavy.SetText("Heavy Armor - $" + heavyPrice.ToString());
        m_BtnArmorHeavy.Enable(m_CurrentMoney >= heavyPrice && m_CurrentArmor < SwarmArenaArmorType.HEAVY);
    }
    
    // --- External State Updates ---
    
    void SetMoney(int amount)
    {
        m_CurrentMoney = amount;
        RefreshMoneyDisplay();
        RefreshPresetButtons();
        RefreshArmorButtons();
    }
    
    void SetFreezeTime(int seconds)
    {
        m_FreezeTimeRemaining = seconds;
        RefreshTimerDisplay();
    }
    
    void SetCurrentArmor(SwarmArenaArmorType armor)
    {
        m_CurrentArmor = armor;
        RefreshArmorDisplay();
        RefreshArmorButtons();
    }
    
    void SetPresets(array<ref SwarmArenaPreset> presets)
    {
        m_PlayerPresets.Clear();
        foreach (SwarmArenaPreset preset : presets)
        {
            m_PlayerPresets.Insert(preset);
        }
        RefreshPresetButtons();
    }
    
    // --- Button Handlers ---
    
    override bool OnClick(Widget w, int x, int y, int button)
    {
        // Preset buttons
        for (int i = 0; i < m_PresetButtons.Count(); i++)
        {
            if (w == m_PresetButtons[i])
            {
                BuyPreset(i);
                return true;
            }
        }
        
        // Armor buttons
        if (w == m_BtnArmorLight)
        {
            BuyArmor(SwarmArenaArmorType.LIGHT);
            return true;
        }
        if (w == m_BtnArmorMedium)
        {
            BuyArmor(SwarmArenaArmorType.MEDIUM);
            return true;
        }
        if (w == m_BtnArmorHeavy)
        {
            BuyArmor(SwarmArenaArmorType.HEAVY);
            return true;
        }
        
        return false;
    }
    
    protected void BuyPreset(int presetIndex)
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(presetIndex);
        rpc.Send(null, SwarmArenaRPC.Buy_Preset, true, null);
        
        SwarmArenaCore.Log("Buy preset request: " + presetIndex.ToString());
    }
    
    protected void BuyArmor(SwarmArenaArmorType armorType)
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(armorType);
        rpc.Send(null, SwarmArenaRPC.Buy_Armor, true, null);
        
        SwarmArenaCore.Log("Buy armor request: " + armorType.ToString());
    }
    
    // --- Input handling ---
    
    override void Update(float timeslice)
    {
        super.Update(timeslice);
        
        // Close on B press
        Input input = GetGame().GetInput();
        if (input.LocalPress("UASwarmArenaBuyMenu"))
        {
            Close();
        }
    }
}

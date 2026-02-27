// SwarmArenaPreset - Player loadout preset with items and cost validation

class SwarmArenaPreset
{
    string Name;
    int Index;
    ref array<ref SwarmArenaPresetItem> Items = new array<ref SwarmArenaPresetItem>();
    
    void SwarmArenaPreset()
    {
        Name = "New Preset";
        Index = 0;
    }
    
    void SetName(string name)
    {
        Name = name;
    }
    
    void AddItem(SwarmArenaPresetItem item)
    {
        Items.Insert(item);
    }
    
    void RemoveItem(int index)
    {
        if (index >= 0 && index < Items.Count())
        {
            Items.Remove(index);
        }
    }
    
    void ClearItems()
    {
        Items.Clear();
    }
    
    int CalculateTotalCost()
    {
        int total = 0;
        
        foreach (SwarmArenaPresetItem item : Items)
        {
            total += item.CalculateCost();
        }
        
        return total;
    }
    
    // First preset (index 0) has budget limit, others are unlimited
    bool IsValidForIndex(int presetIndex)
    {
        if (presetIndex == 0)
        {
            return CalculateTotalCost() <= SwarmArenaConfig.Get().PresetBudgetLimit;
        }
        return true; // Other presets have no limit
    }
    
    bool IsWithinBudget(int budget)
    {
        return CalculateTotalCost() <= budget;
    }
    
    SwarmArenaPreset Clone()
    {
        SwarmArenaPreset clone = new SwarmArenaPreset();
        clone.Name = Name;
        clone.Index = Index;
        
        foreach (SwarmArenaPresetItem item : Items)
        {
            clone.Items.Insert(item.Clone());
        }
        
        return clone;
    }
}

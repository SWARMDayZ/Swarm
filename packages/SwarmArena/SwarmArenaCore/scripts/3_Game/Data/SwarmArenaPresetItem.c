// SwarmArenaPresetItem - Single item with attachments in a preset

class SwarmArenaPresetItem
{
    string Classname;
    string Category;     // "PRIMARY", "SECONDARY", "ATTACHMENTS", "MISC"
    int Price;
    string DisplayName;
    ref array<string> Attachments = new array<string>();
    
    void SwarmArenaPresetItem()
    {
        Classname = "";
        Category = "";
        Price = 0;
        DisplayName = "";
    }
    
    void Set(string classname)
    {
        Classname = classname;
    }
    
    void AddAttachment(string attachmentClassname)
    {
        Attachments.Insert(attachmentClassname);
    }
    
    void ClearAttachments()
    {
        Attachments.Clear();
    }
    
    int CalculateCost()
    {
        int total = SwarmArenaItemPrices.GetPrice(Classname);
        
        foreach (string attachment : Attachments)
        {
            total += SwarmArenaItemPrices.GetPrice(attachment);
        }
        
        return total;
    }
    
    SwarmArenaPresetItem Clone()
    {
        SwarmArenaPresetItem clone = new SwarmArenaPresetItem();
        clone.Classname = Classname;
        clone.Category = Category;
        clone.Price = Price;
        clone.DisplayName = DisplayName;
        
        foreach (string attachment : Attachments)
        {
            clone.Attachments.Insert(attachment);
        }
        
        return clone;
    }
}

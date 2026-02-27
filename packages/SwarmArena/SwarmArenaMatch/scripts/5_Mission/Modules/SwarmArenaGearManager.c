// SwarmArenaGearManager - Handles gear stripping and spawning

class SwarmArenaGearManager
{
    // Strip all items from player
    static void StripAll(PlayerBase player)
    {
        if (!player)
        {
            return;
        }
        
        array<EntityAI> items = new array<EntityAI>();
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
        
        foreach (EntityAI entity : items)
        {
            if (entity == player)
            {
                continue;
            }
            
            ItemBase item;
            if (Class.CastTo(item, entity))
            {
                item.Delete();
            }
        }
    }
    
    // Strip only weapons and magazines (keep clothes)
    static void StripWeapons(PlayerBase player)
    {
        if (!player)
        {
            return;
        }
        
        array<EntityAI> items = new array<EntityAI>();
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
        
        foreach (EntityAI entity : items)
        {
            if (entity == player)
            {
                continue;
            }
            
            // Delete weapons
            Weapon_Base weapon;
            if (Class.CastTo(weapon, entity))
            {
                weapon.Delete();
                continue;
            }
            
            // Delete magazines
            Magazine mag;
            if (Class.CastTo(mag, entity))
            {
                mag.Delete();
                continue;
            }
            
            // Delete items that can be used as melee weapons
            ItemBase item;
            if (Class.CastTo(item, entity))
            {
                // Check if it's a tool/melee weapon type
                if (item.IsWeapon() || item.IsMeleeWeapon())
                {
                    item.Delete();
                }
            }
        }
    }
    
    // Spawn a preset's items on player
    static void SpawnPreset(PlayerBase player, SwarmArenaPreset preset)
    {
        if (!player || !preset)
        {
            return;
        }
        
        foreach (SwarmArenaPresetItem presetItem : preset.Items)
        {
            SpawnPresetItem(player, presetItem);
        }
    }
    
    // Spawn a single preset item with attachments
    static void SpawnPresetItem(PlayerBase player, SwarmArenaPresetItem presetItem)
    {
        if (!player || !presetItem || presetItem.Classname == "")
        {
            return;
        }
        
        // Create the main item
        EntityAI item = player.GetInventory().CreateInInventory(presetItem.Classname);
        if (!item)
        {
            // Try hands
            item = player.GetHumanInventory().CreateInHands(presetItem.Classname);
        }
        
        if (!item)
        {
            SwarmArenaCore.LogWarning("Failed to spawn item: " + presetItem.Classname);
            return;
        }
        
        // Spawn attachments
        foreach (string attachmentClassname : presetItem.Attachments)
        {
            if (attachmentClassname == "")
            {
                continue;
            }
            
            item.GetInventory().CreateAttachment(attachmentClassname);
        }
    }
    
    // Spawn armor on player
    static void SpawnArmor(PlayerBase player, SwarmArenaArmorEntry armorEntry)
    {
        if (!player || !armorEntry)
        {
            return;
        }
        
        if (armorEntry.Classname == "")
        {
            return;
        }
        
        // Remove existing vest if any
        EntityAI existingVest = player.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Vest"));
        if (existingVest)
        {
            existingVest.Delete();
        }
        
        // Spawn new armor
        player.GetInventory().CreateAttachment(armorEntry.Classname);
    }
    
    // Spawn a single item by classname
    static EntityAI SpawnItem(PlayerBase player, string classname)
    {
        if (!player || classname == "")
        {
            return null;
        }
        
        EntityAI item = player.GetInventory().CreateInInventory(classname);
        if (!item)
        {
            item = player.GetHumanInventory().CreateInHands(classname);
        }
        
        return item;
    }
    
    // Check if player has inventory space for items
    static bool HasInventorySpace(PlayerBase player, string classname)
    {
        if (!player || classname == "")
        {
            return false;
        }
        
        // Simple check - try to find any cargo space
        return player.GetInventory().CanAddEntityIntoInventory(null);
    }
}

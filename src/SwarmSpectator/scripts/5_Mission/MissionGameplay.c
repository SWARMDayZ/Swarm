modded class MissionGameplay
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        
        // Toggle Lock On Target with hotkey (only when no menu is open)
        if (GetGame().GetInput().LocalPress("UASwarmSpectatorToggleLockOn") && GetGame().GetUIManager().GetMenu() == NULL)
        {
            // Toggle the setting
            SwarmSpectatorSettings.LockOnTarget = !SwarmSpectatorSettings.LockOnTarget;
            
            // Show feedback message in chat
            if (SwarmSpectatorSettings.LockOnTarget)
            {
                GetGame().Chat("Lock On Target: ENABLED", "colorFriendly");
            }
            else
            {
                GetGame().Chat("Lock On Target: DISABLED", "colorImportant");
            }
        }
    }
}

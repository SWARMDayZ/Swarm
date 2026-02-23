modded class MissionGameplay
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        
        // Toggle Lock On Target with hotkey (only when no menu is open)
        if (GetGame().GetInput().LocalPress("UASwarmSpectatorToggleLockOn") && GetGame().GetUIManager().GetMenu() == NULL)
        {
            SwarmSpectatorSettings.LockOnTarget = !SwarmSpectatorSettings.LockOnTarget;
        }
    }
}

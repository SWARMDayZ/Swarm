modded class JMCameraForm
{
    private UIActionCheckbox m_ShowTargetMarker;
    private UIActionCheckbox m_LockOnTarget;

    override void InitCameraEffects()
    {
        super.InitCameraEffects();
        
        // Add checkboxes to the camera effects panel
        Widget content = m_sclr_MainActions.GetContentWidget();
        if (content)
        {
            m_LockOnTarget = UIActionManager.CreateCheckbox(content, "Lock On Target (Dolly Cam)", this, "OnClick_LockOnTarget", SwarmSpectatorSettings.LockOnTarget);
            m_ShowTargetMarker = UIActionManager.CreateCheckbox(content, "Show Target Marker (Dolly Cam)", this, "OnClick_ShowTargetMarker", SwarmSpectatorSettings.ShowTargetMarker);
        }
    }

    void OnClick_ShowTargetMarker(UIEvent eid, UIActionBase action)
    {
        if (eid != UIEvent.CLICK)
            return;

        // Update the static setting
        SwarmSpectatorSettings.ShowTargetMarker = action.IsChecked();
    }

    void OnClick_LockOnTarget(UIEvent eid, UIActionBase action)
    {
        if (eid != UIEvent.CLICK)
            return;

        // Update the static setting
        SwarmSpectatorSettings.LockOnTarget = action.IsChecked();
    }
}

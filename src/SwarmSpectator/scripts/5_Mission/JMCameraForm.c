modded class JMCameraForm
{
    private UIActionCheckbox m_ShowTargetMarker;

    override void InitCameraEffects()
    {
        super.InitCameraEffects();
        
        // Add checkbox to the camera effects panel
        Widget content = m_sclr_MainActions.GetContentWidget();
        if (content)
        {
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
}

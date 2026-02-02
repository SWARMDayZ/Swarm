modded class JMSpectatorCamera
{
    override void OnUpdate(float timeslice)
    {
        // Run all original COT camera logic first
        super.OnUpdate(timeslice);
        
        // After COT processing, check if we should hide the marker
        // Uses static setting from SwarmSpectatorSettings (3_Game) 
        // This only affects m_COT_TargetMarker and m_COT_TargetLight - nothing else
        if (!SwarmSpectatorSettings.ShowTargetMarker)
        {
            // Force marker removal by setting the flag and calling remove
            m_COT_RemoveMarker = true;
            COT_RemoveMarker();
        }
    }
}

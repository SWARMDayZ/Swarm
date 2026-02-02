modded class JMSpectatorCamera
{
    override void OnUpdate(float timeslice)
    {
        // If lock-on is disabled, clear the target before COT processes it
        // This prevents the camera from focusing on detected targets
        if (!SwarmSpectatorSettings.LockOnTarget)
        {
            m_COT_LookAtTarget = null;
        }
        
        // Run all original COT camera logic
        super.OnUpdate(timeslice);
        
        // After COT processing, check if we should hide the marker
        // Uses static setting from SwarmSpectatorSettings (3_Game) 
        // This only affects m_COT_TargetMarker and m_COT_TargetLight - nothing else
        if (!SwarmSpectatorSettings.ShowTargetMarker)
        {
            // Remove marker particle
            if (m_COT_TargetMarker)
            {
                if (m_COT_TargetMarker.IsParticlePlaying())
                    m_COT_TargetMarker.StopParticle();
                m_COT_TargetMarker = null;
            }
            
            // Remove light immediately (don't use FadeOut which takes 3 seconds)
            if (m_COT_TargetLight)
            {
                m_COT_TargetLight.Destroy();
                m_COT_TargetLight = null;
            }
            
            m_COT_RemoveMarker = true;
        }
        
        // If lock-on is disabled, also clear the target after processing
        // to prevent it from persisting to the next frame
        if (!SwarmSpectatorSettings.LockOnTarget)
        {
            m_COT_LookAtTarget = null;
        }
    }
}

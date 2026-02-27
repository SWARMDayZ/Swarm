// SwarmArenaSpectatorCamera - Custom 3rd-person spectator camera
// Based on COT JMSpectatorCamera patterns with simplified implementation

class SwarmArenaSpectatorCamera extends Camera
{
    // Static instance (weak ref - Camera destructor is private)
    static SwarmArenaSpectatorCamera s_Instance;
    
    // Current spectate target
    protected PlayerBase m_TargetPlayer;
    
    // Valid spectate targets (alive teammates)
    protected ref array<string> m_ValidTargetSteamIds = new array<string>();
    protected int m_CurrentTargetIndex = 0;
    
    // Camera state
    protected bool m_IsActive = false;
    protected vector m_CurrentPosition;
    protected vector m_CurrentOrientation;
    
    // Smooth velocity tracking (for SmoothCD)
    protected float m_SmoothVelX[1];
    protected float m_SmoothVelY[1];
    protected float m_SmoothVelZ[1];
    
    // Angular velocity arrays for SmoothCD
    protected float m_AngularVelYaw[1];
    protected float m_AngularVelPitch[1];
    protected float m_AngularVelFreeLookYaw[1];
    protected float m_AngularVelFreeLookPitch[1];
    protected float m_FreeLookYaw;
    protected float m_FreeLookPitch;
    protected bool m_InFreeLook;
    
    void SwarmArenaSpectatorCamera()
    {
        // Register for frame events
        SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        
        // Initialize velocities
        m_SmoothVelX[0] = 0;
        m_SmoothVelY[0] = 0;
        m_SmoothVelZ[0] = 0;
        m_AngularVelYaw[0] = 0;
        m_AngularVelPitch[0] = 0;
        m_AngularVelFreeLookYaw[0] = 0;
        m_AngularVelFreeLookPitch[0] = 0;
        
        SwarmArenaCore.Log("SpectatorCamera created");
    }
    
    void ~SwarmArenaSpectatorCamera()
    {
        if (s_Instance == this)
        {
            s_Instance = null;
        }
    }
    
    // --- Static interface ---
    
    static SwarmArenaSpectatorCamera GetInstance()
    {
        return s_Instance;
    }
    
    // Enter spectator mode
    static void Enter(string targetSteamId, array<string> validTargetSteamIds)
    {
        if (!s_Instance)
        {
            // Create camera entity
            s_Instance = SwarmArenaSpectatorCamera.Cast(GetGame().CreateObjectEx("SwarmArenaSpectatorCamera", vector.Zero, ECE_LOCAL));
        }
        
        if (!s_Instance)
        {
            SwarmArenaCore.LogError("Failed to create spectator camera");
            return;
        }
        
        s_Instance.m_ValidTargetSteamIds.Clear();
        foreach (string steamId : validTargetSteamIds)
        {
            s_Instance.m_ValidTargetSteamIds.Insert(steamId);
        }
        
        // Find initial target
        s_Instance.SetTargetBySteamId(targetSteamId);
        s_Instance.Activate();
        
        SwarmArenaCore.Log("Spectator camera activated");
    }
    
    // Exit spectator mode
    static void Leave()
    {
        if (s_Instance)
        {
            s_Instance.Deactivate();
        }
        
        SwarmArenaCore.Log("Spectator camera deactivated");
    }
    
    // Update valid targets (when teammate dies)
    static void UpdateValidTargets(array<string> validTargetSteamIds)
    {
        if (!s_Instance)
        {
            return;
        }
        
        s_Instance.m_ValidTargetSteamIds.Clear();
        foreach (string steamId : validTargetSteamIds)
        {
            s_Instance.m_ValidTargetSteamIds.Insert(steamId);
        }
        
        // Check if current target is still valid
        if (s_Instance.m_TargetPlayer)
        {
            string currentSteamId = "";
            if (s_Instance.m_TargetPlayer.GetIdentity())
            {
                currentSteamId = s_Instance.m_TargetPlayer.GetIdentity().GetPlainId();
            }
            
            if (s_Instance.m_ValidTargetSteamIds.Find(currentSteamId) < 0)
            {
                // Current target no longer valid, switch
                s_Instance.NextTarget();
            }
        }
    }
    
    // --- Instance methods ---
    
    void Activate()
    {
        m_IsActive = true;
        SetActive(true);
        
        // Initialize position
        if (m_TargetPlayer)
        {
            m_CurrentPosition = CalculateDesiredPosition();
            SetPosition(m_CurrentPosition);
            
            // Look at target
            vector targetPos = m_TargetPlayer.GetPosition();
            vector dir = (targetPos - m_CurrentPosition).Normalized();
            m_CurrentOrientation = dir.VectorToAngles();
            SetOrientation(m_CurrentOrientation);
        }
    }
    
    void Deactivate()
    {
        m_IsActive = false;
        SetActive(false);
        m_TargetPlayer = null;
    }
    
    void SetTargetBySteamId(string steamId)
    {
        if (steamId == "")
        {
            return;
        }
        
        // Find player by steam ID
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        
        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player && player.GetIdentity())
            {
                if (player.GetIdentity().GetPlainId() == steamId)
                {
                    SetTarget(player);
                    
                    // Update index
                    m_CurrentTargetIndex = m_ValidTargetSteamIds.Find(steamId);
                    if (m_CurrentTargetIndex < 0)
                    {
                        m_CurrentTargetIndex = 0;
                    }
                    return;
                }
            }
        }
        
        SwarmArenaCore.LogWarning("Spectator target not found: " + steamId);
    }
    
    void SetTarget(PlayerBase target)
    {
        if (m_TargetPlayer == target)
        {
            return;
        }
        
        // Deselect old target
        if (m_TargetPlayer)
        {
            OnTargetDeselected(m_TargetPlayer);
        }
        
        m_TargetPlayer = target;
        
        // Select new target
        if (m_TargetPlayer)
        {
            OnTargetSelected(m_TargetPlayer);
            
            // Reset free look
            m_FreeLookYaw = 0;
            m_FreeLookPitch = 0;
            m_InFreeLook = false;
        }
    }
    
    void OnTargetSelected(PlayerBase target)
    {
        SwarmArenaCore.Log("Spectating player: " + target.GetIdentity().GetName());
    }
    
    void OnTargetDeselected(PlayerBase target)
    {
        // Cleanup if needed
    }
    
    // Switch to next teammate
    void NextTarget()
    {
        if (m_ValidTargetSteamIds.Count() == 0)
        {
            return;
        }
        
        m_CurrentTargetIndex++;
        if (m_CurrentTargetIndex >= m_ValidTargetSteamIds.Count())
        {
            m_CurrentTargetIndex = 0;
        }
        
        string steamId = m_ValidTargetSteamIds[m_CurrentTargetIndex];
        SetTargetBySteamId(steamId);
    }
    
    // Switch to previous teammate
    void PreviousTarget()
    {
        if (m_ValidTargetSteamIds.Count() == 0)
        {
            return;
        }
        
        m_CurrentTargetIndex--;
        if (m_CurrentTargetIndex < 0)
        {
            m_CurrentTargetIndex = m_ValidTargetSteamIds.Count() - 1;
        }
        
        string steamId = m_ValidTargetSteamIds[m_CurrentTargetIndex];
        SetTargetBySteamId(steamId);
    }
    
    // Get target's head position
    protected vector GetTargetHeadPosition()
    {
        if (!m_TargetPlayer)
        {
            return vector.Zero;
        }
        
        // Try to get head bone position
        int boneIdx = m_TargetPlayer.GetBoneIndexByName("Head");
        if (boneIdx != -1)
        {
            return m_TargetPlayer.GetBonePositionWS(boneIdx);
        }
        
        // Fallback: player position + height offset
        return m_TargetPlayer.GetPosition() + "0 1.8 0";
    }
    
    // Calculate desired camera position behind target
    protected vector CalculateDesiredPosition()
    {
        if (!m_TargetPlayer)
        {
            return m_CurrentPosition;
        }
        
        vector headPos = GetTargetHeadPosition();
        vector offset = SwarmArenaSpectatorSettings.CameraOffset;
        
        // Get target's orientation
        vector targetOri = m_TargetPlayer.GetOrientation();
        float yaw = targetOri[0];
        
        // Apply free look adjustment
        yaw += m_FreeLookYaw;
        
        // Rotate offset by target's yaw
        float cosYaw = Math.Cos(yaw * Math.DEG2RAD);
        float sinYaw = Math.Sin(yaw * Math.DEG2RAD);
        float rotX = offset[0] * cosYaw - offset[2] * sinYaw;
        float rotZ = offset[0] * sinYaw + offset[2] * cosYaw;
        vector rotatedOffset = Vector(rotX, offset[1], rotZ);
        
        vector desiredPos = headPos + rotatedOffset;
        
        // Collision check - raycast from head to desired position
        desiredPos = ResolveCollision(headPos, desiredPos);
        
        return desiredPos;
    }
    
    // Resolve camera collision with world
    protected vector ResolveCollision(vector origin, vector desiredPos)
    {
        vector dir = desiredPos - origin;
        float dist = dir.Length();
        
        if (dist < 0.1)
        {
            return desiredPos;
        }
        
        dir.Normalize();
        
        // Raycast from origin to desired position
        vector hitPos;
        vector hitNormal;
        int hitComponent;
        
        bool hit = DayZPhysics.RaycastRV(origin, desiredPos, hitPos, hitNormal, hitComponent, null, m_TargetPlayer, null, false, false, ObjIntersectFire, 0.0);
        
        if (hit)
        {
            // Move camera to hit point with offset
            return hitPos + hitNormal * SwarmArenaSpectatorSettings.CollisionOffset;
        }
        
        return desiredPos;
    }
    
    // Frame update stored for PostFrame
    protected float m_OnFrameTimeslice;
    
    override void EOnFrame(IEntity other, float timeSlice)
    {
        m_OnFrameTimeslice = timeSlice;
    }
    
    // Main update (called from EOnPostFrame like COT)
    override void EOnPostFrame(IEntity other, int extra)
    {
        if (!m_IsActive)
        {
            return;
        }
        
        float timeslice = m_OnFrameTimeslice;
        
        OnUpdate(timeslice);
    }
    
    void OnUpdate(float timeslice)
    {
        if (!m_TargetPlayer)
        {
            return;
        }
        
        // Handle input
        HandleInput(timeslice);
        
        // Calculate desired position
        vector desiredPos = CalculateDesiredPosition();
        
        // Smooth position update (COT-style SmoothCD)
        float smoothTime = SwarmArenaSpectatorSettings.SmoothTime;
        
        m_CurrentPosition[0] = Math.SmoothCD(m_CurrentPosition[0], desiredPos[0], m_SmoothVelX, smoothTime, 1000, timeslice);
        m_CurrentPosition[1] = Math.SmoothCD(m_CurrentPosition[1], desiredPos[1], m_SmoothVelY, smoothTime, 1000, timeslice);
        m_CurrentPosition[2] = Math.SmoothCD(m_CurrentPosition[2], desiredPos[2], m_SmoothVelZ, smoothTime, 1000, timeslice);
        
        SetPosition(m_CurrentPosition);
        
        // Calculate orientation to look at target
        vector targetLookPos = GetTargetHeadPosition();
        vector lookDir = (targetLookPos - m_CurrentPosition).Normalized();
        
        // Apply pitch adjustment from free look
        vector angles = lookDir.VectorToAngles();
        float targetPitch = angles[1] + m_FreeLookPitch;
        
        // Clamp pitch
        if (targetPitch > 89) targetPitch = 89;
        if (targetPitch < -89) targetPitch = -89;
        
        // Smooth orientation
        float halfSmoothTime = smoothTime * 0.5;
        m_CurrentOrientation[0] = Math.SmoothCD(m_CurrentOrientation[0], angles[0], m_AngularVelYaw, halfSmoothTime, 1000, timeslice);
        m_CurrentOrientation[1] = Math.SmoothCD(m_CurrentOrientation[1], targetPitch, m_AngularVelPitch, halfSmoothTime, 1000, timeslice);
        
        SetOrientation(m_CurrentOrientation);
        
        // Update spectator position for network bubble
        if (GetGame())
        {
            GetGame().UpdateSpectatorPosition(m_CurrentPosition);
        }
    }
    
    protected void HandleInput(float timeslice)
    {
        Input input = GetGame().GetInput();
        
        // Check for target switching
        if (input.LocalPress("UASwarmArenaSpectatePrev"))
        {
            PreviousTarget();
        }
        
        if (input.LocalPress("UASwarmArenaSpectateNext"))
        {
            NextTarget();
        }
        
        // Free look handling
        bool freelook = input.LocalValue("UALookAround");
        
        if (freelook)
        {
            m_InFreeLook = true;
            
            float sensitivity = SwarmArenaSpectatorSettings.MouseSensitivity;
            float smooth = SwarmArenaSpectatorSettings.AngularSmooth;
            
            float yawInput = input.LocalValue("UAAimLeft") - input.LocalValue("UAAimRight");
            float pitchInput = input.LocalValue("UAAimDown") - input.LocalValue("UAAimUp");
            
            m_FreeLookYaw += yawInput * sensitivity * timeslice;
            m_FreeLookPitch += pitchInput * sensitivity * timeslice;
            
            // Clamp free look angles
            if (m_FreeLookYaw > 180) m_FreeLookYaw -= 360;
            if (m_FreeLookYaw < -180) m_FreeLookYaw += 360;
            if (m_FreeLookPitch > 45) m_FreeLookPitch = 45;
            if (m_FreeLookPitch < -45) m_FreeLookPitch = -45;
        }
        else if (m_InFreeLook)
        {
            // Gradually return to default view
            m_InFreeLook = false;
            m_FreeLookYaw = Math.SmoothCD(m_FreeLookYaw, 0, m_AngularVelFreeLookYaw, 0.3, 1000, timeslice);
            m_FreeLookPitch = Math.SmoothCD(m_FreeLookPitch, 0, m_AngularVelFreeLookPitch, 0.3, 1000, timeslice);
        }
    }
    
    // Get current target info
    string GetTargetName()
    {
        if (m_TargetPlayer && m_TargetPlayer.GetIdentity())
        {
            return m_TargetPlayer.GetIdentity().GetName();
        }
        return "";
    }
    
    int GetCurrentTargetIndex()
    {
        return m_CurrentTargetIndex;
    }
    
    int GetValidTargetCount()
    {
        return m_ValidTargetSteamIds.Count();
    }
}

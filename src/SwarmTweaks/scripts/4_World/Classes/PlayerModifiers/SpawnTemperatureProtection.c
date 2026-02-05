/**
 * SpawnTemperatureProtection.c
 *
 * Prevents the player from entering "flash dark blue" (severe hypothermia)
 * during the first 5 minutes after spawning as a fresh spawn.
 *
 * This is achieved by clamping the HeatComfort value to prevent it from
 * going below the threshold that triggers dark blue indicator.
 */

modded class PlayerBase
{
    // Time in seconds for spawn protection (5 minutes)
    static const float SPAWN_TEMP_PROTECTION_DURATION = 300.0;

    // Minimum HeatComfort value allowed during protection
    // Flashing dark blue starts at approximately -0.9, we clamp at -0.8 to stay above threshold
    static const float SPAWN_TEMP_PROTECTION_MIN_HEAT_COMFORT = -0.8;

    // Track spawn time for temperature protection
    protected float m_SpawnTempProtectionTimer;
    protected bool m_HasSpawnTempProtection;

    void PlayerBase()
    {
        m_SpawnTempProtectionTimer = 0;
        m_HasSpawnTempProtection = false;
    }

    override void Init()
    {
        super.Init();

        // Only apply protection on server
        if (GetGame().IsServer())
        {
            // Check if this is a fresh spawn (new character)
            // We'll set the flag here, actual start happens in OnPlayerLoaded
            m_HasSpawnTempProtection = false;
        }
    }

    override void OnPlayerLoaded()
    {
        super.OnPlayerLoaded();

        if (GetGame().IsServer())
        {
            // Check if this is a fresh spawn by looking at playtime stat
            // Fresh spawns have very low playtime
            float playTime = StatGet(AnalyticsManagerServer.STAT_PLAYTIME);

            if (playTime < 10.0) // Less than 10 seconds = fresh spawn
            {
                m_HasSpawnTempProtection = true;
                m_SpawnTempProtectionTimer = SPAWN_TEMP_PROTECTION_DURATION;
            }
        }
    }

    override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
    {
        super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);

        // Update spawn protection timer on server
        if (GetGame().IsServer() && m_HasSpawnTempProtection)
        {
            m_SpawnTempProtectionTimer -= pDt;

            if (m_SpawnTempProtectionTimer <= 0)
            {
                m_HasSpawnTempProtection = false;
            }
            else
            {
                // Apply protection: clamp HeatComfort to prevent dark blue
                ApplySpawnTemperatureProtection();
            }
        }
    }

    protected void ApplySpawnTemperatureProtection()
    {
        float currentHeatComfort = GetStatHeatComfort().Get();

        if (currentHeatComfort < SPAWN_TEMP_PROTECTION_MIN_HEAT_COMFORT)
        {
            GetStatHeatComfort().Set(SPAWN_TEMP_PROTECTION_MIN_HEAT_COMFORT);
        }
    }

    // Public method to check if player has spawn protection
    bool HasSpawnTemperatureProtection()
    {
        return m_HasSpawnTempProtection;
    }

    // Get remaining protection time
    float GetSpawnTemperatureProtectionTimeRemaining()
    {
        return m_SpawnTempProtectionTimer;
    }
}

// SwarmArenaSpectatorSettings - Camera configuration

class SwarmArenaSpectatorSettings
{
    // 3rd person camera offset (relative to target head)
    // X = left/right, Y = up/down, Z = forward/back (negative = behind)
    static vector CameraOffset = "0 2.0 -3.5";
    
    // Camera smoothing (lower = faster response)
    static float SmoothTime = 0.15;
    
    // Camera collision offset from walls
    static float CollisionOffset = 0.3;
    
    // Mouse sensitivity for looking around
    static float MouseSensitivity = 35.0;
    
    // Angular smoothing
    static float AngularSmooth = 0.8;
}

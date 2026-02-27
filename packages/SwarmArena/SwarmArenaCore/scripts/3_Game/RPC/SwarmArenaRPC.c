// SwarmArenaRPC - RPC definitions for all SwarmArena modules
// Using unique offset (70000+) to avoid collisions with other mods

enum SwarmArenaRPC
{
    INVALID = 70000,
    
    // ==========================================
    // Lobby RPCs (handled by SwarmArenaLobby)
    // ==========================================
    Lobby_JoinQueue,            // Client->Server: Request to join queue
    Lobby_LeaveQueue,           // Client->Server: Request to leave queue
    Lobby_QueueStatus,          // Server->Client: Queue position update
    Lobby_QueueLeft,            // Server->Client: Confirm left queue
    
    // ==========================================
    // Party RPCs (handled by SwarmArenaLobby)
    // ==========================================
    Party_Create,               // Client->Server: Create new party
    Party_Invite,               // Client->Server: Invite player to party
    Party_InviteReceived,       // Server->Client: Invitation received
    Party_Accept,               // Client->Server: Accept party invite
    Party_Decline,              // Client->Server: Decline party invite
    Party_Leave,                // Client->Server: Leave current party
    Party_Kick,                 // Client->Server: Kick member (owner only)
    Party_Update,               // Server->Client: Party state sync
    Party_Disbanded,            // Server->Client: Party was disbanded
    
    // ==========================================
    // Match RPCs (handled by SwarmArenaMatch)
    // ==========================================
    Match_Found,                // Server->Client: Match found, prepare to start
    Match_Start,                // Server->Client: Match is starting
    Match_RoundStart,           // Server->Client: Round starting
    Match_RoundEnd,             // Server->Client: Round ended
    Match_End,                  // Server->Client: Match ended
    Match_PlayerDied,           // Server->Client: Player died notification
    Match_PlayerDeath,          // Internal: Player death event from PlayerBase
    Match_FreezeStart,          // Server->Client: Freeze time started
    Match_FreezeEnd,            // Server->Client: Freeze time ended
    Match_ScoreUpdate,          // Server->Client: Score changed
    Match_MoneyUpdate,          // Server->Client: Player money changed
    
    // ==========================================
    // Buy RPCs (handled by SwarmArenaMatch)
    // ==========================================
    Buy_RequestMenu,            // Client->Server: Request buy menu data
    Buy_Preset,                 // Client->Server: Buy a preset
    Buy_Armor,                  // Client->Server: Buy armor
    Buy_Result,                 // Server->Client: Purchase result
    
    // ==========================================
    // Preset RPCs (handled by SwarmArenaCore)
    // ==========================================
    Preset_Request,             // Client->Server: Request presets from API
    Preset_Sync,                // Server->Client: Sync presets to client
    Preset_Save,                // Client->Server: Save preset to API
    Preset_Delete,              // Client->Server: Delete preset from API
    Preset_SaveResult,          // Server->Client: Save operation result
    
    // ==========================================
    // Spectator RPCs (handled by SwarmArenaSpectator)
    // ==========================================
    Spectator_Enter,            // Server->Client: Enter spectator mode
    Spectator_Exit,             // Server->Client: Exit spectator mode
    Spectator_SwitchNext,       // Client->Server: Switch to next target
    Spectator_SwitchPrev,       // Client->Server: Switch to previous target
    Spectator_TargetUpdate,     // Server->Client: New spectator target assigned
    
    // ==========================================
    // Additional GUI RPCs
    // ==========================================
    Match_StateUpdate,          // Server->Client: Match state changed
    Lobby_QueueUpdate,          // Server->Client: Queue status update
    Lobby_MatchFound,           // Server->Client: Match found notification
    
    COUNT
}

// Queue type enum
enum SwarmArenaQueueType
{
    INVALID = 0,
    SOLO_1V1 = 1,
    SOLO_2V2 = 2,
    SOLO_3V3 = 3,
    PARTY_2V2 = 4,
    PARTY_3V3 = 5
}

// Match state enum
enum SwarmArenaMatchState
{
    INVALID = 0,
    INITIALIZING,
    FREEZE_TIME,
    ROUND_ACTIVE,
    ROUND_END,
    MATCH_END
}

// Armor type enum
enum SwarmArenaArmorType
{
    NONE = 0,
    LIGHT,
    MEDIUM,
    HEAVY
}

// Buy result enum
enum SwarmArenaBuyResult
{
    SUCCESS = 0,
    INSUFFICIENT_FUNDS,
    NOT_IN_FREEZE_TIME,
    INVALID_PRESET,
    INVALID_ARMOR,
    INVENTORY_FULL
}

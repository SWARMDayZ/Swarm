# Swarm - DayZ Mod Collection

A comprehensive collection of DayZ Standalone mods designed for server administration, gameplay enhancement, and quality-of-life improvements. Features modular design with automated build, validation, and publishing workflows.

## 📦 Included Mods

### SwarmTweaks
Gameplay modifications and quality-of-life improvements focused on item usability and reduced annoyances:

- **Shouldered Fishing Rod & Tripod** - Adds custom fishing rod (`Swarm_FishingRod`) and tripod (`Swarm_Tripod`) that can be carried in shoulder slot alongside other items
- **No Bury Large Bags** - Prevents burying large bags (like the Lantia suit) in the ground for gameplay balance
- **Less Laugh** - Reduces laughing frequency from BrainDisease to be less annoying
- **Less Sneeze** - Reduces sneezing frequency from Influenza to be less disruptive
- **Improvised Suppressor Crafting** - Requires a full duct tape (instead of partial) to craft improvised suppressors for balancing

**Dependencies:** DZ_Data, DZ_Scripts, DZ_Gear_Tools, DZ_Gear_Cooking

### SwarmAnimals
Modular animal drop modifications with clean, expandable architecture. Modifies harvesting/skinning drops for:

- Wolf (Canis Lupus)
- Goat (Capra Hircus - Male & Female variants)
- Sheep (Ovis Aries - Male & Female variants)
- Reindeer (Rangifer Tarandus - Male & Female variants)
- Wild Boar (Sus Scrofa)
- Brown Bear (Ursus Arctos)
- Polar Bear (Ursus Maritimus)

Each animal has its own dedicated config for easy customization and future expansion.

**Dependencies:** DZ_Animals

### SwarmEarplugs
Repack of the original [EarPlugs mod by DaemonF0rge](https://github.com/DaemonForge/DayZ-EarPlugs/) with Swarm branding. Client-side volume control system:

- **Volume Levels** - Cycle through Full, Mid, Low, and Off volume levels
- **In-Game Widget** - Visual indicator showing current volume state
- **Custom Keybind** - Configurable hotkey via inputs.xml
- **Game Audio Control** - Reduces master volume for immersive RP scenarios or hearing protection

**Credits:** Original mod by DaemonF0rge. Thanks for allowing community repacks!

**Dependencies:** Mission module only (client-side)

### SwarmSpectator
Enhanced spectator camera system for filming and content creation. Extends [Community Online Tools (COT)](https://github.com/Jacob-Mango/DayZ-CommunityOnlineTools) spectator functionality:

- **Delete Marker** - Remove camera markers from spectator view
- **Delete Light** - Remove light sources for cleaner filming
- **Lock-On Target** - Enhanced camera locking on entities
- **Custom Inputs** - Additional keybinds for camera control via inputs.xml

**Purpose:** Provides server admins and content creators with advanced camera controls for filming cinematic footage, events, and documentation.

**Dependencies:** JM_COT_Scripts (Community Online Tools), CF (Community Framework)  
**Note:** This is NOT a repack - it requires COT to be installed and only overrides/extends the spectator camera functionality.

### SwarmObserver
Server-side moderation and player activity tracking tool:

- **Restricted Area Logout Menu** - Custom logout interface for designated zones
- **Combat Logout Tracking** - Silent monitoring of combat logging behavior (provides evidence for admin action)
- **Player Activity Monitoring** - Tools for server administrators to observe player behavior

**Purpose:** Moderation tool for server admins to track and prevent rule violations without disrupting gameplay.

**Dependencies:** DZ_Data, DZ_Scripts (Game, World, Mission modules)

## 🏗️ Project Structure

```
Swarm/
├── packages/                     # Source packages (each folder = one mod)
│   ├── Swarm/                    # Main Swarm mod
│   │   ├── meta.cpp              # Mod metadata template
│   │   ├── .version              # Package-specific version (optional)
│   │   ├── .workshop_id          # Package-specific Workshop ID (optional)
│   │   ├── SwarmTweaks/          # PBO: Gameplay tweaks
│   │   ├── SwarmAnimals/         # PBO: Animal drops
│   │   ├── SwarmEarplugs/        # PBO: Volume control
│   │   ├── SwarmSpectator/       # PBO: Enhanced spectator
│   │   └── SwarmObserver/        # PBO: Moderation tools
│   ├── SwarmArena/               # Arena mod (separate package)
│   └── SwarmTest/                # Test mod (separate package)
│       ├── meta.cpp
│       └── Core/
├── dist/                         # Build output (gitignored)
│   ├── @Swarm/                   # Built Swarm mod
│   │   ├── Addons/               # Compiled PBO files
│   │   ├── Keys/                 # Public signing key (.bikey)
│   │   └── meta.cpp              # Processed metadata
│   ├── @SwarmArena/              # Built SwarmArena mod
│   └── @SwarmTest/               # Built SwarmTest mod
├── keys/                         # Signing keys directory
│   ├── Swarm.bikey               # Public key (committed)
│   └── Swarm.biprivatekey        # Private key (gitignored)
├── scripts/                      # Build tools and utilities
│   ├── load_env.ps1              # PowerShell .env parser
│   ├── update_version.ps1        # Version preprocessor
│   ├── build_prompt.ps1          # Interactive build menu
│   ├── publish_prompt.ps1        # Interactive publish menu
│   └── steamcmd.exe              # SteamCMD for Workshop publishing
├── validate/                     # Script validation environment
│   ├── mods/                     # Dependency mods (CF, COT, etc.)
│   ├── serverDZ.cfg              # Custom server config (gitignored)
│   └── serverDZ.default.cfg      # Default validation config
├── build.bat                     # Interactive build/validate/sign/launch
├── publish.bat                   # Interactive publish workflow
├── .build_settings               # Persisted build selections (gitignored)
├── .version                      # Root version fallback
└── .env                          # Environment configuration (gitignored)
```

### Package Structure

Each folder inside `packages/` represents a **mod**. Each subfolder inside a mod becomes a **PBO**:

```
packages/
├── Swarm/                  → builds to dist/@Swarm/
│   ├── meta.cpp            → processed and copied to dist/@Swarm/meta.cpp
│   ├── SwarmTweaks/        → builds to dist/@Swarm/Addons/SwarmTweaks.pbo
│   ├── SwarmAnimals/       → builds to dist/@Swarm/Addons/SwarmAnimals.pbo
│   └── ...
├── SwarmArena/             → builds to dist/@SwarmArena/
└── SwarmTest/              → builds to dist/@SwarmTest/
```

## 🔧 Prerequisites

### Required
- **DayZ Tools** - Install via Steam (free with DayZ ownership)
- **Windows** - Build scripts are Windows batch files (PowerShell 5.1+)

### Optional
- **DayZ Server** - Required for validation and launch (via `build.bat`)
- **DayZ Client** - Required for launch (via `build.bat`)
- **SteamCMD** - Required for `publish.bat` workflow

### Dependency Mods (for validation/testing)
If your mods depend on other mods, place them in `validate/mods/`:
```
validate/mods/@CF/
validate/mods/@Community-Online-Tools/
```

## ⚙️ Environment Setup

### Configuration File (.env)

Create a `.env` file in the project root with your paths:

```ini
# ============================================
# DayZ Tools (REQUIRED for build/publish)
# ============================================
DAYZ_TOOLS=G:\SteamLibrary\steamapps\common\DayZ Tools

# ============================================
# DayZ Server (REQUIRED for validate/launch)
# ============================================
DAYZ_SERVER=G:\SteamLibrary\steamapps\common\DayZServer

# ============================================
# DayZ Client (REQUIRED for launch via build.bat)
# ============================================
DAYZ_CLIENT=G:\SteamLibrary\steamapps\common\DayZ

# ============================================
# Steam Workshop Publishing
# ============================================
# Steam account username for SteamCMD
STEAM_USERNAME=your_steam_username

# Workshop item ID (get this after first manual publish)
WORKSHOP_ID=1234567890

# ============================================
# Development Options
# ============================================
# Dependency mods to load (semicolon-separated)
MODS=@CF;@Community-Online-Tools

# Client profile directory (optional, for launch via build.bat)
DAYZ_PROFILE=C:\Users\YourName\Documents\DayZ
```

### Environment Variable Details

| Variable | Required For | Description |
|----------|--------------|-------------|
| `DAYZ_TOOLS` | build, publish | Path to DayZ Tools installation. Used for AddonBuilder.exe, DSSignFile.exe, and DSCreateKey.exe |
| `DAYZ_SERVER` | validate, launch | Path to DayZ Server. Required to run script validation and local test server |
| `DAYZ_CLIENT` | launch | Path to DayZ client installation for local testing |
| `STEAM_USERNAME` | publish | Your Steam account username for SteamCMD authentication |
| `WORKSHOP_ID` | publish | Steam Workshop item ID (obtained after first publish) |
| `MODS` | validate, launch | List of dependency mods to load, separated by semicolons (e.g., `@CF;@COT`) |
| `DAYZ_PROFILE` | launch | Optional: Path to DayZ profile directory to copy settings from |

### Alternative: System Environment Variables

Instead of using `.env`, you can set system environment variables:

```batch
setx DAYZ_TOOLS "G:\SteamLibrary\steamapps\common\DayZ Tools"
setx DAYZ_SERVER "G:\SteamLibrary\steamapps\common\DayZServer"
setx DAYZ_CLIENT "G:\SteamLibrary\steamapps\common\DayZ"
```

**Note:** System variables take precedence over `.env` file values.

## 🔨 Build Workflow (build.bat)

Interactive build system with package selection, validation, signing, and launch - all in one script. Replaces the old `build.bat`, `validate.bat`, and `launch.bat`.

### Interactive Prompts

When you run `build.bat`, you get an interactive menu:

**Step 1: Reuse Settings** (if previous settings exist)
```
Do you want to build with latest settings?

  Packages: Swarm, SwarmTest
  Params:   validate, sign, launch

(y/n) >
```

**Step 2: Package Selection**
```
What mod do you want to build?

(*) packages/Swarm          <
( ) packages/SwarmArena
( ) packages/SwarmTest

(use space to toggle, enter to validate)
```

**Step 3: Action Selection**
```
What should we do with the build?

(*) validate using server validation  <
(*) sign the pbos in the mods
(*) launch testing server & client

(use space to toggle, enter to validate)
```

### How It Works

1. **Environment Loading** - Loads `DAYZ_TOOLS`, `DAYZ_SERVER`, `DAYZ_CLIENT`, `MODS`, `DAYZ_PROFILE` from `.env`
2. **Package Discovery** - Scans `packages/` for mod folders
3. **Settings Persistence** - Saves/loads selections to `.build_settings` for quick reuse
4. **Interactive Selection** - PowerShell-based multi-select menus (arrow keys + space + enter)
5. **Build** - For each selected package, builds every subfolder into a PBO using AddonBuilder
6. **Version Injection** - Runs `update_version.ps1` to replace `%VERSION%` placeholders
7. **Metadata Processing** - Processes `meta.cpp` with version, timestamp, and Workshop ID
8. **Sign** *(optional)* - Signs all PBOs with private key, copies public key to mod's `Keys/` folder
9. **Validate** *(optional)* - Launches DayZ Server with built mods, checks RPT logs for script errors
10. **Launch** *(optional)* - Starts DayZ Server + Client with auto-connect for testing

### Usage

```batch
# Run with interactive prompts
build.bat
```

No command-line flags needed - everything is selected interactively.

### Output

Each selected package builds to its own `dist/@PackageName/` directory:
- PBO files: `dist/@Swarm/Addons/*.pbo`
- Signatures: `dist/@Swarm/Addons/*.bisign`
- Public key: `dist/@Swarm/Keys/Swarm.bikey`
- Metadata: `dist/@Swarm/meta.cpp`

### Version Management

Versions are read per-package:
1. `packages/Swarm/.version` - Package-specific version (takes priority)
2. `.version` - Root fallback version

### Settings Persistence

Your last selections are saved to `.build_settings` and offered for quick reuse on the next run. Press `y` to rebuild with the same packages and actions instantly.

## 📤 Publish Workflow (publish.bat)

Interactive pipeline for building, validating, signing, and publishing mods to Steam Workshop.

### Interactive Prompts

**Step 1: Package Selection**
```
What mod do you want to publish?

(*) packages/Swarm          <
( ) packages/SwarmArena
( ) packages/SwarmTest

(use space to toggle, enter to validate)
```

**Step 2: Version Input** (per selected package)
```
What version for packages/Swarm ?
version [1.5.0]: 1.5.1

What version for packages/SwarmTest ?
version [1.0.0]:
```

Press Enter to keep the current version, or type a new one.

### Pipeline (per package)

For each selected package, the publish script runs a 5-step pipeline:

| Step | Action | Details |
|------|--------|---------|
| 1/5 | **Update version** | Saves new version to `packages/ModName/.version` |
| 2/5 | **Build** | Compiles all subfolders into PBOs, processes `meta.cpp` |
| 3/5 | **Validate** | Launches DayZ Server, checks logs for script errors |
| 4/5 | **Sign** | Signs all PBOs with private key, copies public key |
| 5/5 | **Publish** | Uploads to Steam Workshop via SteamCMD |

If validation fails, the pipeline stops - no broken mods get published.

### Usage

```batch
# Run with interactive prompts
publish.bat
```

### Workshop ID

Each package can have its own Workshop ID:

1. **Per-package** - Create `packages/Swarm/.workshop_id` containing the ID
2. **Global fallback** - Set `WORKSHOP_ID=1234567890` in `.env`

### Prerequisites for Publishing

1. **SteamCMD** - Place `steamcmd.exe` in `scripts/`
2. **Workshop Item** - Create manually in DayZ Tools Publisher first, note the ID
3. **Steam Credentials** - Set `STEAM_USERNAME` in `.env`, run SteamCMD once to cache login

### SteamCMD Authentication

#### Initial Login
```batch
cd scripts
steamcmd.exe +login your_steam_username +quit
```

This caches your credentials in `scripts/config/loginusers.vdf`.

#### Steam Guard
- If Steam Guard is enabled, you'll be prompted for a code during first login
- The code is sent to your email or mobile app
- After successful authentication, credentials are cached
- Future `publish.bat` runs will use cached credentials automatically

### Signing Keys

On first publish, the script automatically creates a key pair:
- **Private Key:** `keys/Swarm.biprivatekey` - Keep this SECRET (gitignored)
- **Public Key:** `keys/Swarm.bikey` - Share with server owners (committed to repo)

The public key is automatically copied to each package's `dist/@PackageName/Keys/` for Workshop distribution.

## 🚀 Launch & Validate

Launch and validation are now integrated directly into `build.bat` as selectable actions - no standalone scripts needed.

When you select **launch** in the action menu, `build.bat`:
1. Kills any existing DayZ processes
2. Sets up `validate/serverDZ.cfg` or `validate/serverDZ.default.cfg`
3. Creates `.launch_temp/` for server/client profiles
4. Enables BattleEye automatically
5. Copies your profiles from `DAYZ_PROFILE` (if set)
6. Loads dependency mods from `validate/mods/` or `MODS` env variable
7. If COT is detected, creates full admin permissions for "everyone" role
8. Starts DayZ Server, waits, then starts DayZ Client with auto-connect to `127.0.0.1`

When you select **validate**, the script:
1. Launches DayZ Server with all built mods loaded
2. Waits for scripts to initialize
3. Scans RPT and crash logs for script errors, compile errors, and fatal errors
4. Reports results and preserves logs in `.validate_temp/`

### Server Configuration

Create `validate/serverDZ.cfg` for a custom map:

```cpp
hostname = "Dev Test Server";
Missions = {
    class Namalsk {
        template = "dayzOffline.namalsk";
    };
};
```

If no custom config exists, `validate/serverDZ.default.cfg` (vanilla Chernarus) is used.

### Dependency Mods

Place dependency mods in `validate/mods/` or set in `.env`:

```ini
MODS=@CF;@Community-Online-Tools;@DeerIsle
```

Dependencies are loaded before your packages to ensure proper load order.

### Logs

- **Server Logs** - `.launch_temp/*.RPT`, `.launch_temp/*.ADM`
- **Client Logs** - `.launch_temp/client/Users/DevClient/*.RPT`
- **Validation Logs** - `.validate_temp/*.RPT`, `.validate_temp/crash_*.log`

## 🛠️ Development Tips

### Quick Development Cycle

```batch
# 1. Build with validation and launch
build.bat
#    → select your packages
#    → select: validate, sign, launch

# 2. Make changes to packages/ files

# 3. Re-run build.bat → press 'y' to reuse settings
build.bat
```

### Build Only (No Launch)

```batch
# Build and sign only (no server launch)
build.bat
#    → select packages
#    → select: validate, sign
```

**Note:** Launching requires both **sign** and **validate** - unsigned PBOs won't load, and validation ensures scripts compile before starting the server.

### Debugging Script Errors

If validation fails:

1. **Check preserved logs** - `.validate_temp/` contains RPT, crash, and script logs
2. **Check launch logs** - `.launch_temp/` contains full server and client logs
3. **Increase timeout** - Edit the validation timeout in `build.bat` if scripts need more time to load

### Version Management

Versions are managed per-package:
- `packages/Swarm/.version` - Package-specific version
- `.version` - Root fallback

The publish workflow prompts you to set versions interactively. Build workflow reads existing versions for metadata injection.

## 📝 Scripting Details

### Environment Variable Loading (load_env.ps1)

PowerShell script that parses `.env` files:

```powershell
# Usage from batch files
powershell -NoProfile -ExecutionPolicy Bypass -File "scripts\load_env.ps1" -EnvFile ".env" -VarName DAYZ_TOOLS
```

**Features:**
- Skips empty lines and comments (lines starting with `#`)
- Removes surrounding quotes from values
- Returns single variable value or all variables as `KEY=VALUE` lines
- Silent failure (returns nothing if file or variable not found)

### Version Preprocessing (update_version.ps1)

Copies source files to temporary directory and replaces `%VERSION%` placeholders:

```powershell
# Usage from bat files
powershell -NoProfile -ExecutionPolicy Bypass -File "scripts\update_version.ps1" -SourceDir "packages\Swarm\SwarmTweaks" -TempDir ".build_temp\SwarmTweaks" -Version "1.2.3"
```

**Replacements:**
- `%VERSION%` → Actual version string (e.g., "1.2.3")
- Used in `config.cpp` files for mod version display

### Meta.cpp Placeholders

Each package's `meta.cpp` uses these placeholders:

```cpp
version = "%VERSION%";         // Replaced with package version
timeStamp = %TIMESTAMP%;       // Replaced with DateTime.UtcNow.Ticks
publishedid = %PUBLISHEDID%;   // Replaced with WORKSHOP_ID
```

**Processing:**
- `build.bat` replaces all placeholders when copying to `dist/@PackageName/meta.cpp`
- Timestamp format is .NET ticks for Steam Workshop compatibility
- Published ID defaults to `0` if no Workshop ID is configured

## 🤝 Contributing

### Adding a New Mod (Package)

1. Create a new folder in `packages/` (e.g., `packages/SwarmNewMod/`)
2. Add subfolders for each PBO (e.g., `packages/SwarmNewMod/MyFeature/`)
3. Add `config.cpp` with mod configuration in each subfolder
4. Add scripts in appropriate module folders (`3_Game/`, `4_World/`, `5_Mission/`)
5. Add `meta.cpp` at the package root with `%VERSION%`, `%TIMESTAMP%`, `%PUBLISHEDID%` placeholders
6. Build and test:
   ```batch
   build.bat
   # Select your new package → validate + launch
   ```

### Adding a New PBO to an Existing Mod

1. Create a subfolder in the package (e.g., `packages/Swarm/SwarmNewFeature/`)
2. Add `config.cpp` and script files
3. Rebuild - the new folder is automatically discovered and compiled as a PBO

### Testing Changes

Always validate before publishing:

```batch
# Build with validation
build.bat
# Select packages → validate

# Or publish (includes validation in the pipeline)
publish.bat
```

## 📄 License

See license file.
SwarmEarplugs is a repack of the original EarPlugs mod by DaemonF0rge.

## 🙏 Credits

- **SwarmEarplugs** - Original [EarPlugs mod by DaemonF0rge](https://github.com/DaemonForge/DayZ-EarPlugs/)
- **SwarmSpectator** - Extends [Community Online Tools by Jacob-Mango](https://github.com/Jacob-Mango/DayZ-Community-Online-Tools)
- **Swarm Team** - Kize, Arky and contributors

---

**Last Updated:** February 2026  
**Current Version:** See `.version` file  
**Workshop:** Check your `WORKSHOP_ID` in `.env`

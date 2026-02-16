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
Enhanced spectator camera system for filming and content creation. Extends [Community Online Tools (COT)](https://github.com/Jacob-Mango/DayZ-Community-Online-Tools) spectator functionality:

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
├── src/                          # Source code for all mods
│   ├── meta.cpp                  # Mod metadata template
│   ├── SwarmTweaks/              # Gameplay tweaks
│   │   ├── config.cpp
│   │   └── scripts/4_World/      # World module overrides
│   ├── SwarmAnimals/             # Animal drops (modular structure)
│   │   ├── config.cpp
│   │   ├── canis_lupus/config.cpp
│   │   ├── capra_hircus/config.cpp
│   │   ├── ovis_aries/config.cpp
│   │   └── ...
│   ├── SwarmEarplugs/            # Volume control
│   │   ├── config.cpp
│   │   ├── data/inputs.xml
│   │   └── scripts/5_Mission/
│   ├── SwarmSpectator/           # Enhanced spectator
│   │   ├── config.cpp
│   │   ├── data/inputs.xml
│   │   └── scripts/3_Game, 4_World, 5_Mission/
│   └── SwarmObserver/            # Moderation tools
│       ├── config.cpp
│       └── scripts/3_Game, 4_World, 5_Mission/
├── dist/@Swarm/                  # Build output (gitignored)
│   ├── Addons/                   # Compiled PBO files
│   ├── Keys/                     # Public signing key (.bikey)
│   ├── meta.cpp                  # Processed metadata
│   └── mod.cpp                   # Workshop metadata
├── keys/                         # Signing keys directory
│   ├── Swarm.bikey               # Public key (committed)
│   └── Swarm.biprivatekey        # Private key (gitignored)
├── scripts/                      # Build tools and utilities
│   ├── load_env.ps1              # PowerShell .env parser
│   ├── update_version.ps1        # Version preprocessor
│   └── steamcmd.exe              # SteamCMD for Workshop publishing
├── validate/                     # Script validation environment
│   ├── mods/                     # Dependency mods (CF, COT, etc.)
│   │   ├── @CF/
│   │   └── @Community-Online-Tools/
│   ├── serverDZ.cfg              # Custom server config (gitignored)
│   └── serverDZ.default.cfg      # Default validation config
├── build.bat                     # Build all mods
├── validate.bat                  # Script validation
├── publish.bat                   # Full publish workflow
├── launch.bat                    # Local dev environment launcher
└── .env                          # Environment configuration (gitignored)
```

## 🔧 Prerequisites

### Required
- **DayZ Tools** - Install via Steam (free with DayZ ownership)
- **Windows** - Build scripts are Windows batch files (PowerShell 5.1+)

### Optional
- **DayZ Server** - Required for `validate.bat` and `launch.bat`
- **DayZ Client** - Required for `launch.bat` only
- **SteamCMD** - Required for automated `publish.bat` workflow

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
# DayZ Client (REQUIRED for launch.bat only)
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

# Client profile directory (optional, for launch.bat)
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

Compiles all source packages into PBO files using DayZ Tools AddonBuilder.

### How It Works

1. **Environment Loading** - Loads `DAYZ_TOOLS` from `.env` or environment variables
2. **Version Management** - Uses `--version` flag or reads from `.version` file
3. **Preprocessing** - Runs `update_version.ps1` to replace `%VERSION%` placeholders in all source files
4. **Compilation** - Invokes AddonBuilder for each folder in `src/` directory
5. **Metadata Processing** - Copies and processes `meta.cpp` with version, timestamp, and Workshop ID
6. **Optional Signing** - Signs PBOs with private key if `--sign` flag is specified
7. **Optional Validation** - Runs `validate.bat` if `--validate` flag is specified

### Usage

```batch
# Basic build with version number
build.bat --version 1.2.3

# Build with signing
build.bat --version 1.2.3 --sign

# Build with validation (requires DayZ Server)
build.bat --version 1.2.3 --validate

# Build with custom validation timeout
build.bat --version 1.2.3 --validate --timeout 30

# Build without specifying version (uses .version file)
build.bat
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--version X.X.X` | Version number for this build (updates `.version` file) |
| `--sign` | Sign all PBO files with private key after building |
| `--validate` | Run script validation after successful build |
| `--timeout N` | Validation timeout in seconds (default: 15) |

### Output

- PBO files: `dist/@Swarm/Addons/*.pbo`
- Meta file: `dist/@Swarm/meta.cpp` (processed with version/timestamp/Workshop ID)
- Version file: `.version` (stores last used version)

### Build Process Details

1. **Folder Discovery** - Scans `src/` for subdirectories (each becomes a PBO)
2. **Version Injection** - PowerShell preprocessor finds all `%VERSION%` placeholders and replaces them
3. **AddonBuilder Execution** - Each folder is compiled with `-clear -packonly` flags
4. **Meta.cpp Processing** - Replaces `%VERSION%`, `%TIMESTAMP%`, and `%PUBLISHEDID%` in meta.cpp
5. **Timestamp Format** - Uses .NET DateTime.UtcNow.Ticks format for Workshop compatibility

## ✅ Validation Workflow (validate.bat)

Tests compiled scripts by launching a DayZ Server instance and checking logs for errors.

### How It Works

1. **Environment Loading** - Loads `DAYZ_SERVER` and `MODS` from `.env`
2. **Build Verification** - Checks if `dist/@Swarm/Addons` exists
3. **Config Selection** - Uses `validate/serverDZ.cfg` (custom) or `validate/serverDZ.default.cfg` (default Chernarus)
4. **Mod List Building** - Loads dependency mods from `validate/mods/` or `MODS` env variable, then adds `@Swarm` last
5. **Server Launch** - Starts DayZServer_x64.exe in background with all mods loaded
6. **Timeout Wait** - Waits specified seconds for server to load and initialize scripts
7. **Log Analysis** - Scans RPT and crash logs for script errors, compile errors, and fatal errors
8. **Process Termination** - Kills server process and analyzes final logs
9. **Error Reporting** - Displays crash logs, script logs, and error lines from RPT files if errors found

### Usage

```batch
# Run validation with default timeout (15 seconds)
validate.bat

# Run validation with longer timeout
validate.bat --timeout 30

# Skip build check (assume mods are already built)
validate.bat --skip-build

# Show help
validate.bat --help
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--timeout N` | Server run duration in seconds (default: 15) |
| `--skip-build` | Skip checking if mod is built |
| `--help` | Display detailed help information |

### Server Configuration

The script looks for configs in this order:

1. `validate/serverDZ.cfg` - Custom config (your specific map, mods, settings)
2. `validate/serverDZ.default.cfg` - Default config (vanilla Chernarus offline)

To validate on a different map, copy `serverDZ.default.cfg` to `serverDZ.cfg` and modify the `Missions` class:

```cpp
Missions = {
    class MyCustomMap {
        template = "dayzOffline.enoch";  // or .namalsk, .takistanplus, etc.
    };
};
```

### Dependency Mods

Place dependency mods in `validate/mods/`:

```
validate/mods/@CF/
validate/mods/@Community-Online-Tools/
validate/mods/@DeerIsle/
```

Or specify in `.env`:

```ini
MODS=@CF;@Community-Online-Tools;@DeerIsle
```

The script automatically loads dependencies BEFORE `@Swarm` to ensure proper load order.

### Error Detection

The validation script detects:

- **Crash Logs** - `crash_*.log` files
- **Crash Dumps** - `*.mdmp` minidump files
- **Script Errors** - RPT entries containing "SCRIPT ERROR", "SCRIPT (E)", "Compile error"
- **Fatal Errors** - "Fatal error", "ErrorModule", "Cannot compile"

If errors are found, logs are preserved at `.validate_temp/` for inspection.

### Exit Codes

- `0` - Validation passed, no errors detected
- `1` - Validation failed, script errors or crashes detected

## 📤 Publish Workflow (publish.bat)

Complete pipeline for building, validating, signing, and publishing to Steam Workshop.

### How It Works

This is a **4-step automated pipeline**:

#### Step 1: Build
- Runs `build.bat --version X.X.X` to compile all mods
- Preprocesses version placeholders
- Generates PBO files in `dist/@Swarm/Addons/`
- Skip with `--skip-build` if already built

#### Step 2: Validate
- Runs `validate.bat --skip-build` to check for script errors
- Launches DayZ Server with all mods loaded
- Analyzes RPT logs for compile errors
- Skip with `--skip-validate` (not recommended)

#### Step 3: Sign
- Creates signing keys if they don't exist (using DSCreateKey.exe)
- Signs all PBO files with private key (using DSSignFile.exe)
- Generates `.bisign` signature files
- Copies public `.bikey` to `dist/@Swarm/Keys/`
- Skip with `--skip-sign`

#### Step 4: Publish
- Creates `mod.cpp` with version and metadata
- Generates Workshop VDF configuration file
- Logs into Steam via SteamCMD
- Uploads mod files to Workshop item
- Skip with `--skip-publish` (useful for build+sign only)

### Prerequisites for Publishing

Before first publish, you MUST:

1. **Install SteamCMD**
   - Download from: https://developer.valvesoftware.com/wiki/SteamCMD
   - Extract `steamcmd.exe` to `scripts/steamcmd.exe`

2. **Initialize SteamCMD and Cache Credentials**
   ```batch
   cd scripts
   steamcmd.exe +login your_steam_username +quit
   ```
   - Enter your password
   - Enter Steam Guard code if prompted
   - Credentials are cached for future automated runs
   - **Important:** You only need to do this once, or when changing passwords

3. **Create Workshop Item Manually**
   - Open DayZ Tools → Publisher
   - Click "New" and fill in mod details
   - Upload initial version manually
   - Note the **Workshop ID** from the URL (e.g., `https://steamcommunity.com/sharedfiles/filedetails/?id=1234567890`)
   - Add `WORKSHOP_ID=1234567890` to your `.env` file

4. **Configure .env File**
   ```ini
   DAYZ_TOOLS=G:\SteamLibrary\steamapps\common\DayZ Tools
   DAYZ_SERVER=G:\SteamLibrary\steamapps\common\DayZServer
   STEAM_USERNAME=your_steam_username
   WORKSHOP_ID=1234567890
   ```

### Usage

```batch
# Full publish pipeline (build → validate → sign → publish)
publish.bat --version 1.2.3

# Publish with changelog
publish.bat --version 1.2.3 --changelog "Fixed bug with fishing rod shouldering"

# Publish with custom signing key
publish.bat --version 1.2.3 --key MyCustomKey

# Dry run (show what would happen without doing it)
publish.bat --version 1.2.3 --dry-run

# Skip validation (faster, but risky)
publish.bat --version 1.2.3 --skip-validate

# Build and sign only, don't publish
publish.bat --version 1.2.3 --skip-publish

# Use existing build, just sign and publish
publish.bat --version 1.2.3 --skip-build
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--version X.X.X` | **Required** - Version number to build and publish |
| `--key KeyName` | Key name for signing (default: "Swarm") |
| `--changelog "text"` | Change note displayed on Workshop (default: "Version X.X.X") |
| `--workshop-id ID` | Override Workshop ID from .env |
| `--skip-build` | Skip build step (use existing PBOs) |
| `--skip-validate` | Skip script validation (not recommended) |
| `--skip-sign` | Skip signing step |
| `--skip-publish` | Build and sign only, don't upload to Workshop |
| `--dry-run` | Show what would be done without executing |

### Signing Keys

#### First-Time Setup
On first run, `publish.bat` automatically creates a key pair:
- **Private Key:** `keys/Swarm.biprivatekey` - Keep this SECRET (gitignored)
- **Public Key:** `keys/Swarm.bikey` - Share with server owners (committed to repo)

#### Key Management
- **Never share your `.biprivatekey` file** - It's used to prove authenticity
- **Always share your `.bikey` file** - Server owners need this to verify your mods
- The public key is automatically copied to `dist/@Swarm/Keys/` for Workshop distribution
- Custom key names: Use `--key CustomName` to create/use different key pairs

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

#### Troubleshooting
- **Wrong password?** - Run the manual login command again to update cached credentials
- **Steam Guard timeout?** - Re-run the login command and enter code faster
- **Two-Factor Required?** - Use the Steam Mobile Authenticator app for instant codes

### Exit Codes

- `0` - Published successfully
- `Non-zero` - Error occurred (build failed, validation failed, signing failed, or upload failed)

### Output

After successful publish:
- **Workshop URL** - `https://steamcommunity.com/sharedfiles/filedetails/?id=YOUR_ID`
- **Signed PBOs** - `dist/@Swarm/Addons/*.pbo` and `*.bisign`
- **Public Key** - `dist/@Swarm/Keys/Swarm.bikey`
- **Mod Metadata** - `dist/@Swarm/mod.cpp` and `meta.cpp`

## 🚀 Launch Workflow (launch.bat)

Launches local DayZ Server and Client for development testing with automatic configuration.

### How It Works

1. **Environment Loading** - Loads paths from `.env` (DAYZ_SERVER, DAYZ_CLIENT, DAYZ_TOOLS, MODS, DAYZ_PROFILE)
2. **Process Cleanup** - Kills existing DayZ processes to free up files and ports
3. **Optional Build** - Runs `build.bat` and signs PBOs if `--build` flag specified
4. **Config Selection** - Uses `validate/serverDZ.cfg` or `validate/serverDZ.default.cfg`
5. **Temp Directory Setup** - Creates `.launch_temp/` for server profiles and client profiles
6. **BattleEye Configuration** - Automatically enables BattleEye in server config for stability
7. **Profile Copying** - Copies existing DayZ profiles from `DAYZ_PROFILE` to temp client directory (preserves settings)
8. **Mod List Building** - Loads dependencies from `validate/mods/` or `MODS` env variable, adds `@Swarm` last
9. **COT Permission Setup** - If Community Online Tools is detected, creates full admin permissions for "everyone" role
10. **Server Launch** - Starts DayZServer_x64.exe in new window with full logging
11. **Client Launch** - After 10-second wait, starts DayZ_BE.exe with auto-connect to local server

### Usage

```batch
# Launch server and client (default port 2302)
launch.bat

# Launch with custom port
launch.bat --port 2303

# Build, sign, and launch
launch.bat --build

# Build with custom port
launch.bat --build --port 2303

# Show help
launch.bat --help
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--port N` | Server port (default: 2302) |
| `--build` | Build and sign mod before launching |
| `--help` | Display help information |

### Server Configuration

Same as `validate.bat` - uses `validate/serverDZ.cfg` or `validate/serverDZ.default.cfg`.

### Automatic Features

#### BattleEye Auto-Enable
The script automatically adds `BattlEye = 1;` to the server config for proper client connection.

#### COT Admin Auto-Setup
If Community Online Tools is detected in the mod list, the script creates:
- `PermissionsFramework/Roles/everyone.txt` with full admin permissions
- All COT permissions granted to "everyone" role (no need to manually set up admins)
- Includes camera, ESP, teleport, weather, vehicle, loadout, and Namalsk event controls

#### Profile Preservation
If `DAYZ_PROFILE` is set, the script copies your existing:
- Client settings (`*.DayZProfile`)
- Graphics settings (`*.cfg`)
- Input settings (`*.xml`)

### Logs and Profiles

Everything is stored in `.launch_temp/`:
- **Server Logs** - `.launch_temp/*.RPT`, `.launch_temp/*.ADM`, `.launch_temp/*.log`
- **Client Logs** - `.launch_temp/client/Users/DevClient/*.RPT`
- **Server Profiles** - `.launch_temp/PermissionsFramework/`, etc.

### Auto-Connect

Client automatically connects to `127.0.0.1:PORT` using `-connect` and `-port` parameters.

### Stopping

- Close the server and client windows manually
- Or use Task Manager to kill `DayZServer_x64.exe` and `DayZ_x64.exe`
- Or run `launch.bat` again (it kills existing processes first)

## 🛠️ Development Tips

### Quick Development Cycle

```batch
# 1. Build with validation
build.bat --version 1.0.0 --validate

# 2. Launch for testing
launch.bat

# 3. Make changes to src/ files

# 4. Quick rebuild and relaunch
launch.bat --build
```

### Testing Without Validation

```batch
# Faster builds (skips validation)
build.bat --version 1.0.0

# Launch directly
launch.bat
```

### Version Management

The `.version` file stores the last used version:
```batch
# First build creates .version file
build.bat --version 1.0.0

# Subsequent builds can omit version
build.bat

# Update version explicitly
build.bat --version 1.1.0
```

### Debugging Script Errors

If validation fails:

1. **Check preserved logs** - `.validate_temp/` directory contains:
   - `*.RPT` - Full server logs
   - `crash_*.log` - Crash details
   - `script_*.log` - Script-specific logs

2. **Run with longer timeout** - Give server more time to load:
   ```batch
   validate.bat --timeout 60
   ```

3. **Test manually** - Launch server yourself:
   ```batch
   launch.bat
   # Check logs in .launch_temp/
   ```

### Testing Different Maps

Create `validate/serverDZ.cfg` with your custom map:

```cpp
hostname = "Dev Test Server";
Missions = {
    class Namalsk {
        template = "dayzOffline.namalsk";
    };
};
```

Then run validation or launch as normal.

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
# Usage from build.bat
powershell -NoProfile -ExecutionPolicy Bypass -File "scripts\update_version.ps1" -SourceDir "src\SwarmTweaks" -TempDir ".build_temp\SwarmTweaks" -Version "1.2.3"
```

**Replacements:**
- `%VERSION%` → Actual version string (e.g., "1.2.3")
- Used in `config.cpp` files for mod version display

### Meta.cpp Placeholders

The `src/meta.cpp` file uses these placeholders:

```cpp
version = "%VERSION%";         // Replaced with --version value
timeStamp = %TIMESTAMP%;       // Replaced with DateTime.UtcNow.Ticks
publishedid = %PUBLISHEDID%;   // Replaced with WORKSHOP_ID env variable
```

**Processing:**
- `build.bat` replaces all placeholders when copying to `dist/@Swarm/meta.cpp`
- Timestamp format is .NET ticks for Steam Workshop compatibility
- Published ID defaults to `0` if `WORKSHOP_ID` not set

## 🔐 Security Best Practices

### Private Key Protection

**Critical:** Never share or commit your `.biprivatekey` file!

```gitignore
# .gitignore already includes
keys/*.biprivatekey
```

If your private key is compromised:
1. Generate a new key pair: `publish.bat --version X.X.X --key NewKeyName`
2. Re-sign all PBOs with the new key
3. Distribute the new `.bikey` to server owners
4. Update your Workshop item with newly signed PBOs

### .env File Security

The `.env` file is gitignored but contains sensitive paths:

```gitignore
# .gitignore already includes
.env
```

**Never commit:**
- Steam usernames
- Workshop IDs (if you want to keep them private)
- Local file paths (they're specific to your machine)

## 🤝 Contributing

### Adding New Mods

1. Create new folder in `src/` (e.g., `src/SwarmNewMod/`)
2. Add `config.cpp` with comments explaining features
3. Add scripts in appropriate module folders (3_Game, 4_World, 5_Mission)
4. Build and test:
   ```batch
   build.bat --version 1.0.0 --validate
   launch.bat
   ```

### Modifying Animal Drops

Each animal has its own `config.cpp` in `src/SwarmAnimals/`:
- `canis_lupus/config.cpp` - Wolf
- `capra_hircus/config.cpp` - Goat (male)
- `capra_hircus_fem/config.cpp` - Goat (female)
- etc.

Edit the appropriate file and rebuild.

### Testing Changes

Always validate before publishing:

```batch
# Build with validation
build.bat --version X.X.X --validate

# Or use the full publish pipeline
publish.bat --version X.X.X --dry-run
```

## 📄 License

See individual mod directories for licensing information. SwarmEarplugs is a repack of the original EarPlugs mod by DaemonF0rge.

## 🙏 Credits

- **SwarmEarplugs** - Original [EarPlugs mod by DaemonF0rge](https://github.com/DaemonForge/DayZ-EarPlugs/)
- **SwarmSpectator** - Extends [Community Online Tools by Jacob-Mango](https://github.com/Jacob-Mango/DayZ-Community-Online-Tools)
- **Swarm Team** - Kize and contributors

## 📞 Support

For issues, questions, or suggestions:
- Check the logs in `.validate_temp/` or `.launch_temp/` for errors
- Review this README for workflow details
- Check each mod's `config.cpp` for feature documentation

---

**Last Updated:** February 2026  
**Current Version:** See `.version` file  
**Workshop:** Check your `WORKSHOP_ID` in `.env`

3. **Create Workshop Item Manually**
   - Open DayZ Tools → Publisher
   - Click "New" and fill in mod details
   - Upload initial version manually
   - Note the **Workshop ID** from the URL (e.g., `https://steamcommunity.com/sharedfiles/filedetails/?id=1234567890`)
   - Add `WORKSHOP_ID=1234567890` to your `.env` file

4. **Configure .env File**
   ```ini
   DAYZ_TOOLS=G:\SteamLibrary\steamapps\common\DayZ Tools
   DAYZ_SERVER=G:\SteamLibrary\steamapps\common\DayZServer
   STEAM_USERNAME=your_steam_username
   WORKSHOP_ID=1234567890
   ```

### Usage

```batch
# Full publish pipeline (build → validate → sign → publish)
publish.bat --version 1.2.3

# Publish with changelog
publish.bat --version 1.2.3 --changelog "Fixed bug with fishing rod shouldering"

# Publish with custom signing key
publish.bat --version 1.2.3 --key MyCustomKey

# Dry run (show what would happen without doing it)
publish.bat --version 1.2.3 --dry-run

# Skip validation (faster, but risky)
publish.bat --version 1.2.3 --skip-validate

# Build and sign only, don't publish
publish.bat --version 1.2.3 --skip-publish

# Use existing build, just sign and publish
publish.bat --version 1.2.3 --skip-build
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--version X.X.X` | **Required** - Version number to build and publish |
| `--key KeyName` | Key name for signing (default: "Swarm") |
| `--changelog "text"` | Change note displayed on Workshop (default: "Version X.X.X") |
| `--workshop-id ID` | Override Workshop ID from .env |
| `--skip-build` | Skip build step (use existing PBOs) |
| `--skip-validate` | Skip script validation (not recommended) |
| `--skip-sign` | Skip signing step |
| `--skip-publish` | Build and sign only, don't upload to Workshop |
| `--dry-run` | Show what would be done without executing |

### Signing Keys

#### First-Time Setup
On first run, `publish.bat` automatically creates a key pair:
- **Private Key:** `keys/Swarm.biprivatekey` - Keep this SECRET (gitignored)
- **Public Key:** `keys/Swarm.bikey` - Share with server owners (committed to repo)

#### Key Management
- **Never share your `.biprivatekey` file** - It's used to prove authenticity
- **Always share your `.bikey` file** - Server owners need this to verify your mods
- The public key is automatically copied to `dist/@Swarm/Keys/` for Workshop distribution
- Custom key names: Use `--key CustomName` to create/use different key pairs

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

#### Troubleshooting
- **Wrong password?** - Run the manual login command again to update cached credentials
- **Steam Guard timeout?** - Re-run the login command and enter code faster
- **Two-Factor Required?** - Use the Steam Mobile Authenticator app for instant codes

### Exit Codes

- `0` - Published successfully
- `Non-zero` - Error occurred (build failed, validation failed, signing failed, or upload failed)

### Output

After successful publish:
- **Workshop URL** - `https://steamcommunity.com/sharedfiles/filedetails/?id=YOUR_ID`
- **Signed PBOs** - `dist/@Swarm/Addons/*.pbo` and `*.bisign`
- **Public Key** - `dist/@Swarm/Keys/Swarm.bikey`
- **Mod Metadata** - `dist/@Swarm/mod.cpp` and `meta.cpp`

## 🚀 Launch Workflow (launch.bat)

Launches local DayZ Server and Client for development testing with automatic configuration.

### How It Works

1. **Environment Loading** - Loads paths from `.env` (DAYZ_SERVER, DAYZ_CLIENT, DAYZ_TOOLS, MODS, DAYZ_PROFILE)
2. **Process Cleanup** - Kills existing DayZ processes to free up files and ports
3. **Optional Build** - Runs `build.bat` and signs PBOs if `--build` flag specified
4. **Config Selection** - Uses `validate/serverDZ.cfg` or `validate/serverDZ.default.cfg`
5. **Temp Directory Setup** - Creates `.launch_temp/` for server profiles and client profiles
6. **BattleEye Configuration** - Automatically enables BattleEye in server config for stability
7. **Profile Copying** - Copies existing DayZ profiles from `DAYZ_PROFILE` to temp client directory (preserves settings)
8. **Mod List Building** - Loads dependencies from `validate/mods/` or `MODS` env variable, adds `@Swarm` last
9. **COT Permission Setup** - If Community Online Tools is detected, creates full admin permissions for "everyone" role
10. **Server Launch** - Starts DayZServer_x64.exe in new window with full logging
11. **Client Launch** - After 10-second wait, starts DayZ_BE.exe with auto-connect to local server

### Usage

```batch
# Launch server and client (default port 2302)
launch.bat

# Launch with custom port
launch.bat --port 2303

# Build, sign, and launch
launch.bat --build

# Build with custom port
launch.bat --build --port 2303

# Show help
launch.bat --help
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--port N` | Server port (default: 2302) |
| `--build` | Build and sign mod before launching |
| `--help` | Display help information |

### Server Configuration

Same as `validate.bat` - uses `validate/serverDZ.cfg` or `validate/serverDZ.default.cfg`.

### Automatic Features

#### BattleEye Auto-Enable
The script automatically adds `BattlEye = 1;` to the server config for proper client connection.

#### COT Admin Auto-Setup
If Community Online Tools is detected in the mod list, the script creates:
- `PermissionsFramework/Roles/everyone.txt` with full admin permissions
- All COT permissions granted to "everyone" role (no need to manually set up admins)
- Includes camera, ESP, teleport, weather, vehicle, loadout, and Namalsk event controls

#### Profile Preservation
If `DAYZ_PROFILE` is set, the script copies your existing:
- Client settings (`*.DayZProfile`)
- Graphics settings (`*.cfg`)
- Input settings (`*.xml`)

### Logs and Profiles

Everything is stored in `.launch_temp/`:
- **Server Logs** - `.launch_temp/*.RPT`, `.launch_temp/*.ADM`, `.launch_temp/*.log`
- **Client Logs** - `.launch_temp/client/Users/DevClient/*.RPT`
- **Server Profiles** - `.launch_temp/PermissionsFramework/`, etc.

### Auto-Connect

Client automatically connects to `127.0.0.1:PORT` using `-connect` and `-port` parameters.

### Stopping

- Close the server and client windows manually
- Or use Task Manager to kill `DayZServer_x64.exe` and `DayZ_x64.exe`
- Or run `launch.bat` again (it kills existing processes first)

## 🛠️ Development Tips

### Quick Development Cycle

```batch
# 1. Build with validation
build.bat --version 1.0.0 --validate

# 2. Launch for testing
launch.bat

# 3. Make changes to src/ files

# 4. Quick rebuild and relaunch
launch.bat --build
```

### Testing Without Validation

```batch
# Faster builds (skips validation)
build.bat --version 1.0.0

# Launch directly
launch.bat
```

### Version Management

The `.version` file stores the last used version:
```batch
# First build creates .version file
build.bat --version 1.0.0

# Subsequent builds can omit version
build.bat

# Update version explicitly
build.bat --version 1.1.0
```

### Debugging Script Errors

If validation fails:

1. **Check preserved logs** - `.validate_temp/` directory contains:
   - `*.RPT` - Full server logs
   - `crash_*.log` - Crash details
   - `script_*.log` - Script-specific logs

2. **Run with longer timeout** - Give server more time to load:
   ```batch
   validate.bat --timeout 60
   ```

3. **Test manually** - Launch server yourself:
   ```batch
   launch.bat
   # Check logs in .launch_temp/
   ```

### Testing Different Maps

Create `validate/serverDZ.cfg` with your custom map:

```cpp
hostname = "Dev Test Server";
Missions = {
    class Namalsk {
        template = "dayzOffline.namalsk";
    };
};
```

Then run validation or launch as normal.

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
# Usage from build.bat
powershell -NoProfile -ExecutionPolicy Bypass -File "scripts\update_version.ps1" -SourceDir "src\SwarmTweaks" -TempDir ".build_temp\SwarmTweaks" -Version "1.2.3"
```

**Replacements:**
- `%VERSION%` → Actual version string (e.g., "1.2.3")
- Used in `config.cpp` files for mod version display

### Meta.cpp Placeholders

The `src/meta.cpp` file uses these placeholders:

```cpp
version = "%VERSION%";         // Replaced with --version value
timeStamp = %TIMESTAMP%;       // Replaced with DateTime.UtcNow.Ticks
publishedid = %PUBLISHEDID%;   // Replaced with WORKSHOP_ID env variable
```

**Processing:**
- `build.bat` replaces all placeholders when copying to `dist/@Swarm/meta.cpp`
- Timestamp format is .NET ticks for Steam Workshop compatibility
- Published ID defaults to `0` if `WORKSHOP_ID` not set

## 🔐 Security Best Practices

### Private Key Protection

**Critical:** Never share or commit your `.biprivatekey` file!

```gitignore
# .gitignore already includes
keys/*.biprivatekey
```

If your private key is compromised:
1. Generate a new key pair: `publish.bat --version X.X.X --key NewKeyName`
2. Re-sign all PBOs with the new key
3. Distribute the new `.bikey` to server owners
4. Update your Workshop item with newly signed PBOs

### .env File Security

The `.env` file is gitignored but contains sensitive paths:

```gitignore
# .gitignore already includes
.env
```

**Never commit:**
- Steam usernames
- Workshop IDs (if you want to keep them private)
- Local file paths (they're specific to your machine)

## 🤝 Contributing

### Adding New Mods

1. Create new folder in `src/` (e.g., `src/SwarmNewMod/`)
2. Add `config.cpp` with comments explaining features
3. Add scripts in appropriate module folders (3_Game, 4_World, 5_Mission)
4. Build and test:
   ```batch
   build.bat --version 1.0.0 --validate
   launch.bat
   ```

### Modifying Animal Drops

Each animal has its own `config.cpp` in `src/SwarmAnimals/`:
- `canis_lupus/config.cpp` - Wolf
- `capra_hircus/config.cpp` - Goat (male)
- `capra_hircus_fem/config.cpp` - Goat (female)
- etc.

Edit the appropriate file and rebuild.

### Testing Changes

Always validate before publishing:

```batch
# Build with validation
build.bat --version X.X.X --validate

# Or use the full publish pipeline
publish.bat --version X.X.X --dry-run
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

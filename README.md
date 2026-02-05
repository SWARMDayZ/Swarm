# Swarm - DayZ Mod Collection

A collection of DayZ Standalone mods including gameplay tweaks, animal configurations, and quality-of-life features.

## Recent Changes

### New Features
- **SwarmSpectator** - New mod for enhanced spectator camera controls with Dolly Cam support
- **launch.bat** - Automated server + client launcher for local testing with BattleEye support
- **DAYZ_CLIENT** environment variable - Configure your DayZ client installation path
- **MODS** environment variable - Define mod load order for testing

### Script Organization
- Consolidated build scripts: `build.bat` now builds all packages automatically
- Renamed `validate_scripts.bat` → `validate.bat` for consistency
- Renamed `validation/` → `validate/` directory
- All temporary directories now use consistent naming (`.build_temp`, `.validate_temp`, `.launch_temp`)

### BattleEye Improvements
- Fixed server crashes by keeping BattleEye **enabled** (not disabled)
- Client now uses `DayZ_BE.exe` for proper BattleEye initialization
- Removed filePatching flags (not needed for mod testing)
- Server config automatically sets `BattlEye = 1` for stability

### CI/CD Cleanup
- Removed GitHub Actions workflows (`.github/workflows/`)
- Simplified to local development focus with batch scripts

## Included Mods

### SwarmTweaks
Gameplay modifications and tweaks:
- **LessLaugh** - Reduced laughing frequency
- **LessCough** - Reduced coughing frequency
- **CraftImprovisedSuppressor** - Craftable improvised suppressors
- **NoBuryItems** - Prevents burying items
- **ItemsInBack** - Items visible on character's back

### SwarmAnimals
Animal skinning and harvesting configurations for various animals:
- Wolf (Canis Lupus)
- Goat (Capra Hircus)
- Sheep (Ovis Aries)
- Reindeer (Rangifer Tarandus)
- Wild Boar (Sus Scrofa)
- Brown Bear (Ursus Arctos)
- Polar Bear (Ursus Maritimus)

### SwarmEarplugs
Volume control system with in-game earplugs widget:
- Adjustable volume levels (Full, Mid, Low, Off)
- Custom keybind support
- Visual indicator widget

### SwarmSpectator
Enhanced spectator/camera system for DayZ:
- **Dolly Cam** - Smooth camera movement following players/entities
- **Lock On Target** - Keep camera focused on specific entity
- **Show Target Marker** - Visual indicator for locked target
- Integrates with JM Camera Tools mod

## Project Structure

```
Swarm/
├── src/
│   ├── SwarmTweaks/        # Gameplay tweaks mod
│   ├── SwarmAnimals/       # Animal configurations
│   ├── SwarmEarplugs/      # Earplugs/volume control
│   └── SwarmSpectator/     # Spectator camera enhancements
├── dist/                   # Build output (gitignored)
│   └── @Swarm/
│       ├── Addons/         # Compiled PBO files
│       ├── Keys/           # Public signing key
│       └── meta.cpp
├── keys/                   # Signing keys (private key gitignored)
├── scripts/                # Build helper scripts + steamcmd.exe
├── validate/             # Script validate configuration
│   ├── mods/               # Dependency mods for validate
│   ├── serverDZ.cfg        # Custom server config (gitignored)
│   └── serverDZ.default.cfg # Default validate config
├── .github/workflows/      # CI/CD pipelines
└── *.bat                   # Build scripts
```

## Prerequisites

### Required
- **DayZ Tools** - Install via Steam (free with DayZ ownership)
- **Windows** - Build scripts are Windows batch files

### Optional (for script validate)
- **DayZ Server** - Required for `validate.bat` and publish validate

### Environment Setup

Create a `.env` file in the project root with your paths:

```ini
# Required - Path to DayZ Tools installation
DAYZ_TOOLS=G:\SteamLibrary\steamapps\common\DayZ Tools

# Optional - Path to DayZ Server (for script validate)
DAYZ_SERVER=G:\SteamLibrary\steamapps\common\DayZServer

# Required for publishing - Steam credentials
STEAM_USERNAME=your_steam_username
WORKSHOP_ID=1234567890
```

Or set environment variables manually:

```batch
setx DAYZ_TOOLS "G:\SteamLibrary\steamapps\common\DayZ Tools"
setx DAYZ_SERVER "G:\SteamLibrary\steamapps\common\DayZServer"
```

## Building

### Build All Packages

```batch
build.bat --version 1.0.0
```

This will:
1. Update version numbers in all `meta.cpp` and `config.cpp` files
2. Build PBO files using DayZ Tools AddonBuilder
3. Output to `dist/@Swarm/Addons/`

### Build Output

After building, you'll find:
```
dist/@Swarm/
├── Addons/
│   ├── SwarmTweaks.pbo
│   ├── SwarmAnimals.pbo
│   ├── SwarmEarplugs.pbo
│   └── SwarmSpectator.pbo
└── meta.cpp
```

## Development & Testing

### Launch Server and Client

The `launch.bat` script provides an easy way to test your mod locally:

```batch
launch.bat
```

**Key Features:**
- Automatically starts both DayZ Server and Client
- Enables BattleEye for stability (uses `DayZ_BE.exe` for client)
- Auto-connects client to local server
- Supports dependency mods from `validate/mods/` or `MODS` env variable
- Optional mod building and signing with `--build` flag

**Options:**
```batch
launch.bat --build           # Build mod before launching
launch.bat --port 2303       # Use custom port
launch.bat --build --port 2303  # Combined options
```

**Environment Variables Required:**
```ini
DAYZ_SERVER=C:\Path\To\DayZServer
DAYZ_CLIENT=C:\Path\To\DayZ
DAYZ_TOOLS=C:\Path\To\DayZ Tools  # Only for --build flag
MODS=@CF;@CommunityFramework      # Optional, load order
```

**What it does:**
1. Verifies BattleEye folders exist and are properly set up
2. Optionally builds and signs the mod (`--build`)
3. Starts DayZ Server with your mod loaded
4. Waits for server initialization
5. Launches DayZ Client using `DayZ_BE.exe` for proper BattleEye support
6. Auto-connects client to 127.0.0.1 on specified port

**Note:** The script keeps BattleEye **enabled** for both server and client to prevent crashes. File patching is not used.

## Script Validate

The validate system allows you to verify your scripts compile correctly before publishing by running a DayZ Server instance.

### Running Validate

```batch
validate.bat
```

This will:
1. Start a minimal DayZ Server instance
2. Load your mod and any dependency mods
3. Check for script compilation errors
4. Report any errors found in the logs

### Validate Options

```batch
validate.bat --timeout 90      # Custom timeout (default: 60s)
validate.bat --skip-build      # Skip build check
validate.bat --help            # Show help
```

### Server Configuration

The validate system uses server configs in `validate/`:

| File | Purpose |
|------|---------|
| `serverDZ.cfg` | Custom config (gitignored) - use for your specific map |
| `serverDZ.default.cfg` | Default config - uses Chernarus offline |

To use a different map (e.g., Namalsk):
1. Copy `serverDZ.default.cfg` to `serverDZ.cfg`
2. Modify the `Missions` class template

Example `serverDZ.cfg` for Namalsk:
```cpp
// ... other settings ...
class Missions {
    class Swarm_Validate {
        template="hardcore.namalsk";
    };
};
```

### Dependency Mods

If your mod depends on other mods (CF, Community Framework, etc.), place them in `validate/mods/`:

```
validate/mods/
├── @CF/
│   └── Addons/
│       └── CF.pbo
├── @Community-Online-Tools/
│   └── Addons/
│       └── ...
└── README.md
```

**Tips:**
- Use **symlinks** to avoid duplicating large mod folders:
  ```batch
  mklink /D "@CF" "C:\Path\To\Steam\workshop\content\221100\@CF"
  ```
- **Load order**: Mods are loaded alphabetically. Use numeric prefixes if needed (e.g., `@1_CF`, `@2_COT`)
- The `validate/mods/` folder is gitignored (except README.md)

## Publishing to Steam Workshop

The `publish.bat` script provides a complete workflow: build, validate, sign, and publish to Steam Workshop using SteamCMD.

### SteamCMD Setup

SteamCMD is included in the `scripts/` folder. Before your first publish, cache your Steam credentials:

```batch
scripts\steamcmd.exe +login your_steam_username +quit
```

Enter your Steam Guard code when prompted - credentials will be cached for future runs.

### Full Publish Workflow

```batch
publish.bat --version 1.0.0
```

This will:
1. **Build** all packages with the specified version
2. **Validate** scripts using DayZ Server (if `DAYZ_SERVER` is set)
3. **Sign** all PBO files with `.bikey/.biprivatekey`
4. **Publish** to Steam Workshop using SteamCMD

### Publish Options

| Option | Description |
|--------|-------------|
| `--version X.X.X` | Version number (required) |
| `--key KeyName` | Custom key name for signing (default: Swarm) |
| `--changelog "text"` | Change note for Steam Workshop |
| `--workshop-id ID` | Steam Workshop ID (or set in `.env`) |
| `--skip-build` | Skip build step (use existing PBOs) |
| `--skip-validate` | Skip script validate step |
| `--skip-sign` | Skip PBO signing step |
| `--skip-publish` | Build and sign only, don't upload |
| `--dry-run` | Preview what would be done without executing |

### Examples

```batch
# Full publish with changelog
publish.bat --version 1.0.0 --changelog "Bug fixes and new features"

# Build and sign only (no upload)
publish.bat --version 1.0.0 --skip-publish

# Skip validate (not recommended)
publish.bat --version 1.0.0 --skip-validate

# Preview what would happen
publish.bat --version 1.0.0 --dry-run

# Use existing build, just sign and publish
publish.bat --version 1.0.0 --skip-build
```

### First-Time Workshop Setup

For the first publish, you must create the Workshop item manually:

1. Go to [Steam Workshop](https://steamcommunity.com/app/221100/workshop/) for DayZ
2. Click **Create** to make a new Workshop item
3. Fill in title, description, visibility, and upload your mod folder once
4. Copy the **Workshop ID** from the URL (e.g., `https://steamcommunity.com/sharedfiles/filedetails/?id=1234567890`)
5. Add to your `.env` file:
   ```ini
   WORKSHOP_ID=1234567890
   ```
6. Subsequent updates can use `publish.bat` automatically

### After Publishing

Your mod folder will contain:
```
dist/@Swarm/
├── Addons/
│   ├── SwarmTweaks.pbo
│   ├── SwarmTweaks.pbo.Swarm.bisign
│   ├── SwarmAnimals.pbo
│   ├── SwarmAnimals.pbo.Swarm.bisign
│   ├── SwarmEarplugs.pbo
│   └── SwarmEarplugs.pbo.Swarm.bisign
├── Keys/
│   └── Swarm.bikey
├── meta.cpp
└── mod.cpp
```

### Troubleshooting Publishing

| Issue | Solution |
|-------|----------|
| First time publish fails | Create Workshop item manually on Steam first, then add WORKSHOP_ID to `.env` |
| Steam Guard prompt | Run `scripts\steamcmd.exe +login your_username +quit` to cache credentials |
| Authentication error | Re-run SteamCMD manually to update cached credentials |
| SteamCMD not found | Verify `scripts/steamcmd.exe` exists |
| Wrong password | SteamCMD uses cached credentials - run manual login to update |

## Server Installation

### For Server Owners

1. Subscribe to the mod on Steam Workshop, or download the release
2. Copy `@Swarm` folder to your server's mod directory
3. Add `@Swarm` to your server's `-mod=` parameter
4. Copy `Swarm.bikey` from `@Swarm/Keys/` to your server's `keys/` folder

### Example Server Launch

```
DayZServer_x64.exe -config=serverDZ.cfg -mod=@Swarm
```

## Development

### Adding a New Feature

1. Create/modify files in the appropriate `src/` subfolder
2. Update `config.cpp` if adding new classes
3. Run `validate.bat` to verify scripts compile
4. Test locally with DayZ
5. Submit a pull request

### File Types

| Extension | Purpose |
|-----------|---------|
| `.cpp` | Configuration files (CfgPatches, CfgMods, etc.) |
| `.c` | EnforceScript code |
| `.layout` | UI layout definitions |
| `.xml` | Input bindings and data |
| `.edds` | Compressed textures |

## CI/CD

This project uses GitHub Actions for continuous integration:

| Workflow | Trigger | Purpose |
|----------|---------|---------|
| `build.yml` | Push/PR to main | Validate source files |
| `pr-check.yml` | Pull requests | Code quality checks |
| `release.yml` | Version tags | Create GitHub releases |
| `publish.yml` | Manual | Build, sign, and publish to Workshop |

### Creating a Release

```bash
git tag v1.0.0
git push origin v1.0.0
```

See [.github/CICD.md](.github/CICD.md) for detailed CI/CD documentation.

## Security Notes

- **Never commit** `.biprivatekey` files (already in `.gitignore`)
- **Never share** your private signing key
- The `.bikey` (public key) is safe to distribute
- Server owners need the `.bikey` to verify your mod signatures
- Keep your `.env` file private (it's gitignored)

## Credits

- **Author**: Kize & Arky
- **DayZ Tools**: Bohemia Interactive

# Swarm - DayZ Mod Collection

A collection of DayZ Standalone mods including gameplay tweaks, animal configurations, and quality-of-life features.

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

## Project Structure

```
Swarm/
├── src/
│   ├── SwarmTweaks/        # Gameplay tweaks mod
│   ├── SwarmAnimals/       # Animal configurations
│   └── SwarmEarplugs/      # Earplugs/volume control
├── dist/                   # Build output (gitignored)
│   └── @Swarm/
│       ├── Addons/         # Compiled PBO files
│       ├── Keys/           # Public signing key
│       └── meta.cpp
├── keys/                   # Signing keys (private key gitignored)
├── scripts/                # Build helper scripts + steamcmd.exe
├── validation/             # Script validation configuration
│   ├── mods/               # Dependency mods for validation
│   ├── serverDZ.cfg        # Custom server config (gitignored)
│   └── serverDZ.default.cfg # Default validation config
├── .github/workflows/      # CI/CD pipelines
└── *.bat                   # Build scripts
```

## Prerequisites

### Required
- **DayZ Tools** - Install via Steam (free with DayZ ownership)
- **Windows** - Build scripts are Windows batch files

### Optional (for script validation)
- **DayZ Server** - Required for `validate_scripts.bat` and publish validation

### Environment Setup

Create a `.env` file in the project root with your paths:

```ini
# Required - Path to DayZ Tools installation
DAYZ_TOOLS=G:\SteamLibrary\steamapps\common\DayZ Tools

# Optional - Path to DayZ Server (for script validation)
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
build_all.bat --version 1.0.0
```

This will:
1. Update version numbers in all `meta.cpp` and `config.cpp` files
2. Build PBO files using DayZ Tools AddonBuilder
3. Output to `dist/@Swarm/Addons/`

### Build Individual Packages

```batch
build_swarmtweaks.bat --version 1.0.0
build_swarmanimals.bat --version 1.0.0
build_swarmearplugs.bat --version 1.0.0
```

### Build Output

After building, you'll find:
```
dist/@Swarm/
├── Addons/
│   ├── SwarmTweaks.pbo
│   ├── SwarmAnimals.pbo
│   └── SwarmEarplugs.pbo
└── meta.cpp
```

## Script Validation

The validation system allows you to verify your scripts compile correctly before publishing by running a DayZ Server instance.

### Running Validation

```batch
validate_scripts.bat
```

This will:
1. Start a minimal DayZ Server instance
2. Load your mod and any dependency mods
3. Check for script compilation errors
4. Report any errors found in the logs

### Validation Options

```batch
validate_scripts.bat --timeout 90      # Custom timeout (default: 60s)
validate_scripts.bat --skip-build      # Skip build check
validate_scripts.bat --help            # Show help
```

### Server Configuration

The validation system uses server configs in `validation/`:

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
    class Swarm_Validation {
        template="hardcore.namalsk";
    };
};
```

### Dependency Mods

If your mod depends on other mods (CF, Community Framework, etc.), place them in `validation/mods/`:

```
validation/mods/
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
- The `validation/mods/` folder is gitignored (except README.md)

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
| `--skip-validate` | Skip script validation step |
| `--skip-sign` | Skip PBO signing step |
| `--skip-publish` | Build and sign only, don't upload |
| `--dry-run` | Preview what would be done without executing |

### Examples

```batch
# Full publish with changelog
publish.bat --version 1.0.0 --changelog "Bug fixes and new features"

# Build and sign only (no upload)
publish.bat --version 1.0.0 --skip-publish

# Skip validation (not recommended)
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
3. Run `validate_scripts.bat` to verify scripts compile
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

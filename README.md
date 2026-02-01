# Swarm - DayZ Mod Collection

A collection of DayZ Standalone mods including gameplay tweaks, animal configurations, and quality-of-life features.

## Included Mods

### SwarmTweaks
Gameplay modifications and tweaks:
- **LessLaugh** - Reduced laughing frequency
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
├── scripts/                # Build helper scripts
├── .github/workflows/      # CI/CD pipelines
└── *.bat                   # Build scripts
```

## Prerequisites

### Required
- **DayZ Tools** - Install via Steam (free with DayZ ownership)
- **Windows** - Build scripts are Windows batch files

### Environment Setup

Set the `DAYZ_TOOLS` environment variable to your DayZ Tools installation:

```batch
setx DAYZ_TOOLS "G:\SteamLibrary\steamapps\common\DayZ Tools"
```

Or create a `.env` file in the project root:
```
DAYZ_TOOLS=G:\SteamLibrary\steamapps\common\DayZ Tools
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

## Publishing to Steam Workshop

### Full Publish (Build + Sign + Upload)

```batch
publish.bat --version 1.0.0
```

This will:
1. Build all packages with the specified version
2. Create signing keys (first run only)
3. Sign all PBO files with `.bisign` signatures
4. Copy public key to `@Swarm/Keys/`
5. Upload to Steam Workshop (if configured)

### Using a Custom Signing Key

```batch
publish.bat --version 1.0.0 --key MyKeyName
```

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
└── meta.cpp
```

### First-Time Workshop Setup

1. Open **DayZ Tools** → **Publisher**
2. Create a new Workshop item
3. Select `dist/@Swarm` as the mod folder
4. Fill in title, description, and upload
5. Subsequent updates can use `publish.bat`

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
3. Test locally with DayZ
4. Submit a pull request

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

## Credits

- **Author**: Kize & Arky
- **DayZ Tools**: Bohemia Interactive

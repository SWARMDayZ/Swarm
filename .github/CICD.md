# CI/CD Pipeline Documentation

This document describes the CI/CD pipeline for the Swarm DayZ mod collection.

## Overview

The pipeline consists of four workflows:

| Workflow | Trigger | Purpose |
|----------|---------|---------|
| `build.yml` | Push to main, PRs | Validate source files and optionally build PBOs |
| `pr-check.yml` | Pull requests | Comprehensive PR validation |
| `release.yml` | Tags (v*), Manual | Create GitHub releases |
| `publish.yml` | Manual only | Build, sign, and publish to Steam Workshop |

## Workflows

### Build (`build.yml`)

Runs on every push to `main` and on pull requests.

**What it does:**
- Validates config.cpp and meta.cpp files exist
- Checks EnforceScript (.c) syntax (brace/parenthesis matching)
- Validates CfgPatches references
- Optionally builds PBOs (requires self-hosted runner)

**Artifacts:**
- `swarm-source` - Source files (7 days retention)
- `swarm-pbos` - Built PBO files (7 days retention, if available)

### PR Check (`pr-check.yml`)

Runs on all pull requests to `main`.

**What it does:**
- Checks version consistency across all packages
- Validates required file structure
- Detects debug code (Print, Chat, DEBUG comments)
- Validates EnforceScript syntax
- Reports warnings for TODO/FIXME/HACK comments

### Release (`release.yml`)

Triggered by version tags (`v*`) or manually.

**Trigger options:**
- Push a tag: `git tag v1.0.0 && git push --tags`
- Manual: Go to Actions > Release > Run workflow

**Inputs (manual):**
- `version` - Version number (e.g., 1.0.0)
- `publish_workshop` - Also publish to Steam Workshop (requires self-hosted runner)

**What it does:**
1. Updates version in all meta.cpp and config.cpp files
2. Builds PBOs (if DayZ Tools available) or creates source package
3. Creates GitHub release with downloadable archives
4. Optionally triggers Workshop publishing

**Artifacts:**
- GitHub Release with zip archives
- Individual package archives

### Publish (`publish.yml`)

Manual workflow for Steam Workshop publishing.

**Requirements:**
- Self-hosted Windows runner with DayZ Tools installed
- Steam authentication configured
- Workshop item already created

**Inputs:**
- `version` - Version to publish
- `change_note` - Steam Workshop change note
- `visibility` - public/friends_only/private

## Self-Hosted Runner Setup

For PBO building and Steam Workshop publishing, you need a self-hosted runner.

### Prerequisites

1. **Windows machine** (Windows 10/11 or Windows Server)
2. **DayZ Tools** installed via Steam
3. **Steam client** logged in with publishing rights

### Installation Steps

1. **Install GitHub Actions Runner**
   ```powershell
   # Download runner from: Settings > Actions > Runners > New self-hosted runner
   # Follow the configuration instructions
   ```

2. **Configure DayZ Tools environment**
   ```powershell
   # Set system environment variable
   [System.Environment]::SetEnvironmentVariable("DAYZ_TOOLS", "C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools", "Machine")
   ```

3. **Add runner labels**
   
   When configuring the runner, add these labels:
   - `self-hosted`
   - `windows`
   - `dayz-tools`

4. **Steam Workshop Setup**
   
   Before automated publishing works:
   1. Open DayZ Tools > Publisher
   2. Create your mod item manually
   3. Note the Workshop ID from the URL
   4. Authenticate with Steam (this caches credentials)

### Runner Configuration

The runner service should run as a user with:
- Access to DayZ Tools
- Steam authentication cached
- Write access to the workspace

### Signing Key Setup

For automated signing, place your key pair in one of these locations on the runner:

```
%USERPROFILE%\DayZKeys\Swarm.biprivatekey
%USERPROFILE%\DayZKeys\Swarm.bikey
```

Or:

```
%DAYZ_TOOLS%\Keys\Swarm.biprivatekey
%DAYZ_TOOLS%\Keys\Swarm.bikey
```

**IMPORTANT**: Never commit private keys to the repository!

## Repository Configuration

### Secrets

Configure these in Settings > Secrets and Variables > Actions > Secrets:

| Secret | Required For | Description |
|--------|--------------|-------------|
| `STEAM_USERNAME` | Workshop publish | Steam account username |
| `STEAM_PASSWORD` | Workshop publish | Steam account password |
| `STEAM_GUARD_CODE` | Workshop publish | Current Steam Guard code (update before publish) |

**Note:** `GITHUB_TOKEN` is automatically provided.

### Variables

Configure these in Settings > Secrets and Variables > Actions > Variables:

| Variable | Required For | Description |
|----------|--------------|-------------|
| `WORKSHOP_ID` | Workshop publish | Your Steam Workshop item ID |

### Getting Workshop ID

1. Go to your mod on Steam Workshop
2. URL format: `https://steamcommunity.com/sharedfiles/filedetails/?id=XXXXXXXXXX`
3. The `XXXXXXXXXX` is your Workshop ID

## Usage Examples

### Create a Release

```bash
# Tag-triggered release
git tag v1.2.0
git push origin v1.2.0

# Or manually via GitHub Actions UI
```

### Publish to Workshop

1. Go to Actions > "Publish to Steam Workshop"
2. Click "Run workflow"
3. Enter version (e.g., 1.2.0)
4. Enter change note
5. Click "Run workflow"

**Important:** Update `STEAM_GUARD_CODE` secret with current code before publishing!

### Local Build

```batch
REM Build all packages (no signing)
build.bat --version 1.2.0

REM Build, sign, and publish (local machine with DayZ Tools)
publish.bat --version 1.2.0

REM Build with custom key name
publish.bat --version 1.2.0 --key MyCustomKey
```

## Mod Signing

DayZ server mods must be signed with a `.bikey`/`.biprivatekey` pair for servers to verify authenticity.

### How Signing Works

1. **Key Generation**: First run creates a key pair in the `keys/` folder
2. **PBO Signing**: Each `.pbo` file is signed, creating a `.bisign` file
3. **Key Distribution**: The `.bikey` (public key) is copied to `@Swarm/Keys/`

### Key Files

| File | Purpose | Share? |
|------|---------|--------|
| `keys/Swarm.biprivatekey` | Signs PBO files | **NEVER** - Keep secret! |
| `keys/Swarm.bikey` | Verifies signatures | Yes - Server owners need this |
| `@Swarm/Keys/Swarm.bikey` | Included in mod | Distributed with mod |

### Security

- **NEVER** commit `.biprivatekey` files (already in `.gitignore`)
- **NEVER** share your private key
- If compromised, generate a new key pair and re-sign
- Server owners add your `.bikey` to their `keys/` folder to allow your mod

## Troubleshooting

### PBO Build Fails

1. Ensure DayZ Tools is installed correctly
2. Verify `DAYZ_TOOLS` environment variable
3. Check AddonBuilder logs in the workflow output

### Workshop Publish Fails

1. **Steam Guard:** Update `STEAM_GUARD_CODE` secret
2. **First publish:** Create mod manually in DayZ Tools Publisher first
3. **Auth expired:** Re-authenticate via Steam client on runner machine
4. **Permission denied:** Ensure Steam account has Workshop publishing rights

### Self-Hosted Runner Issues

1. Check runner is online: Settings > Actions > Runners
2. Verify labels match workflow requirements
3. Check runner logs: `_diag/` folder in runner directory

## File Structure

```
.github/
├── workflows/
│   ├── build.yml        # Build validation
│   ├── pr-check.yml     # PR validation
│   ├── release.yml      # GitHub releases
│   └── publish.yml      # Steam Workshop
└── CICD.md              # This file

src/
├── SwarmTweaks/
├── SwarmAnimals/
└── SwarmEarplugs/

dist/                    # Build output (gitignored)
└── @Swarm/
    ├── Addons/
    │   ├── SwarmTweaks.pbo
    │   ├── SwarmAnimals.pbo
    │   └── SwarmEarplugs.pbo
    └── meta.cpp
```

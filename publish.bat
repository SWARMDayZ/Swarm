@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo Swarm Mod Publisher
echo ========================================
echo.

REM Load environment variables from .env file using PowerShell
set "_envfile=%~dp0.env"
set "_psscript=%~dp0scripts\load_env.ps1"
set "_tmpenv=%TEMP%\_swarm_env.tmp"
if exist "!_envfile!" if exist "!_psscript!" (
    for %%V in (DAYZ_TOOLS DAYZ_SERVER STEAM_USERNAME WORKSHOP_ID) do (
        powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName %%V > "!_tmpenv!" 2>nul
        if exist "!_tmpenv!" (
            set /p %%V=<"!_tmpenv!"
            del "!_tmpenv!" >nul 2>&1
        )
    )
)

set "VERSION="
set "MOD_DIR=%~dp0dist\@Swarm"
set "KEYS_DIR=%~dp0keys"
set "KEY_NAME=Swarm"
set "CHANGELOG="
set "SKIP_BUILD=0"
set "SKIP_VALIDATE=0"
set "SKIP_SIGN=0"
set "SKIP_PUBLISH=0"
set "DRY_RUN=0"

REM Parse arguments
:parse_args
if "%~1"=="" goto :check_args
if /i "%~1"=="--version" (
    set "VERSION=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--key" (
    set "KEY_NAME=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--changelog" (
    set "CHANGELOG=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--workshop-id" (
    set "WORKSHOP_ID=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--skip-build" (
    set "SKIP_BUILD=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--skip-validate" (
    set "SKIP_VALIDATE=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--skip-sign" (
    set "SKIP_SIGN=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--skip-publish" (
    set "SKIP_PUBLISH=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--dry-run" (
    set "DRY_RUN=1"
    shift
    goto :parse_args
)
shift
goto :parse_args

:check_args
REM Check if version is provided
if "!VERSION!"=="" (
    echo ERROR: Version is required!
    echo.
    echo Usage: publish.bat --version X.X.X [options]
    echo.
    echo Options:
    echo   --version X.X.X       Version number to build and publish ^(required^)
    echo   --key KeyName         Key name for signing ^(default: Swarm^)
    echo   --changelog "text"    Change note for Steam Workshop
    echo   --workshop-id ID      Steam Workshop ID ^(or set WORKSHOP_ID in .env^)
    echo   --skip-build          Skip the build step ^(use existing PBOs^)
    echo   --skip-validate       Skip script validation step
    echo   --skip-sign           Skip the signing step
    echo   --skip-publish        Build and sign only, don't publish to Workshop
    echo   --dry-run             Show what would be done without executing
    echo.
    echo Environment Variables ^(set in .env file^):
    echo   DAYZ_TOOLS            Path to DayZ Tools installation
    echo   DAYZ_SERVER           Path to DayZ Server ^(for validation^)
    echo   STEAM_USERNAME        Steam account username
    echo   WORKSHOP_ID           Steam Workshop item ID
    echo.
    echo Note: steamcmd.exe must be in the scripts folder
    echo.
    echo This will:
    echo   1. Build all packages with the specified version
    echo   2. Validate scripts using DayZ Server
    echo   3. Sign all PBO files with .bikey/.biprivatekey
    echo   4. Upload to Steam Workshop using SteamCMD
    echo.
    echo Examples:
    echo   publish.bat --version 1.0.0
    echo   publish.bat --version 1.0.0 --changelog "Bug fixes and improvements"
    echo   publish.bat --version 1.0.0 --skip-validate
    echo   publish.bat --version 1.0.0 --dry-run
    echo.
    goto :eof
)

REM Check for DayZ Tools
if not defined DAYZ_TOOLS (
    echo ERROR: DAYZ_TOOLS environment variable not set!
    echo Please set it to your DayZ Tools installation path.
    echo.
    echo You can add this to your .env file:
    echo   DAYZ_TOOLS=C:\Program Files ^(x86^)\Steam\steamapps\common\DayZ Tools
    goto :eof
)

if "!DRY_RUN!"=="1" (
    echo [DRY RUN] Would use DayZ Tools: !DAYZ_TOOLS!
) else (
    echo Using DayZ Tools: !DAYZ_TOOLS!
)
echo.

REM Set tool paths
set "DS_SIGN_FILE=!DAYZ_TOOLS!\Bin\DsUtils\DSSignFile.exe"
set "DS_CREATE_KEY=!DAYZ_TOOLS!\Bin\DsUtils\DSCreateKey.exe"

REM Verify signing tools exist (only if we need them)
if "!SKIP_SIGN!"=="0" (
    if not exist "!DS_SIGN_FILE!" (
        echo ERROR: DSSignFile.exe not found at: !DS_SIGN_FILE!
        echo Please verify DayZ Tools is installed correctly.
        goto :eof
    )

    if not exist "!DS_CREATE_KEY!" (
        echo ERROR: DSCreateKey.exe not found at: !DS_CREATE_KEY!
        echo Please verify DayZ Tools is installed correctly.
        goto :eof
    )
    echo Found signing tools.
)
echo.

echo ========================================
echo Step 1: Building mod with version !VERSION!
echo ========================================
echo.

if "!SKIP_BUILD!"=="1" (
    echo Skipping build step ^(--skip-build specified^)
    echo Using existing PBOs in: !MOD_DIR!\Addons
    echo.
) else (
    if "!DRY_RUN!"=="1" (
        echo [DRY RUN] Would execute: build_all.bat --version !VERSION!
        echo.
    ) else (
        call "%~dp0build_all.bat" --version !VERSION!

        if !ERRORLEVEL! NEQ 0 (
            echo ERROR: Build failed!
            goto :eof
        )
    )
)
echo.

REM Check if mod directory exists
if not exist "!MOD_DIR!" (
    echo ERROR: Mod directory not found: !MOD_DIR!
    goto :eof
)

REM Check if PBO files exist
set "PBO_COUNT=0"
for %%F in ("!MOD_DIR!\Addons\*.pbo") do set /a PBO_COUNT+=1

if !PBO_COUNT! EQU 0 (
    echo ERROR: No PBO files found in !MOD_DIR!\Addons
    echo Make sure DayZ Tools AddonBuilder is configured correctly.
    goto :eof
)

echo Found !PBO_COUNT! PBO files.
echo.

echo ========================================
echo Step 2: Validating scripts
echo ========================================
echo.

if "!SKIP_VALIDATE!"=="1" (
    echo Skipping validation step ^(--skip-validate specified^)
    echo.
    goto :step3
)

REM Check if DAYZ_SERVER is set (required for validation)
if not defined DAYZ_SERVER (
    echo WARNING: DAYZ_SERVER environment variable not set!
    echo Skipping script validation.
    echo.
    echo To enable validation, add to your .env file:
    echo   DAYZ_SERVER=C:\Path\To\DayZServer
    echo.
    goto :step3
)

if "!DRY_RUN!"=="1" (
    echo [DRY RUN] Would execute: validate_scripts.bat --skip-build
    echo.
    goto :step3
)

echo Running script validation using DayZ Server...
echo.

call "%~dp0validate_scripts.bat" --skip-build

if !ERRORLEVEL! NEQ 0 (
    echo.
    echo ERROR: Script validation failed!
    echo.
    echo Fix the script errors above before publishing.
    echo You can skip validation with --skip-validate ^(not recommended^)
    goto :eof
)

echo.

:step3
echo ========================================
echo Step 3: Signing PBO files
echo ========================================
echo.

if "!SKIP_SIGN!"=="1" (
    echo Skipping signing step ^(--skip-sign specified^)
    echo.
    goto :step4
)

REM Create keys directory if it doesn't exist
if not exist "!KEYS_DIR!" (
    if "!DRY_RUN!"=="1" (
        echo [DRY RUN] Would create keys directory: !KEYS_DIR!
    ) else (
        echo Creating keys directory: !KEYS_DIR!
        mkdir "!KEYS_DIR!"
    )
)

REM Check if keys exist, create if not
set "PRIVATE_KEY=!KEYS_DIR!\!KEY_NAME!.biprivatekey"
set "PUBLIC_KEY=!KEYS_DIR!\!KEY_NAME!.bikey"

echo Private key path: !PRIVATE_KEY!
echo Public key path: !PUBLIC_KEY!
echo.

if not exist "!PRIVATE_KEY!" (
    if "!DRY_RUN!"=="1" (
        echo [DRY RUN] Would create new key pair: !KEY_NAME!
        echo.
    ) else (
        echo Creating new key pair: !KEY_NAME!
        echo.
        
        pushd "!KEYS_DIR!"
        "!DS_CREATE_KEY!" !KEY_NAME!
        set "CREATE_RESULT=!ERRORLEVEL!"
        popd
        
        if not exist "!PRIVATE_KEY!" (
            echo ERROR: Failed to create key pair!
            echo DSCreateKey exit code: !CREATE_RESULT!
            goto :eof
        )
        
        echo Key pair created successfully!
        echo.
        echo IMPORTANT: Keep your .biprivatekey file safe and SECRET!
        echo            Only share the .bikey file with server owners.
        echo.
    )
) else (
    echo Using existing key pair: !KEY_NAME!
    echo.
)

REM Create Keys folder in mod directory
set "MOD_KEYS_DIR=!MOD_DIR!\Keys"
if not exist "!MOD_KEYS_DIR!" (
    if "!DRY_RUN!"=="1" (
        echo [DRY RUN] Would create mod keys directory: !MOD_KEYS_DIR!
    ) else (
        echo Creating mod keys directory: !MOD_KEYS_DIR!
        mkdir "!MOD_KEYS_DIR!"
    )
)

REM Copy public key to mod's Keys folder
if "!DRY_RUN!"=="1" (
    echo [DRY RUN] Would copy !KEY_NAME!.bikey to !MOD_KEYS_DIR!
    echo.
) else (
    echo Copying public key to mod folder...
    copy "!PUBLIC_KEY!" "!MOD_KEYS_DIR!\" >nul
    if !ERRORLEVEL! NEQ 0 (
        echo ERROR: Failed to copy public key!
        goto :eof
    )
    echo   Copied: !KEY_NAME!.bikey to !MOD_KEYS_DIR!
    echo.
)

REM Sign all PBO files
echo Signing PBO files...
set "SIGN_COUNT=0"
set "SIGN_ERRORS=0"

for %%F in ("!MOD_DIR!\Addons\*.pbo") do (
    if "!DRY_RUN!"=="1" (
        echo   [DRY RUN] Would sign: %%~nxF
        set /a SIGN_COUNT+=1
    ) else (
        echo   Signing: %%~nxF
        
        REM Remove existing signatures for this PBO
        del "!MOD_DIR!\Addons\%%~nF.*.bisign" 2>nul
        
        REM Sign the PBO
        "!DS_SIGN_FILE!" "!PRIVATE_KEY!" "%%F"
        set "SIGN_RESULT=!ERRORLEVEL!"
        
        if !SIGN_RESULT! EQU 0 (
            set /a SIGN_COUNT+=1
            echo     OK
        ) else (
            echo     FAILED - exit code !SIGN_RESULT!
            set /a SIGN_ERRORS+=1
        )
    )
)

echo.
if "!DRY_RUN!"=="1" (
    echo [DRY RUN] Would sign !SIGN_COUNT! PBO files.
) else (
    echo Signed !SIGN_COUNT! PBO files.
)

if !SIGN_ERRORS! GTR 0 (
    echo WARNING: !SIGN_ERRORS! files failed to sign!
)
echo.

REM List all signatures
if "!DRY_RUN!"=="0" (
    echo Signatures created:
    for %%F in ("!MOD_DIR!\Addons\*.bisign") do (
        echo   %%~nxF
    )
    echo.
)

:step4

echo ========================================
echo Step 4: Publishing to Steam Workshop
echo ========================================
echo.

if "!SKIP_PUBLISH!"=="1" (
    echo Skipping Workshop publish ^(--skip-publish specified^)
    echo.
    echo Your mod is ready at: !MOD_DIR!
    echo.
    echo To publish manually:
    echo   1. Open DayZ Tools
    echo   2. Go to Publisher
    echo   3. Select mod folder: !MOD_DIR!
    echo   4. Upload to Workshop
    echo.
    goto :done
)

REM Check for Workshop ID
if not defined WORKSHOP_ID (
    echo ERROR: WORKSHOP_ID not set!
    echo.
    echo For first-time publishing:
    echo   1. Open DayZ Tools Publisher GUI
    echo   2. Create your mod item manually
    echo   3. Note the Workshop ID from the URL
    echo   4. Add WORKSHOP_ID=XXXXXXXXXX to your .env file
    echo.
    echo Your mod is built and signed at: !MOD_DIR!
    goto :done
)

echo Workshop ID: !WORKSHOP_ID!
echo Mod directory: !MOD_DIR!

REM Prepare change note
if "!CHANGELOG!"=="" (
    set "CHANGELOG=Version !VERSION!"
    echo Change note: Version !VERSION! ^(default^)
) else (
    echo Change note: !CHANGELOG!
)
echo.

REM Verify mod directory exists
if not exist "!MOD_DIR!" (
    echo ERROR: Mod directory not found: !MOD_DIR!
    echo Please build the mod first or remove --skip-build
    goto :eof
)

REM Verify PBOs exist
set "PBO_CHECK=0"
for %%F in ("!MOD_DIR!\Addons\*.pbo") do set /a PBO_CHECK+=1
if !PBO_CHECK! EQU 0 (
    echo ERROR: No PBO files found in !MOD_DIR!\Addons
    echo Please build the mod first.
    goto :eof
)

echo Found !PBO_CHECK! PBO files to publish.
echo.

REM Create/update mod.cpp for Workshop
if exist "!MOD_DIR!\mod.cpp" (
    echo Updating mod.cpp version...
) else (
    echo Creating mod.cpp...
)
call :write_mod_cpp
goto :after_mod_cpp

:write_mod_cpp
(
    echo name = "Swarm";
    echo picture = "";
    echo actionName = "";
    echo action = "";
    echo logo = "";
    echo logoSmall = "";
    echo logoOver = "";
    echo tooltip = "Swarm - DayZ Mod Collection";
    echo overview = "A collection of DayZ mods including SwarmTweaks, SwarmAnimals, and SwarmEarplugs.";
    echo author = "Swarm Team";
    echo version = "!VERSION!";
) > "!MOD_DIR!\mod.cpp"
goto :eof

:after_mod_cpp
echo.

if "!DRY_RUN!"=="1" (
    echo [DRY RUN] Would publish to Steam Workshop using SteamCMD:
    echo   Mod Directory: !MOD_DIR!
    echo   Change Note: !CHANGELOG!
    echo   Workshop ID: !WORKSHOP_ID!
    echo   Workshop URL: https://steamcommunity.com/sharedfiles/filedetails/?id=!WORKSHOP_ID!
    echo.
    goto :done
)

REM Check for SteamCMD in scripts folder
set "STEAMCMD=%~dp0scripts\steamcmd.exe"
if not exist "!STEAMCMD!" (
    echo ERROR: steamcmd.exe not found at: !STEAMCMD!
    echo.
    echo Please download SteamCMD and place steamcmd.exe in the scripts folder.
    echo Download from: https://developer.valvesoftware.com/wiki/SteamCMD
    echo.
    echo Your mod is built and signed at: !MOD_DIR!
    goto :done
)

REM Check for Steam credentials
if not defined STEAM_USERNAME (
    echo ERROR: STEAM_USERNAME is required for SteamCMD publishing!
    echo.
    echo Add to your .env file:
    echo   STEAM_USERNAME=your_steam_username
    echo.
    goto :done
)

echo Steam username: !STEAM_USERNAME!
echo.

REM Create VDF file for Workshop upload
set "VDF_FILE=%~dp0workshop_upload.vdf"
echo Creating Workshop upload configuration...

(
    echo "workshopitem"
    echo {
    echo     "appid" "221100"
    echo     "publishedfileid" "!WORKSHOP_ID!"
    echo     "contentfolder" "!MOD_DIR!"
    echo     "changenote" "!CHANGELOG!"
    echo }
) > "!VDF_FILE!"

echo.
echo Starting SteamCMD Workshop upload...
echo.

REM Run SteamCMD
REM Note: If Steam Guard is enabled, you may need to enter the code
"!STEAMCMD!" +login "!STEAM_USERNAME!" +workshop_build_item "!VDF_FILE!" +quit
set "PUB_RESULT=!ERRORLEVEL!"

echo.

REM Clean up VDF file
del "!VDF_FILE!" 2>nul

if !PUB_RESULT! EQU 0 (
    echo.
    echo SUCCESS: Mod published to Steam Workshop!
    echo.
    echo View your mod: https://steamcommunity.com/sharedfiles/filedetails/?id=!WORKSHOP_ID!
) else (
    echo.
    echo WARNING: SteamCMD returned error code !PUB_RESULT!
    echo.
    echo Common issues and solutions:
    echo.
    echo   [Steam Guard?]
    echo     - Run SteamCMD manually once to cache credentials:
    echo       !STEAMCMD! +login !STEAM_USERNAME! +quit
    echo     - Enter your Steam Guard code when prompted
    echo     - Credentials will be cached for future runs
    echo.
    echo   [Wrong password?]
    echo     - SteamCMD uses cached credentials from previous login
    echo     - Run the manual login above to update credentials
    echo.
    echo   [First time publishing?]
    echo     - You must create the Workshop item manually first
    echo     - Use DayZ Tools Publisher GUI to create the initial item
    echo     - Then add the Workshop ID to your .env file
    echo.
    echo Your mod is built and signed at: !MOD_DIR!
)

:done
echo.
echo ========================================
echo Publish Complete
echo ========================================
echo.
echo Summary:
echo   Version: !VERSION!
echo   Key: !KEY_NAME!
echo   Mod folder: !MOD_DIR!
if "!SKIP_SIGN!"=="0" (
    echo   Public key: !MOD_KEYS_DIR!\!KEY_NAME!.bikey
    echo   PBOs signed: !SIGN_COUNT!
)
if defined WORKSHOP_ID (
    echo   Workshop ID: !WORKSHOP_ID!
    echo   Workshop URL: https://steamcommunity.com/sharedfiles/filedetails/?id=!WORKSHOP_ID!
)
if not "!CHANGELOG!"=="" (
    echo   Change note: !CHANGELOG!
)
if defined PUB_RESULT (
    echo   SteamCMD exit code: !PUB_RESULT!
)
if "!DRY_RUN!"=="1" (
    echo.
    echo   [DRY RUN - No actual changes were made]
)
echo.
echo Server owners need the .bikey file to verify your mod.
echo.

:eof
endlocal
pause

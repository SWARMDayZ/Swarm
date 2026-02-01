@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo Swarm Mod Publisher
echo ========================================
echo.

REM Load environment variables from .env file
if exist "%~dp0.env" (
    for /f "usebackq tokens=1,2 delims==" %%a in ("%~dp0.env") do set "%%a=%%b"
)

set "VERSION="
set "MOD_DIR=%~dp0dist\@Swarm"
set "KEYS_DIR=%~dp0keys"
set "KEY_NAME=Swarm"

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
shift
goto :parse_args

:check_args
REM Debug: show parsed values
echo Parsed arguments:
echo   VERSION=!VERSION!
echo   KEY_NAME=!KEY_NAME!
echo.

REM Check if version is provided
if "!VERSION!"=="" (
    echo ERROR: Version is required!
    echo.
    echo Usage: publish.bat --version X.X.X [--key KeyName]
    echo.
    echo Options:
    echo   --version X.X.X    Version number to build and publish
    echo   --key KeyName      Key name for signing - default: Swarm
    echo.
    echo This will:
    echo   1. Build all packages with the specified version
    echo   2. Sign all PBO files with .bikey/.biprivatekey
    echo   3. Upload to Steam Workshop using DayZ Tools Publisher
    echo.
    goto :eof
)

REM Check for DayZ Tools
if not defined DAYZ_TOOLS (
    echo ERROR: DAYZ_TOOLS environment variable not set!
    echo Please set it to your DayZ Tools installation path.
    echo Example: set DAYZ_TOOLS=C:\Program Files x86\Steam\steamapps\common\DayZ Tools
    goto :eof
)

echo Using DayZ Tools: !DAYZ_TOOLS!
echo.

REM Set tool paths
set "DS_SIGN_FILE=!DAYZ_TOOLS!\Bin\DsUtils\DSSignFile.exe"
set "DS_CREATE_KEY=!DAYZ_TOOLS!\Bin\DsUtils\DSCreateKey.exe"
set "PUBLISHER=!DAYZ_TOOLS!\Bin\Publisher\PublisherCmd.exe"

REM Verify signing tools exist
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
echo.

echo ========================================
echo Step 1: Building mod with version !VERSION!
echo ========================================
echo.

call "%~dp0build_all.bat" --version !VERSION!

if !ERRORLEVEL! NEQ 0 (
    echo ERROR: Build failed!
    goto :eof
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
echo Step 2: Signing PBO files
echo ========================================
echo.

REM Create keys directory if it doesn't exist
if not exist "!KEYS_DIR!" (
    echo Creating keys directory: !KEYS_DIR!
    mkdir "!KEYS_DIR!"
)

REM Check if keys exist, create if not
set "PRIVATE_KEY=!KEYS_DIR!\!KEY_NAME!.biprivatekey"
set "PUBLIC_KEY=!KEYS_DIR!\!KEY_NAME!.bikey"

echo Private key path: !PRIVATE_KEY!
echo Public key path: !PUBLIC_KEY!
echo.

if not exist "!PRIVATE_KEY!" (
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
) else (
    echo Using existing key pair: !KEY_NAME!
    echo.
)

REM Create Keys folder in mod directory
set "MOD_KEYS_DIR=!MOD_DIR!\Keys"
if not exist "!MOD_KEYS_DIR!" (
    echo Creating mod keys directory: !MOD_KEYS_DIR!
    mkdir "!MOD_KEYS_DIR!"
)

REM Copy public key to mod's Keys folder
echo Copying public key to mod folder...
copy "!PUBLIC_KEY!" "!MOD_KEYS_DIR!\" >nul
if !ERRORLEVEL! NEQ 0 (
    echo ERROR: Failed to copy public key!
    goto :eof
)
echo   Copied: !KEY_NAME!.bikey to !MOD_KEYS_DIR!
echo.

REM Sign all PBO files
echo Signing PBO files...
set "SIGN_COUNT=0"
set "SIGN_ERRORS=0"

for %%F in ("!MOD_DIR!\Addons\*.pbo") do (
    echo   Signing: %%~nxF
    
    REM Remove existing signatures for this PBO
    del "!MOD_DIR!\Addons\%%~nF.*.bisign" 2>nul
    
    REM Sign the PBO
    "!DS_SIGN_FILE!" "!PRIVATE_KEY!" "%%F"
    
    if !ERRORLEVEL! EQU 0 (
        set /a SIGN_COUNT+=1
        echo     OK
    ) else (
        echo     FAILED - exit code !ERRORLEVEL!
        set /a SIGN_ERRORS+=1
    )
)

echo.
echo Signed !SIGN_COUNT! PBO files.

if !SIGN_ERRORS! GTR 0 (
    echo WARNING: !SIGN_ERRORS! files failed to sign!
)
echo.

REM List all signatures
echo Signatures created:
for %%F in ("!MOD_DIR!\Addons\*.bisign") do (
    echo   %%~nxF
)
echo.

REM Check for Publisher
if not exist "!PUBLISHER!" (
    echo ========================================
    echo Step 3: Manual Workshop Upload Required
    echo ========================================
    echo.
    echo DayZ Tools Publisher not found at: !PUBLISHER!
    echo.
    echo Your mod is built and signed at: !MOD_DIR!
    echo.
    echo To upload manually:
    echo   1. Open DayZ Tools
    echo   2. Go to Publisher
    echo   3. Select mod folder: !MOD_DIR!
    echo   4. Upload to Workshop
    echo.
    goto :done
)

echo ========================================
echo Step 3: Publishing to Steam Workshop
echo ========================================
echo.
echo Mod directory: !MOD_DIR!
echo.

REM Note: PublisherCmd requires the mod to be configured first via the GUI
"!PUBLISHER!" update -modDir="!MOD_DIR!"
set "PUB_RESULT=!ERRORLEVEL!"

if !PUB_RESULT! NEQ 0 (
    echo.
    echo WARNING: Publisher returned error code !PUB_RESULT!
    echo.
    echo Common issues:
    echo   - First time? Configure the mod in DayZ Tools Publisher GUI first
    echo   - Steam not running? Start Steam and log in
    echo   - Auth issue? Re-authenticate in Steam client
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
echo   Public key: !MOD_KEYS_DIR!\!KEY_NAME!.bikey
echo   PBOs signed: !SIGN_COUNT!
echo.
echo Server owners need the .bikey file to verify your mod.
echo.

:eof
endlocal
pause

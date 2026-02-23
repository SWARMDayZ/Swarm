@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo Swarm Build System
echo ========================================
echo.

REM ============================================================
REM Load environment variables from .env
REM ============================================================

set "_envfile=%~dp0.env"
set "_psscript=%~dp0scripts\load_env.ps1"
set "_tmpenv=%TEMP%\_swarm_env.tmp"

if exist "!_envfile!" if exist "!_psscript!" (
    for %%V in (DAYZ_TOOLS DAYZ_SERVER DAYZ_CLIENT WORKSHOP_ID MODS DAYZ_PROFILE) do (
        if not defined %%V (
            powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName %%V > "!_tmpenv!" 2>nul
            if exist "!_tmpenv!" (
                set /p %%V=<"!_tmpenv!"
                del "!_tmpenv!" >nul 2>&1
            )
        )
    )
)

set "ADDON_BUILDER=!DAYZ_TOOLS!\Bin\AddonBuilder\AddonBuilder.exe"
set "PREPROCESS_SCRIPT=%~dp0scripts\update_version.ps1"
set "TEMP_DIR=%~dp0.build_temp"
set "SETTINGS_FILE=%~dp0.build_settings"

REM ============================================================
REM Discover packages
REM ============================================================

set PACKAGE_COUNT=0
set "PACKAGE_LIST="
for /d %%D in ("%~dp0packages\*") do (
    set /a PACKAGE_COUNT+=1
    if "!PACKAGE_LIST!"=="" (
        set "PACKAGE_LIST=%%~nxD"
    ) else (
        set "PACKAGE_LIST=!PACKAGE_LIST!,%%~nxD"
    )
)

if !PACKAGE_COUNT! EQU 0 (
    echo ERROR: No packages found in packages\ directory!
    goto :error
)

REM ============================================================
REM Load saved settings
REM ============================================================

set "SAVED_PACKAGES="
set "SAVED_PARAMS="
if exist "!SETTINGS_FILE!" (
    for /f "usebackq tokens=1,* delims==" %%A in ("!SETTINGS_FILE!") do (
        if "%%A"=="PACKAGES" set "SAVED_PACKAGES=%%B"
        if "%%A"=="PARAMS" set "SAVED_PARAMS=%%B"
    )
)

REM ============================================================
REM Interactive selection (via PowerShell)
REM ============================================================

set "MENU_OUTPUT=%TEMP%\_swarm_build_menu.tmp"

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\build_prompt.ps1" ^
    -Packages "!PACKAGE_LIST!" ^
    -SavedPackages "!SAVED_PACKAGES!" ^
    -SavedParams "!SAVED_PARAMS!" ^
    -OutputFile "!MENU_OUTPUT!"

if !ERRORLEVEL! NEQ 0 (
    echo.
    del "!MENU_OUTPUT!" >nul 2>&1
    goto :end
)

REM Read selections from menu output
set "SELECTED_PACKAGES="
set "SELECTED_PARAMS="
if exist "!MENU_OUTPUT!" (
    for /f "usebackq tokens=1,* delims==" %%A in ("!MENU_OUTPUT!") do (
        if "%%A"=="PACKAGES" set "SELECTED_PACKAGES=%%B"
        if "%%A"=="PARAMS" set "SELECTED_PARAMS=%%B"
    )
    del "!MENU_OUTPUT!" >nul 2>&1
)

if "!SELECTED_PACKAGES!"=="" (
    echo No packages selected!
    goto :error
)

REM Save settings for next time
echo PACKAGES=!SELECTED_PACKAGES!> "!SETTINGS_FILE!"
echo PARAMS=!SELECTED_PARAMS!>> "!SETTINGS_FILE!"

REM ============================================================
REM Determine actions from params
REM ============================================================

set "DO_VALIDATE="
set "DO_SIGN="
set "DO_LAUNCH="
for %%P in ("!SELECTED_PARAMS:;=" "!") do (
    if /i "%%~P"=="validate" set "DO_VALIDATE=1"
    if /i "%%~P"=="sign" set "DO_SIGN=1"
    if /i "%%~P"=="launch" set "DO_LAUNCH=1"
)

echo.
echo ========================================
echo Build Configuration
echo ========================================
echo   Packages: !SELECTED_PACKAGES!
echo   Validate: %DO_VALIDATE%
echo   Sign:     %DO_SIGN%
echo   Launch:   %DO_LAUNCH%
echo ========================================
echo.

REM ============================================================
REM Build each selected package
REM ============================================================

set BUILD_ERRORS=0

for %%P in ("!SELECTED_PACKAGES:;=" "!") do (
    set "PKG_NAME=%%~P"
    call :build_package "!PKG_NAME!"
    if !ERRORLEVEL! NEQ 0 (
        set BUILD_ERRORS=1
        goto :build_failed
    )
)

REM Cleanup temp
if exist "!TEMP_DIR!" rmdir /s /q "!TEMP_DIR!"

echo ========================================
echo All packages built successfully!
echo ========================================
echo.

REM ============================================================
REM Sign (if selected)
REM ============================================================

if defined DO_SIGN (
    call :do_sign
)

REM ============================================================
REM Validate (if selected)
REM ============================================================

if defined DO_VALIDATE (
    call :do_validate
    if !ERRORLEVEL! NEQ 0 (
        echo.
        echo ========================================
        echo VALIDATION FAILED
        echo ========================================
        if defined DO_LAUNCH (
            echo Skipping launch due to validation failure.
        )
        goto :error
    )
)

REM ============================================================
REM Launch (if selected)
REM ============================================================

if defined DO_LAUNCH (
    call :do_launch
)

goto :end

REM ============================================================
REM SUBROUTINE: Build a single package
REM ============================================================
:build_package
set "PKG=%~1"
echo ========================================
echo Building package: !PKG!
echo ========================================
echo.

set "PKG_DIR=%~dp0packages\!PKG!"
set "OUTPUT_DIR=%~dp0dist\@!PKG!\Addons"

REM Verify package exists
if not exist "!PKG_DIR!" (
    echo ERROR: Package directory not found: !PKG_DIR!
    exit /b 1
)

REM Read version (package-specific or root)
set "PKG_VERSION="
if exist "!PKG_DIR!\.version" (
    set /p PKG_VERSION=<"!PKG_DIR!\.version"
) else if exist "%~dp0.version" (
    set /p PKG_VERSION=<"%~dp0.version"
) else (
    echo ERROR: No version file found for !PKG!!
    exit /b 1
)
echo   Version: !PKG_VERSION!

REM Prepare output directory
if exist "!OUTPUT_DIR!" rmdir /s /q "!OUTPUT_DIR!"
mkdir "!OUTPUT_DIR!"

REM Count subfolders (addons)
set ADDON_COUNT=0
for /d %%D in ("!PKG_DIR!\*") do set /a ADDON_COUNT+=1

if !ADDON_COUNT! EQU 0 (
    echo   WARNING: No addon folders found in !PKG_DIR!, skipping PBO build.
    echo.
    goto :pkg_meta
)

REM Build each subfolder as a PBO
set ADDON_INDEX=0
for /d %%D in ("!PKG_DIR!\*") do (
    set /a ADDON_INDEX+=1
    set "ADDON_NAME=%%~nxD"
    echo   [!ADDON_INDEX!/!ADDON_COUNT!] Building !ADDON_NAME!...

    set "TEMP_SRC=!TEMP_DIR!\!ADDON_NAME!"
    powershell -NoProfile -ExecutionPolicy Bypass -File "!PREPROCESS_SCRIPT!" -SourceDir "%%D" -TempDir "!TEMP_SRC!" -Version "!PKG_VERSION!"

    "!ADDON_BUILDER!" "!TEMP_SRC!" "!OUTPUT_DIR!" -clear -packonly

    if !ERRORLEVEL! NEQ 0 (
        echo   ERROR: !ADDON_NAME! build failed!
        exit /b 1
    )
    echo   !ADDON_NAME! OK
    echo.
)

:pkg_meta
REM Process meta.cpp if it exists
if exist "!PKG_DIR!\meta.cpp" (
    echo   Copying mod metadata...

    if not exist "!TEMP_DIR!" mkdir "!TEMP_DIR!"
    set "TEMP_META=!TEMP_DIR!\meta_!PKG!.cpp"
    copy "!PKG_DIR!\meta.cpp" "!TEMP_META!" >nul

    REM Get current timestamp
    for /f %%T in ('powershell -NoProfile -Command "[DateTime]::UtcNow.Ticks"') do set "TIMESTAMP=%%T"

    REM Set publishedid (0 for dev builds)
    set "PUBLISHEDID=0"
    if defined WORKSHOP_ID set "PUBLISHEDID=!WORKSHOP_ID!"

    echo   Timestamp: !TIMESTAMP!
    echo   Published ID: !PUBLISHEDID!

    REM Replace placeholders
    powershell -NoProfile -ExecutionPolicy Bypass -Command "$c = Get-Content '!TEMP_META!' -Raw; $c = $c -replace '%%VERSION%%', '!PKG_VERSION!'; $c = $c -replace '%%TIMESTAMP%%', '!TIMESTAMP!'; $c = $c -replace '%%PUBLISHEDID%%', '!PUBLISHEDID!'; Set-Content '!TEMP_META!' -Value $c -NoNewline"
    copy "!TEMP_META!" "%~dp0dist\@!PKG!\meta.cpp" >nul
)

echo   !PKG! built successfully!
echo.
exit /b 0

REM ============================================================
REM SUBROUTINE: Sign all selected packages
REM ============================================================
:do_sign
echo ========================================
echo Signing packages...
echo ========================================
echo.

set "SIGN_TOOL=!DAYZ_TOOLS!\Bin\DsUtils\DSSignFile.exe"
set "PRIVATE_KEY=%~dp0keys\Swarm.biprivatekey"
set "PUBLIC_KEY=%~dp0keys\Swarm.bikey"

if not exist "!SIGN_TOOL!" (
    echo WARNING: DSSignFile.exe not found at: !SIGN_TOOL!
    echo Skipping signing...
    goto :eof
)

if not exist "!PRIVATE_KEY!" (
    echo WARNING: Private key not found at: !PRIVATE_KEY!
    echo Skipping signing...
    goto :eof
)

for %%P in ("!SELECTED_PACKAGES:;=" "!") do (
    set "PKG=%%~P"
    set "PKG_ADDONS=%~dp0dist\@!PKG!\Addons"
    set "PKG_KEYS=%~dp0dist\@!PKG!\Keys"

    if exist "!PKG_ADDONS!" (
        echo Signing @!PKG!...

        REM Create Keys folder and copy public key
        if not exist "!PKG_KEYS!" mkdir "!PKG_KEYS!"
        if exist "!PUBLIC_KEY!" copy "!PUBLIC_KEY!" "!PKG_KEYS!\" >nul 2>&1

        for %%f in ("!PKG_ADDONS!\*.pbo") do (
            echo   Signing: %%~nxf
            del "!PKG_ADDONS!\%%~nf.*.bisign" 2>nul
            "!SIGN_TOOL!" "!PRIVATE_KEY!" "%%f"
        )
        echo.
    )
)

echo Signing complete.
echo.
goto :eof

REM ============================================================
REM SUBROUTINE: Validate scripts using DayZ Server
REM ============================================================
:do_validate
echo ========================================
echo Validating scripts...
echo ========================================
echo.

if not defined DAYZ_SERVER (
    echo WARNING: DAYZ_SERVER not set, skipping validation.
    echo.
    exit /b 0
)

if not exist "!DAYZ_SERVER!\DayZServer_x64.exe" (
    echo ERROR: DayZServer_x64.exe not found at: !DAYZ_SERVER!
    exit /b 1
)

REM Build mod list
call :build_mod_list

echo Using DayZ Server: !DAYZ_SERVER!
echo Mod list: !MOD_LIST!
echo.

set "VALIDATE_DIR=%~dp0.validate_temp"
set "VALIDATE_CFG_DIR=%~dp0validate"
set "VALIDATE_PORT=2399"
set "VALIDATE_TIMEOUT=15"

REM Select config
set "VALIDATE_CFG="
if exist "!VALIDATE_CFG_DIR!\serverDZ.cfg" (
    set "VALIDATE_CFG=!VALIDATE_CFG_DIR!\serverDZ.cfg"
    echo Using custom config: serverDZ.cfg
) else if exist "!VALIDATE_CFG_DIR!\serverDZ.default.cfg" (
    set "VALIDATE_CFG=!VALIDATE_CFG_DIR!\serverDZ.default.cfg"
    echo Using default config: serverDZ.default.cfg
) else (
    echo WARNING: No validate config found, creating default...
    if not exist "!VALIDATE_CFG_DIR!" mkdir "!VALIDATE_CFG_DIR!"
    set "VALIDATE_CFG=!VALIDATE_CFG_DIR!\serverDZ.default.cfg"
    call :create_default_config
)

REM Prepare validate directory
if exist "!VALIDATE_DIR!" rmdir /s /q "!VALIDATE_DIR!"
mkdir "!VALIDATE_DIR!"
copy "!VALIDATE_CFG!" "!VALIDATE_DIR!\serverDZ.cfg" >nul

echo.
echo Starting DayZ Server for validation...
echo.

start "DayZScriptValidate" /B /MIN "!DAYZ_SERVER!\DayZServer_x64.exe" ^
    "-mod=!MOD_LIST!" ^
    -config=serverDZ.cfg ^
    -port=!VALIDATE_PORT! ^
    "-profiles=!VALIDATE_DIR!" ^
    -doLogs ^
    -adminLog

echo Waiting !VALIDATE_TIMEOUT! seconds...
timeout /t !VALIDATE_TIMEOUT! /nobreak >nul

echo Stopping validation server...
timeout /t 2 /nobreak >nul
taskkill /FI "WINDOWTITLE eq DayZScriptValidate" /F >nul 2>&1
timeout /t 3 /nobreak >nul
taskkill /IM DayZServer_x64.exe /F >nul 2>&1
timeout /t 2 /nobreak >nul

echo.
echo Analyzing logs for script errors...
echo.

REM Check for errors
set "FOUND_ERRORS="
for %%f in ("!VALIDATE_DIR!\crash_*.log") do (
    if exist "%%f" (
        echo Found crash log: %%~nxf
        set "FOUND_ERRORS=1"
    )
)
for %%f in ("!VALIDATE_DIR!\*.mdmp") do (
    if exist "%%f" (
        echo Found crash dump: %%~nxf
        set "FOUND_ERRORS=1"
    )
)
for %%f in ("!VALIDATE_DIR!\*.RPT") do (
    if exist "%%f" (
        findstr /i /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Can't compile" /c:"Cannot compile" /c:"Fatal error" /c:"ErrorModule" "%%f" >nul 2>&1
        if !ERRORLEVEL! EQU 0 set "FOUND_ERRORS=1"
    )
)

if defined FOUND_ERRORS (
    echo [ERROR] Script errors detected!
    echo.

    for %%f in ("!VALIDATE_DIR!\crash_*.log") do (
        if exist "%%f" (
            echo ========================================
            echo CRASH LOG: %%~nxf
            echo ========================================
            type "%%f"
            echo.
        )
    )
    for %%f in ("!VALIDATE_DIR!\script_*.log") do (
        if exist "%%f" (
            echo ========================================
            echo SCRIPT LOG: %%~nxf
            echo ========================================
            type "%%f"
            echo.
        )
    )
    echo ========================================
    echo Error details from RPT logs:
    echo ========================================
    for %%f in ("!VALIDATE_DIR!\*.RPT") do (
        if exist "%%f" (
            echo.
            echo From: %%~nxf
            findstr /i /n /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Fatal error" /c:"ErrorModule" "%%f"
        )
    )

    echo.
    echo VALIDATION FAILED - Logs preserved at: !VALIDATE_DIR!
    exit /b 1
)

echo [OK] No script errors detected!
echo.
if exist "!VALIDATE_DIR!" rmdir /s /q "!VALIDATE_DIR!"
echo VALIDATION PASSED
echo.
exit /b 0

REM ============================================================
REM SUBROUTINE: Launch server and client
REM ============================================================
:do_launch
echo ========================================
echo Launching server and client...
echo ========================================
echo.

if not defined DAYZ_SERVER (
    echo ERROR: DAYZ_SERVER not set!
    echo Please set DAYZ_SERVER in your .env file.
    goto :eof
)
if not defined DAYZ_CLIENT (
    echo ERROR: DAYZ_CLIENT not set!
    echo Please set DAYZ_CLIENT in your .env file.
    goto :eof
)
if not exist "!DAYZ_SERVER!\DayZServer_x64.exe" (
    echo ERROR: DayZServer_x64.exe not found at: !DAYZ_SERVER!
    goto :eof
)
if not exist "!DAYZ_CLIENT!\DayZ_BE.exe" (
    echo ERROR: DayZ_BE.exe not found at: !DAYZ_CLIENT!
    goto :eof
)

REM Kill existing processes
echo Checking for existing DayZ processes...
tasklist /FI "IMAGENAME eq DayZServer_x64.exe" 2>nul | findstr /i "DayZServer_x64.exe" >nul
if !ERRORLEVEL! EQU 0 (
    echo Stopping existing DayZ Server...
    taskkill /IM DayZServer_x64.exe /F >nul 2>&1
    timeout /t 2 /nobreak >nul
)
tasklist /FI "IMAGENAME eq DayZ_x64.exe" 2>nul | findstr /i "DayZ_x64.exe" >nul
if !ERRORLEVEL! EQU 0 (
    echo Stopping existing DayZ Client...
    taskkill /IM DayZ_x64.exe /F >nul 2>&1
    timeout /t 2 /nobreak >nul
)

REM Build mod list
call :build_mod_list

set "LAUNCH_TEMP=%~dp0.launch_temp"
set "VALIDATE_CFG_DIR=%~dp0validate"
set "SERVER_PORT=2302"

REM Prepare temp directory
if exist "!LAUNCH_TEMP!" rmdir /s /q "!LAUNCH_TEMP!"
mkdir "!LAUNCH_TEMP!"
mkdir "!LAUNCH_TEMP!\client"

REM Copy server config
set "SERVER_CFG="
if exist "!VALIDATE_CFG_DIR!\serverDZ.cfg" (
    set "SERVER_CFG=!VALIDATE_CFG_DIR!\serverDZ.cfg"
) else if exist "!VALIDATE_CFG_DIR!\serverDZ.default.cfg" (
    set "SERVER_CFG=!VALIDATE_CFG_DIR!\serverDZ.default.cfg"
) else (
    echo ERROR: No server config found in !VALIDATE_CFG_DIR!
    goto :eof
)
copy "!SERVER_CFG!" "!LAUNCH_TEMP!\serverDZ.cfg" >nul

REM Enable BattlEye
powershell -NoProfile -ExecutionPolicy Bypass -Command "$cfg='!LAUNCH_TEMP!\serverDZ.cfg'; $content=Get-Content $cfg -Raw; if ($content -notmatch 'BattlEye\s*=') { Add-Content $cfg \"`nBattlEye = 1;\" } else { $content = $content -replace 'BattlEye\s*=\s*0', 'BattlEye = 1'; Set-Content $cfg $content }"

REM Copy client profiles
if defined DAYZ_PROFILE (
    if exist "!DAYZ_PROFILE!" (
        echo Copying client profiles...
        mkdir "!LAUNCH_TEMP!\client\Users\DevClient" >nul 2>&1
        if exist "!DAYZ_PROFILE!\*.xml" copy "!DAYZ_PROFILE!\*.xml" "!LAUNCH_TEMP!\client\Users\DevClient" >nul 2>&1
        if exist "!DAYZ_PROFILE!\*.DayZProfile" copy "!DAYZ_PROFILE!\*.DayZProfile" "!LAUNCH_TEMP!\client\Users\DevClient" >nul 2>&1
        if exist "!DAYZ_PROFILE!\*.cfg" copy "!DAYZ_PROFILE!\*.cfg" "!LAUNCH_TEMP!\client\Users\DevClient" >nul 2>&1
    )
)

REM Setup COT admin permissions if COT is in mod list
echo !MOD_LIST! | findstr /i /c:"@Community-Online-Tools" /c:"@COT" >nul
if !ERRORLEVEL! EQU 0 (
    echo Setting up COT admin permissions...
    mkdir "!LAUNCH_TEMP!\PermissionsFramework\Roles" >nul 2>&1
    call :create_cot_permissions
)

echo.
echo ========================================
echo Starting DayZ Server...
echo ========================================
echo.
echo Mods: !MOD_LIST!
echo.

start "DayZ Server" "!DAYZ_SERVER!\DayZServer_x64.exe" ^
    "-mod=!MOD_LIST!" ^
    "-config=!LAUNCH_TEMP!\serverDZ.cfg" ^
    -port=!SERVER_PORT! ^
    "-profiles=!LAUNCH_TEMP!" ^
    -doLogs ^
    -adminLog ^
    -netLog ^
    -freezeCheck

echo Server starting... waiting for initialization...
timeout /t 10 /nobreak >nul

echo.
echo ========================================
echo Starting DayZ Client...
echo ========================================
echo.

start "DayZ Client" "!DAYZ_CLIENT!\DayZ_BE.exe" ^
    "-mod=!MOD_LIST!" ^
    "-profiles=!LAUNCH_TEMP!\client" ^
    -connect=127.0.0.1 ^
    -port=!SERVER_PORT! ^
    -name=DevClient

echo.
echo ========================================
echo Server and Client Launched!
echo ========================================
echo   Server port: !SERVER_PORT!
echo   Server logs: !LAUNCH_TEMP!
echo   Client connecting to 127.0.0.1:!SERVER_PORT!
echo.
echo Close their windows or use Task Manager to stop them.
echo.
goto :eof

REM ============================================================
REM SUBROUTINE: Build mod list (dependencies + our mods)
REM ============================================================
:build_mod_list
set "VALIDATE_MODS_DIR=%~dp0validate\mods"
set "MOD_LIST="
set "DEPENDENCY_COUNT=0"

REM Load dependency mods
if defined MODS (
    for %%m in ("!MODS:;=" "!") do (
        set "MOD_NAME=%%~m"
        set "MOD_NAME=!MOD_NAME:"=!"
        if exist "!VALIDATE_MODS_DIR!\!MOD_NAME!" (
            if "!MOD_LIST!"=="" (
                set "MOD_LIST=!VALIDATE_MODS_DIR!\!MOD_NAME!"
            ) else (
                set "MOD_LIST=!MOD_LIST!;!VALIDATE_MODS_DIR!\!MOD_NAME!"
            )
            set /a DEPENDENCY_COUNT+=1
        )
    )
) else (
    if exist "!VALIDATE_MODS_DIR!" (
        for /d %%d in ("!VALIDATE_MODS_DIR!\@*") do (
            if "!MOD_LIST!"=="" (
                set "MOD_LIST=%%d"
            ) else (
                set "MOD_LIST=!MOD_LIST!;%%d"
            )
            set /a DEPENDENCY_COUNT+=1
        )
    )
)

if !DEPENDENCY_COUNT! GTR 0 (
    echo Found !DEPENDENCY_COUNT! dependency mod^(s^)
)

REM Add our built mods (after dependencies)
for %%P in ("!SELECTED_PACKAGES:;=" "!") do (
    set "PKG=%%~P"
    if "!MOD_LIST!"=="" (
        set "MOD_LIST=%~dp0dist\@!PKG!"
    ) else (
        set "MOD_LIST=!MOD_LIST!;%~dp0dist\@!PKG!"
    )
)
goto :eof

REM ============================================================
REM SUBROUTINE: Create default server config for validation
REM ============================================================
:create_default_config
(
echo hostname = "Swarm Script Validate";
echo password = "";
echo passwordAdmin = "admin";
echo maxPlayers = 1;
echo verifySignatures = 0;
echo forceSameBuild = 0;
echo disableVoN = 1;
echo vonCodecQuality = 0;
echo disablePersonalLight = 1;
echo lightingConfig = 0;
echo serverTime = "2024/1/1/12/00";
echo serverTimeAcceleration = 0;
echo serverTimePersistent = 0;
echo guaranteedUpdates = 1;
echo loginQueueConcurrentPlayers = 1;
echo loginQueueMaxPlayers = 1;
echo instanceId = 99;
echo storageAutoFix = 0;
echo respawnTime = 0;
echo Missions = {
echo     class Swarm_Validate {
echo         template = "dayzOffline.chernarusplus";
echo     };
echo };
) > "!VALIDATE_CFG_DIR!\serverDZ.default.cfg"
echo Created: !VALIDATE_CFG_DIR!\serverDZ.default.cfg
goto :eof

REM ============================================================
REM SUBROUTINE: Create COT permissions
REM ============================================================
:create_cot_permissions
(
echo COT 2
echo COT.View 2
echo Admin 2
echo Admin.Player 2
echo Admin.Player.Heal 2
echo Admin.Player.Heal.Attachments 2
echo Admin.Player.Heal.Cargo 2
echo Admin.Player.Godmode 2
echo Admin.Player.Freeze 2
echo Admin.Player.Invisibility 2
echo Admin.Player.UnlimitedAmmo 2
echo Admin.Player.UnlimitedStamina 2
echo Admin.Player.Spectate 2
echo Admin.Player.Strip 2
echo Admin.Player.Dry 2
echo Admin.Player.StopBleeding 2
echo Admin.Player.BrokenLegs 2
echo Admin.Player.ReceiveDamageDealt 2
echo Admin.Player.Kick 2
echo Admin.Player.Ban 2
echo Admin.Player.Message 2
echo Admin.Player.CannotBeTargetedByAI 2
echo Admin.Player.AccessInventory 2
echo Admin.Player.RemoveCollision 2
echo Admin.Player.AdminNVG 2
echo Admin.Player.Teleport 2
echo Admin.Player.Teleport.Position 2
echo Admin.Player.Teleport.SenderTo 2
echo Admin.Player.Teleport.Previous 2
echo Admin.Player.Teleport.Location 2
echo Admin.Player.Teleport.Location.Add 2
echo Admin.Player.Teleport.Location.Refresh 2
echo Admin.Player.Teleport.Location.Remove 2
echo Admin.Player.Teleport.Cursor 2
echo Admin.Player.Teleport.Cursor.NoLog 2
echo Admin.Player.Teleport.View 2
echo Admin.Player.Permissions 2
echo Admin.Player.Roles 2
echo Admin.Player.Set 2
echo Admin.Player.Set.Health 2
echo Admin.Player.Set.Shock 2
echo Admin.Player.Set.Blood 2
echo Admin.Player.Set.Energy 2
echo Admin.Player.Set.Water 2
echo Admin.Player.Set.Stamina 2
echo Admin.Player.Set.HeatBuffer 2
echo Admin.Player.Set.BloodyHands 2
echo Admin.Player.Read 2
echo Admin.Transport 2
echo Admin.Transport.Repair 2
echo Admin.Map 2
echo Admin.Map.View 2
echo Admin.Map.Players 2
echo Admin.Roles 2
echo Admin.Roles.Update 2
echo Entity 2
echo Entity.Spawn 2
echo Entity.Spawn.Position 2
echo Entity.Spawn.Inventory 2
echo Entity.Delete 2
echo Entity.View 2
echo ESP 2
echo ESP.View 2
echo ESP.View.Player 2
echo ESP.View.Infected 2
echo ESP.View.Animal 2
echo ESP.View.Car 2
echo ESP.View.Boat 2
echo ESP.View.Weapon 2
echo ESP.View.Weapon.Archery 2
echo ESP.View.Weapon.BoltActionRifle 2
echo ESP.View.Weapon.BoltRifle 2
echo ESP.View.Weapon.Rifle 2
echo ESP.View.Weapon.Pistol 2
echo ESP.View.Weapon.Launcher 2
echo ESP.View.Item 2
echo ESP.View.Item.Tent 2
echo ESP.View.Item.BaseBuilding 2
echo ESP.View.Item.Food 2
echo ESP.View.Item.Explosive 2
echo ESP.View.Item.Book 2
echo ESP.View.Item.Container 2
echo ESP.View.Item.Transmitter 2
echo ESP.View.Item.Clothing 2
echo ESP.View.Item.Magazine 2
echo ESP.View.Item.Ammo 2
echo ESP.View.Item.Unknown 2
echo ESP.View.Immovable 2
echo ESP.Object 2
echo ESP.Object.SetPosition 2
echo ESP.Object.SetOrientation 2
echo ESP.Object.SetHealth 2
echo ESP.Object.Delete 2
echo ESP.Object.DuplicateAll 2
echo ESP.Object.DeleteAll 2
echo ESP.Object.MoveToCursor 2
echo ESP.Object.BaseBuilding 2
echo ESP.Object.BaseBuilding.Build 2
echo ESP.Object.BaseBuilding.Build.MaterialsNotRequired 2
echo ESP.Object.BaseBuilding.Dismantle 2
echo ESP.Object.BaseBuilding.Repair 2
echo ESP.Object.Car 2
echo ESP.Object.Car.Unstuck 2
echo ESP.Object.Car.Refuel 2
echo ESP.Object.Heal 2
echo Camera 2
echo Camera.View 2
echo Weather 2
echo Weather.QuickAction 2
echo Weather.QuickAction.Clear 2
echo Weather.QuickAction.Cloudy 2
echo Weather.QuickAction.Storm 2
echo Weather.QuickAction.Date 2
echo Weather.FreezeTime 2
echo Weather.Date 2
echo Weather.Wind 2
echo Weather.Wind.FunctionParams 2
echo Weather.Storm 2
echo Weather.Overcast 2
echo Weather.Fog 2
echo Weather.Fog.Dynamic 2
echo Weather.Rain 2
echo Weather.Rain.Thresholds 2
echo Weather.Snow 2
echo Weather.Snow.Thresholds 2
echo Weather.Preset 2
echo Weather.Preset.Use 2
echo Weather.Preset.Create 2
echo Weather.Preset.Update 2
echo Weather.Preset.Remove 2
echo Weather.View 2
echo Loadouts 2
echo Loadouts.View 2
echo Loadouts.Create 2
echo Loadouts.Backup 2
echo Loadouts.Load 2
echo Loadouts.Delete 2
echo Loadouts.Spawn 2
echo Loadouts.Spawn.Cursor 2
echo Loadouts.Spawn.Target 2
echo Loadouts.Spawn.SelectedPlayers 2
echo Vehicles 2
echo Vehicles.View 2
echo Vehicles.Delete 2
echo Vehicles.Delete.All 2
echo Vehicles.Delete.Destroyed 2
echo Vehicles.Teleport 2
echo Namalsk 2
echo Namalsk.View 2
echo Namalsk.Aurora 2
echo Namalsk.Aurora.Start 2
echo Namalsk.Aurora.Cancel 2
echo Namalsk.Blizzard 2
echo Namalsk.Blizzard.Start 2
echo Namalsk.Blizzard.Cancel 2
echo Namalsk.ExtremeCold 2
echo Namalsk.ExtremeCold.Start 2
echo Namalsk.ExtremeCold.Cancel 2
echo Namalsk.Snowfall 2
echo Namalsk.Snowfall.Start 2
echo Namalsk.Snowfall.Cancel 2
echo Namalsk.EVRStorm 2
echo Namalsk.EVRStorm.Start 2
echo Namalsk.EVRStorm.Cancel 2
echo Namalsk.EVRStormDeadly 2
echo Namalsk.EVRStormDeadly.Start 2
echo Namalsk.EVRStormDeadly.Cancel 2
echo Namalsk.HeavyFog 2
echo Namalsk.HeavyFog.Start 2
echo Namalsk.HeavyFog.Cancel 2
echo Actions 2
echo Actions.QuickActions 2
) > "!LAUNCH_TEMP!\PermissionsFramework\Roles\everyone.txt"
echo COT admin permissions configured.
echo.
goto :eof

REM ============================================================
REM Error / End handlers
REM ============================================================

:build_failed
echo.
echo ========================================
echo Build failed!
echo ========================================
if exist "!TEMP_DIR!" rmdir /s /q "!TEMP_DIR!"
endlocal
exit /b 1

:error
echo.
endlocal
exit /b 1

:end
endlocal
exit /b 0

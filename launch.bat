@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo DayZ Server and Client Launcher
echo ========================================
echo.

REM Load environment variables from .env file using PowerShell
set "_envfile=%~dp0.env"
set "_psscript=%~dp0scripts\load_env.ps1"
set "_tmpenv=%TEMP%\_dayz_env_launch.tmp"

if not exist "!_envfile!" goto :skip_env
if not exist "!_psscript!" goto :skip_env

REM Load DAYZ_SERVER
powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName DAYZ_SERVER > "!_tmpenv!" 2>nul
if exist "!_tmpenv!" (
    set /p DAYZ_SERVER=<"!_tmpenv!"
    del "!_tmpenv!" >nul 2>&1
)

REM Load DAYZ_CLIENT
powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName DAYZ_CLIENT > "!_tmpenv!" 2>nul
if exist "!_tmpenv!" (
    set /p DAYZ_CLIENT=<"!_tmpenv!"
    del "!_tmpenv!" >nul 2>&1
)

REM Load MODS
powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName MODS > "!_tmpenv!" 2>nul
if exist "!_tmpenv!" (
    set /p MODS=<"!_tmpenv!"
    del "!_tmpenv!" >nul 2>&1
)

REM Load DAYZ_TOOLS (for signing)
powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName DAYZ_TOOLS > "!_tmpenv!" 2>nul
if exist "!_tmpenv!" (
    set /p DAYZ_TOOLS=<"!_tmpenv!"
    del "!_tmpenv!" >nul 2>&1
)

REM Load DAYZ_PROFILE (for client profiles/logs)
powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName DAYZ_PROFILE > "!_tmpenv!" 2>nul
if exist "!_tmpenv!" (
    set /p DAYZ_PROFILE=<"!_tmpenv!"
    del "!_tmpenv!" >nul 2>&1
)

:skip_env

REM Configuration
set MOD_PATH=%~dp0dist\@Swarm
set VALIDATE_CFG_DIR=%~dp0validate
set LAUNCH_TEMP_DIR=%~dp0.launch_temp
set SERVER_PORT=2302
set RUN_BUILD=
set BUILD_TEST=

REM Parse arguments
:parse_args
if "%~1"=="" goto :done_args
if /i "%~1"=="--port" (
    set "SERVER_PORT=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--build" (
    set "RUN_BUILD=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--test" (
    set "BUILD_TEST=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--help" (
    goto :show_help
)
shift
goto :parse_args
:done_args

REM Kill existing DayZ processes first (needed before build to unlock files)
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

REM Check for DAYZ_CLIENT environment variable
if not defined DAYZ_CLIENT (
    echo ERROR: DAYZ_CLIENT environment variable not set!
    echo.
    echo Please set DAYZ_CLIENT in your .env file:
    echo   DAYZ_CLIENT=C:\Path\To\DayZ
    echo.
    goto :error
)

REM Check for DAYZ_SERVER environment variable
if not defined DAYZ_SERVER (
    echo ERROR: DAYZ_SERVER environment variable not set!
    echo.
    echo Please set DAYZ_SERVER in your .env file:
    echo   DAYZ_SERVER=C:\Path\To\DayZServer
    echo.
    goto :error
)

REM Verify DayZ Server exists
if not exist "!DAYZ_SERVER!\DayZServer_x64.exe" (
    echo ERROR: DayZServer_x64.exe not found at: !DAYZ_SERVER!
    echo.
    echo Please verify DAYZ_SERVER path is correct.
    goto :error
)

REM Verify DayZ Client exists
if not exist "!DAYZ_CLIENT!\DayZ_x64.exe" (
    echo ERROR: DayZ_x64.exe not found at: !DAYZ_CLIENT!
    echo.
    echo Please verify DAYZ_CLIENT path is correct.
    goto :error
)

REM Verify DayZ_BE.exe exists for proper BattleEye client launch
if not exist "!DAYZ_CLIENT!\DayZ_BE.exe" (
    echo.
    echo ERROR: DayZ_BE.exe not found in client!
    echo   Location: !DAYZ_CLIENT!\DayZ_BE.exe
    echo.
    echo This executable is required to launch DayZ with BattleEye.
    echo Please verify your DayZ installation.
    goto :error
)

REM Run build if --build flag was specified
if defined RUN_BUILD (
    echo ========================================
    echo Building mod with signatures...
    echo ========================================
    echo.
    
    REM Construct build command with optional --test flag
    set "BUILD_CMD=%~dp0build.bat"
    if defined BUILD_TEST (
        set "BUILD_CMD=!BUILD_CMD! --test"
    )
    
    call !BUILD_CMD!
    if !ERRORLEVEL! NEQ 0 (
        echo.
        echo ERROR: Build failed!
        goto :error
    )
    
    REM Sign the mod
    echo.
    echo Signing mod...
    set "SIGN_TOOL=!DAYZ_TOOLS!\Bin\DsUtils\DSSignFile.exe"
    set "PRIVATE_KEY=%~dp0keys\Swarm.biprivatekey"
    
    if not exist "!SIGN_TOOL!" (
        echo WARNING: DSSignFile.exe not found at: !SIGN_TOOL!
        echo Skipping signing...
    ) else if not exist "!PRIVATE_KEY!" (
        echo WARNING: Private key not found at: !PRIVATE_KEY!
        echo Skipping signing...
    ) else (
        for %%f in ("%MOD_PATH%\Addons\*.pbo") do (
            echo   Signing: %%~nxf
            "!SIGN_TOOL!" "!PRIVATE_KEY!" "%%f"
        )
        echo Signing complete.
    )
    echo.
)

echo Using DayZ Server: !DAYZ_SERVER!
echo Using DayZ Client: !DAYZ_CLIENT!
echo Server Port: %SERVER_PORT%
echo.

REM Check if mod is built
if not exist "%MOD_PATH%\Addons" (
    echo ERROR: Mod not built! Run build.bat first.
    echo Expected: %MOD_PATH%\Addons
    goto :error
)

REM Select server config (serverDZ.cfg if exists, otherwise serverDZ.default.cfg)
set SERVER_CFG=
if exist "%VALIDATE_CFG_DIR%\serverDZ.cfg" (
    set "SERVER_CFG=%VALIDATE_CFG_DIR%\serverDZ.cfg"
    echo Using custom config: serverDZ.cfg
) else if exist "%VALIDATE_CFG_DIR%\serverDZ.default.cfg" (
    set "SERVER_CFG=%VALIDATE_CFG_DIR%\serverDZ.default.cfg"
    echo Using default config: serverDZ.default.cfg
) else (
    echo ERROR: No server config found!
    echo.
    echo Please create one of:
    echo   - %VALIDATE_CFG_DIR%\serverDZ.cfg ^(custom^)
    echo   - %VALIDATE_CFG_DIR%\serverDZ.default.cfg ^(default^)
    goto :error
)

REM Prepare launch temp directory for server profiles/logs
echo Preparing server temp directory...
if exist "%LAUNCH_TEMP_DIR%" rmdir /s /q "%LAUNCH_TEMP_DIR%"
mkdir "%LAUNCH_TEMP_DIR%"
mkdir "%LAUNCH_TEMP_DIR%\client"

REM Copy server config to temp directory
copy "!SERVER_CFG!" "%LAUNCH_TEMP_DIR%\serverDZ.cfg" >nul

REM Enable BattlEye in server config
powershell -NoProfile -ExecutionPolicy Bypass -Command "$cfg='%LAUNCH_TEMP_DIR%\serverDZ.cfg'; $content=Get-Content $cfg -Raw; if ($content -notmatch 'BattlEye\s*=') { Add-Content $cfg \"`nBattlEye = 1;\" } else { $content = $content -replace 'BattlEye\s*=\s*0', 'BattlEye = 1'; Set-Content $cfg $content }"

REM Grant admin role to any existing PermissionsFramework player files
if exist "%LAUNCH_TEMP_DIR%\PermissionsFramework\Players\*.json" (
    echo Granting admin role to existing players...
    for %%f in ("%LAUNCH_TEMP_DIR%\PermissionsFramework\Players\*.json") do (
        powershell -NoProfile -ExecutionPolicy Bypass -Command "$file='%%f'; $json=Get-Content $file | ConvertFrom-Json; if ($json.Roles -notcontains 'admin') { $json.Roles += 'admin' }; $json | ConvertTo-Json -Depth 10 | Set-Content $file"
    )
)
echo.

REM Copy profiles from existing DayZ profile directory to temp client directory (if DAYZ_PROFILE is set)
if defined DAYZ_PROFILE (
    echo Copying existing profiles from %DAYZ_PROFILE% to temp client directory...
    set "FILES_COPIED=0"
    mkdir "%LAUNCH_TEMP_DIR%\client\Users\DevClient" >nul 2>&1
    
    if exist "%DAYZ_PROFILE%\*.xml" (
        copy "%DAYZ_PROFILE%\*.xml" "%LAUNCH_TEMP_DIR%\client\Users\DevClient" >nul
        set "FILES_COPIED=1"
    )
    
    if exist "%DAYZ_PROFILE%\*.DayZProfile" (
        copy "%DAYZ_PROFILE%\*.DayZProfile" "%LAUNCH_TEMP_DIR%\client\Users\DevClient" >nul
        set "FILES_COPIED=1"
    )
    
    if exist "%DAYZ_PROFILE%\*.cfg" (
        copy "%DAYZ_PROFILE%\*.cfg" "%LAUNCH_TEMP_DIR%\client\Users\DevClient" >nul
        set "FILES_COPIED=1"
    )
    
    if "!FILES_COPIED!"=="1" (
        echo Profiles copied.
    ) else (
        echo No profile files found in %DAYZ_PROFILE%.
    )
    echo.
)

REM Build mod list
set "VALIDATE_MODS_DIR=%VALIDATE_CFG_DIR%\mods"
set "MOD_LIST="
set "DEPENDENCY_COUNT=0"

REM Use MODS env variable if set, otherwise scan validate\mods directory
if defined MODS (
    echo Using MODS from environment variable: !MODS!
    
    REM Replace semicolon separators with mod paths
    for %%m in ("!MODS:;=" "!") do (
        set "MOD_NAME=%%~m"
        REM Remove quotes
        set "MOD_NAME=!MOD_NAME:"=!"
        
        REM Check in validate\mods directory
        if exist "%VALIDATE_MODS_DIR%\!MOD_NAME!" (
            if "!MOD_LIST!"=="" (
                set "MOD_LIST=%VALIDATE_MODS_DIR%\!MOD_NAME!"
            ) else (
                set "MOD_LIST=!MOD_LIST!;%VALIDATE_MODS_DIR%\!MOD_NAME!"
            )
            set /a DEPENDENCY_COUNT+=1
        ) else (
            echo WARNING: Mod !MOD_NAME! not found in %VALIDATE_MODS_DIR%
        )
    )
    
    if !DEPENDENCY_COUNT! GTR 0 (
        echo Found !DEPENDENCY_COUNT! dependency mod^(s^) from MODS env:
        for %%m in ("!MODS:;=" "!") do (
            set "MOD_NAME=%%~m"
            set "MOD_NAME=!MOD_NAME:"=!"
            echo   - !MOD_NAME!
        )
        echo.
    )
) else (
    REM No MODS env variable, scan validate\mods directory
    if exist "%VALIDATE_MODS_DIR%" (
        for /d %%d in ("%VALIDATE_MODS_DIR%\@*") do (
            if "!MOD_LIST!"=="" (
                set "MOD_LIST=%%d"
            ) else (
                set "MOD_LIST=!MOD_LIST!;%%d"
            )
            set /a DEPENDENCY_COUNT+=1
        )
        
        if !DEPENDENCY_COUNT! GTR 0 (
            echo Found !DEPENDENCY_COUNT! dependency mod^(s^) in validate\mods:
            for /d %%d in ("%VALIDATE_MODS_DIR%\@*") do (
                echo   - %%~nxd
            )
            echo.
        )
    )
)

REM Add @Swarm at the end (loads after dependencies)
if "!MOD_LIST!"=="" (
    set "MOD_LIST=%MOD_PATH%"
) else (
    set "MOD_LIST=!MOD_LIST!;%MOD_PATH%"
)

if !DEPENDENCY_COUNT! GTR 0 (
    echo @Swarm will load last ^(after dependencies^)
    echo.
) else (
    echo No dependency mods found
    echo.
)

REM Check if Community Online Tools is in the mod list and setup PermissionsFramework
echo !MOD_LIST! | findstr /i /c:"@Community-Online-Tools" /c:"@COT" >nul
if !ERRORLEVEL! EQU 0 (
    echo Detected Community Online Tools - Setting up admin permissions...
    mkdir "%LAUNCH_TEMP_DIR%\PermissionsFramework\Roles" >nul 2>&1
    
    REM Create everyone.txt with full admin permissions
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
    ) > "%LAUNCH_TEMP_DIR%\PermissionsFramework\Roles\everyone.txt"
    echo Admin permissions configured for everyone role.
    echo.
)

echo.
echo ========================================
echo Starting DayZ Server...
echo ========================================
echo.
echo Mods: !MOD_LIST!
echo.

REM Start server in a new window (with BattleEye enabled for stability)
start "DayZ Server - @Swarm" "!DAYZ_SERVER!\DayZServer_x64.exe" ^
    "-mod=!MOD_LIST!" ^
    "-config=%LAUNCH_TEMP_DIR%\serverDZ.cfg" ^
    -port=%SERVER_PORT% ^
    "-profiles=%LAUNCH_TEMP_DIR%" ^
    -doLogs ^
    -adminLog ^
    -netLog ^
    -freezeCheck

echo Server starting in new window...
echo Waiting for server to initialize...
timeout /t 10 /nobreak >nul

echo.
echo ========================================
echo Starting DayZ Client...
echo ========================================
echo.

REM Start client with mods and auto-connect (using DayZ_BE.exe for proper BattleEye launch)
REM Create a client config
echo. > "%LAUNCH_TEMP_DIR%\client\client.cfg"

start "DayZ Client - @Swarm" "!DAYZ_CLIENT!\DayZ_BE.exe" ^
    "-mod=!MOD_LIST!" ^
    "-profiles=%LAUNCH_TEMP_DIR%\client" ^
    -connect=127.0.0.1 ^
    -port=%SERVER_PORT% ^
    -name=DevClient

echo.
echo ========================================
echo Server and Client Launched!
echo ========================================
echo.
echo Server is running on port %SERVER_PORT%
echo Server logs/profiles: %LAUNCH_TEMP_DIR%
echo.
echo Client auto-connecting to 127.0.0.1:%SERVER_PORT%
echo.
echo Both processes are running in separate windows.
echo Close their windows or use Task Manager to stop them.
echo.
goto :end

:show_help
echo.
echo Usage: launch.bat [options]
echo.
echo Options:
echo   --port N       Set server port (default: 2302)
echo   --build        Build the mod with signatures before launching
echo   --test         Include test addons (SwarmTest) in build
echo   --help         Show this help message
echo.
echo Environment Variables:
echo   DAYZ_SERVER    Path to DayZ Server installation (required)
echo   DAYZ_CLIENT    Path to DayZ Client installation (required)
echo   DAYZ_TOOLS     Path to DayZ Tools (required for --build signing)
echo   MODS           Mod load order (optional, e.g., @CF;@CommunityFramework)
echo.
echo Server Config:
echo   The script looks for server configs in this order:
echo     1. validate\serverDZ.cfg         (custom)
echo     2. validate\serverDZ.default.cfg (default)
echo.
echo Dependency Mods:
echo   If MODS env variable is not set, the script will automatically
echo   detect mods in validate\mods\
echo.
echo Example:
echo   launch.bat
echo   launch.bat --build
echo   launch.bat --build --test
echo   launch.bat --port 2303
echo   launch.bat --build --test --port 2303
echo.
goto :end

:error
echo.
endlocal
exit /b 1

:end
endlocal
exit /b 0

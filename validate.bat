@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo DayZ Script Validate
echo ========================================
echo.

REM Load environment variables from .env file using PowerShell
set "_envfile=%~dp0.env"
set "_psscript=%~dp0scripts\load_env.ps1"
set "_tmpenv=%TEMP%\_dayz_env_val.tmp"
if not exist "!_envfile!" goto :skip_env
if not exist "!_psscript!" goto :skip_env

REM Load DAYZ_SERVER
powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName DAYZ_SERVER > "!_tmpenv!" 2>nul
if exist "!_tmpenv!" (
    set /p DAYZ_SERVER=<"!_tmpenv!"
    del "!_tmpenv!" >nul 2>&1
)

REM Load MODS
powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName MODS > "!_tmpenv!" 2>nul
if exist "!_tmpenv!" (
    set /p MODS=<"!_tmpenv!"
    del "!_tmpenv!" >nul 2>&1
)

:skip_env

REM Configuration
set MOD_PATH=%~dp0dist\@Swarm
set VALIDATE_DIR=%~dp0.validate_temp
set VALIDATE_CFG_DIR=%~dp0validate
set VALIDATE_PORT=2399
set VALIDATE_TIMEOUT=15
set SCRIPT_ERRORS_FOUND=0

REM Parse arguments
:parse_args
if "%~1"=="" goto :done_args
if /i "%~1"=="--timeout" (
    set "VALIDATE_TIMEOUT=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--skip-build" (
    set "SKIP_BUILD=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--help" (
    goto :show_help
)
shift
goto :parse_args
:done_args

REM Check for DAYZ_SERVER environment variable
if not defined DAYZ_SERVER (
    echo ERROR: DAYZ_SERVER environment variable not set!
    echo.
    echo Please set DAYZ_SERVER in your .env file:
    echo   DAYZ_SERVER=C:\Path\To\DayZServer
    echo.
    echo Or set it as a system environment variable.
    goto :error
)

REM Verify DayZ Server exists
if not exist "!DAYZ_SERVER!\DayZServer_x64.exe" (
    echo ERROR: DayZServer_x64.exe not found at: !DAYZ_SERVER!
    echo.
    echo Please verify DAYZ_SERVER path is correct.
    goto :error
)

echo Using DayZ Server: !DAYZ_SERVER!
echo Validate timeout: %VALIDATE_TIMEOUT% seconds
echo.

REM Check if mod is built
if not exist "%MOD_PATH%\Addons" (
    echo ERROR: Mod not built! Run build.bat first.
    echo Expected: %MOD_PATH%\Addons
    goto :error
)

REM Select validate config (serverDZ.cfg if exists, otherwise serverDZ.default.cfg)
set VALIDATE_CFG=
if exist "%VALIDATE_CFG_DIR%\serverDZ.cfg" (
    set "VALIDATE_CFG=%VALIDATE_CFG_DIR%\serverDZ.cfg"
    echo Using custom config: serverDZ.cfg
) else if exist "%VALIDATE_CFG_DIR%\serverDZ.default.cfg" (
    set "VALIDATE_CFG=%VALIDATE_CFG_DIR%\serverDZ.default.cfg"
    echo Using default config: serverDZ.default.cfg
) else (
    echo ERROR: No validate config found!
    echo.
    echo Please create one of:
    echo   - %VALIDATE_CFG_DIR%\serverDZ.cfg ^(custom^)
    echo   - %VALIDATE_CFG_DIR%\serverDZ.default.cfg ^(default^)
    echo.
    echo Creating default validate config...
    if not exist "%VALIDATE_CFG_DIR%" mkdir "%VALIDATE_CFG_DIR%"
    set "VALIDATE_CFG=%VALIDATE_CFG_DIR%\serverDZ.default.cfg"
    call :create_default_config
)

REM Build mod list (dependency mods first, then @Swarm last)
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

REM Prepare validate directory
echo Preparing validate environment...
if exist "%VALIDATE_DIR%" rmdir /s /q "%VALIDATE_DIR%"
mkdir "%VALIDATE_DIR%"

REM Copy validate config to temp directory
copy "!VALIDATE_CFG!" "%VALIDATE_DIR%\serverDZ.cfg" >nul

echo.
echo ========================================
echo Starting DayZ Server for validate...
echo ========================================
echo.
echo Mods: !MOD_LIST!
echo.

REM Start server in background
start "DayZScriptValidate" /B /MIN "!DAYZ_SERVER!\DayZServer_x64.exe" ^
    "-mod=!MOD_LIST!" ^
    -config=serverDZ.cfg ^
    -port=%VALIDATE_PORT% ^
    "-profiles=%VALIDATE_DIR%" ^
    -doLogs ^
    -adminLog

REM Wait for timeout duration
echo Waiting for !VALIDATE_TIMEOUT! seconds...
timeout /t !VALIDATE_TIMEOUT! /nobreak >nul

echo.
echo Timeout reached, checking logs...

:check_results
echo.
echo ========================================
echo Stopping validate server...
echo ========================================

REM Give server time to flush logs before killing
timeout /t 2 /nobreak >nul

REM Kill the server process
taskkill /FI "WINDOWTITLE eq DayZScriptValidate" /F >nul 2>&1
timeout /t 3 /nobreak >nul
taskkill /IM DayZServer_x64.exe /F >nul 2>&1

REM Wait for logs to fully flush after kill
timeout /t 2 /nobreak >nul

echo.
echo ========================================
echo Analyzing logs for script errors...
echo ========================================
echo.

REM Check for crash logs (most reliable error indicator)
set "FOUND_ERRORS="
for %%f in ("%VALIDATE_DIR%\crash_*.log") do (
    if exist "%%f" (
        echo Found crash log: %%~nxf
        set "FOUND_ERRORS=1"
    )
)

REM Check for crash dumps
for %%f in ("%VALIDATE_DIR%\*.mdmp") do (
    if exist "%%f" (
        echo Found crash dump: %%~nxf
        set "FOUND_ERRORS=1"
    )
)

REM Check for script errors in RPT files
for %%f in ("%VALIDATE_DIR%\*.RPT") do (
    if exist "%%f" (
        findstr /i /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Can't compile" /c:"Cannot compile" /c:"Fatal error" /c:"ErrorModule" "%%f" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            set "FOUND_ERRORS=1"
        )
    )
)

if defined FOUND_ERRORS (
    echo [ERROR] Script errors detected!
    echo.
    
    REM Display crash logs
    for %%f in ("%VALIDATE_DIR%\crash_*.log") do (
        if exist "%%f" (
            echo ========================================
            echo CRASH LOG: %%~nxf
            echo ========================================
            type "%%f"
            echo.
        )
    )
    
    REM Display script logs (contains real error details)
    for %%f in ("%VALIDATE_DIR%\script_*.log") do (
        if exist "%%f" (
            echo ========================================
            echo SCRIPT LOG: %%~nxf
            echo ========================================
            type "%%f"
            echo.
        )
    )
    
    REM Display error lines from RPT
    echo ========================================
    echo Error details from RPT logs:
    echo ========================================
    for %%f in ("%VALIDATE_DIR%\*.RPT") do (
        if exist "%%f" (
            echo.
            echo From: %%~nxf
            findstr /i /n /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Fatal error" /c:"ErrorModule" "%%f"
        )
    )
    
    echo.
    echo ========================================
    echo VALIDATE FAILED - Script errors found
    echo ========================================
    echo.
    echo Logs preserved at: %VALIDATE_DIR%
    goto :error_keep_logs
)

echo [OK] No script errors detected!
echo.

REM Cleanup
echo Cleaning up validate files...
if exist "%VALIDATE_DIR%" rmdir /s /q "%VALIDATE_DIR%"

echo.
echo ========================================
echo VALIDATE PASSED
echo ========================================
goto :end

:create_default_config
REM Create a minimal server config for validate
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
) > "%VALIDATE_CFG_DIR%\serverDZ.default.cfg"
echo Created: %VALIDATE_CFG_DIR%\serverDZ.default.cfg
goto :eof

:show_help
echo.
echo Usage: validate.bat [options]
echo.
echo Options:
echo   --timeout N    Set validate timeout in seconds (default: 60)
echo   --skip-build   Skip build check (assume mod is already built)
echo   --help         Show this help message
echo.
echo Environment Variables:
echo   DAYZ_SERVER    Path to DayZ Server installation (required)
echo.
echo Server Config:
echo   The script looks for server configs in this order:
echo     1. validate\serverDZ.cfg         (custom - for your specific map)
echo     2. validate\serverDZ.default.cfg (default - Chernarus offline)
echo.
echo   To use a different map, copy serverDZ.default.cfg to serverDZ.cfg
echo   and modify the Missions class template.
echo.
echo Dependency Mods:
echo   If your mod depends on other mods (e.g., CF, COT), place them in:
echo     validate\mods\
echo.
echo   Example structure:
echo     validate\mods\@CF\
echo     validate\mods\@Community-Online-Tools\
echo.
echo   The script will automatically detect and load these mods.
echo.
echo Example:
echo   validate.bat --timeout 90
echo.
goto :end

:error_keep_logs
endlocal
exit /b 1

:error
echo.
if exist "%VALIDATE_DIR%" rmdir /s /q "%VALIDATE_DIR%"
endlocal
exit /b 1

:end
endlocal
exit /b 0

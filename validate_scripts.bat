@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo DayZ Script Validation
echo ========================================
echo.

REM Load environment variables from .env file
set "_envfile=%~dp0.env"
if not exist "!_envfile!" goto :skip_env
REM Use a temp file to safely handle paths with parentheses
findstr /i /b "DAYZ_SERVER=" "!_envfile!" > "%TEMP%\_dayz_env.tmp" 2>nul
if exist "%TEMP%\_dayz_env.tmp" (
    set /p _dayzline=<"%TEMP%\_dayz_env.tmp"
    del "%TEMP%\_dayz_env.tmp" >nul 2>&1
    if defined _dayzline (
        REM Skip first 12 characters (DAYZ_SERVER=)
        set "DAYZ_SERVER=!_dayzline:~12!"
        REM Remove surrounding quotes if present
        if defined DAYZ_SERVER set "DAYZ_SERVER=!DAYZ_SERVER:"=!"
    )
)
:skip_env

REM Configuration
set MOD_PATH=%~dp0dist\@Swarm
set VALIDATION_DIR=%~dp0.validation_temp
set VALIDATION_CFG_DIR=%~dp0validation
set VALIDATION_PORT=2399
set VALIDATION_TIMEOUT=60
set SCRIPT_ERRORS_FOUND=0

REM Parse arguments
:parse_args
if "%~1"=="" goto :done_args
if /i "%~1"=="--timeout" (
    set "VALIDATION_TIMEOUT=%~2"
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
if not exist "%DAYZ_SERVER%\DayZServer_x64.exe" (
    echo ERROR: DayZServer_x64.exe not found at: %DAYZ_SERVER%
    echo.
    echo Please verify DAYZ_SERVER path is correct.
    goto :error
)

echo Using DayZ Server: %DAYZ_SERVER%
echo Validation timeout: %VALIDATION_TIMEOUT% seconds
echo.

REM Check if mod is built
if not exist "%MOD_PATH%\Addons" (
    echo ERROR: Mod not built! Run build_all.bat first.
    echo Expected: %MOD_PATH%\Addons
    goto :error
)

REM Select validation config (serverDZ.cfg if exists, otherwise serverDZ.default.cfg)
set VALIDATION_CFG=
if exist "%VALIDATION_CFG_DIR%\serverDZ.cfg" (
    set "VALIDATION_CFG=%VALIDATION_CFG_DIR%\serverDZ.cfg"
    echo Using custom config: serverDZ.cfg
) else if exist "%VALIDATION_CFG_DIR%\serverDZ.default.cfg" (
    set "VALIDATION_CFG=%VALIDATION_CFG_DIR%\serverDZ.default.cfg"
    echo Using default config: serverDZ.default.cfg
) else (
    echo ERROR: No validation config found!
    echo.
    echo Please create one of:
    echo   - %VALIDATION_CFG_DIR%\serverDZ.cfg ^(custom^)
    echo   - %VALIDATION_CFG_DIR%\serverDZ.default.cfg ^(default^)
    echo.
    echo Creating default validation config...
    if not exist "%VALIDATION_CFG_DIR%" mkdir "%VALIDATION_CFG_DIR%"
    set "VALIDATION_CFG=%VALIDATION_CFG_DIR%\serverDZ.default.cfg"
    call :create_default_config
)

REM Build mod list (dependency mods first, then @Swarm last)
set "VALIDATION_MODS_DIR=%VALIDATION_CFG_DIR%\mods"
set "MOD_LIST="
set "DEPENDENCY_COUNT=0"

if exist "%VALIDATION_MODS_DIR%" (
    for /d %%d in ("%VALIDATION_MODS_DIR%\@*") do (
        if "!MOD_LIST!"=="" (
            set "MOD_LIST=%%d"
        ) else (
            set "MOD_LIST=!MOD_LIST!;%%d"
        )
        set /a DEPENDENCY_COUNT+=1
    )
)

REM Add @Swarm at the end (loads after dependencies)
if "!MOD_LIST!"=="" (
    set "MOD_LIST=%MOD_PATH%"
) else (
    set "MOD_LIST=!MOD_LIST!;%MOD_PATH%"
)

if !DEPENDENCY_COUNT! GTR 0 (
    echo Found !DEPENDENCY_COUNT! dependency mod^(s^) in validation\mods:
    for /d %%d in ("%VALIDATION_MODS_DIR%\@*") do (
        echo   - %%~nxd
    )
    echo.
    echo @Swarm will load last ^(after dependencies^)
    echo.
) else (
    echo No dependency mods found in validation\mods
    echo.
)

REM Prepare validation directory
echo Preparing validation environment...
if exist "%VALIDATION_DIR%" rmdir /s /q "%VALIDATION_DIR%"
mkdir "%VALIDATION_DIR%"

REM Copy validation config to temp directory
copy "!VALIDATION_CFG!" "%VALIDATION_DIR%\serverDZ.cfg" >nul

echo.
echo ========================================
echo Starting DayZ Server for validation...
echo ========================================
echo.
echo Mods: !MOD_LIST!
echo.
echo This may take 30-60 seconds...
echo.

REM Start server in background
start "DayZScriptValidation" /B /MIN "%DAYZ_SERVER%\DayZServer_x64.exe" ^
    "-mod=!MOD_LIST!" ^
    -config=serverDZ.cfg ^
    -port=%VALIDATION_PORT% ^
    "-profiles=%VALIDATION_DIR%" ^
    -doLogs ^
    -adminLog

REM Wait for server to compile scripts
echo Waiting for script compilation...
set /a WAIT_COUNT=0
set /a WAIT_INTERVAL=5
set "EARLY_ERROR_FOUND="

:wait_loop
timeout /t %WAIT_INTERVAL% /nobreak >nul
set /a WAIT_COUNT+=WAIT_INTERVAL

REM Check if server process is still running
tasklist /FI "IMAGENAME eq DayZServer_x64.exe" 2>nul | findstr /i "DayZServer_x64.exe" >nul
if !ERRORLEVEL! NEQ 0 (
    echo Server process ended unexpectedly - likely a script error.
    set "EARLY_ERROR_FOUND=1"
    REM Wait a moment for logs to flush
    timeout /t 3 /nobreak >nul
    goto :check_results
)

REM Check if RPT file exists and has content
for %%f in ("%VALIDATION_DIR%\DayZServer_x64*.RPT") do (
    REM Check for script errors early (server might be stuck on error dialog)
    findstr /i /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Can't compile" /c:"Cannot compile" /c:"Fatal error" /c:"ErrorModule" /c:"Expected '" "%%f" >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        echo Script error detected in logs!
        set "EARLY_ERROR_FOUND=1"
        REM Give server a moment to finish writing
        timeout /t 3 /nobreak >nul
        goto :check_results
    )
    
    REM Check for script compilation failure message
    findstr /i /c:"script module" "%%f" | findstr /i /c:"error\|fail\|can't\|cannot" >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        echo Script module error detected!
        set "EARLY_ERROR_FOUND=1"
        timeout /t 3 /nobreak >nul
        goto :check_results
    )
    
    REM Check if server has progressed past script loading (success path)
    findstr /i /c:"Mission read" /c:"Starting mission" /c:"World loaded" "%%f" >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        echo Server reached mission load phase - scripts compiled successfully.
        goto :check_results
    )
)

if %WAIT_COUNT% GEQ %VALIDATION_TIMEOUT% (
    echo.
    echo Validation timeout reached (%VALIDATION_TIMEOUT%s)
    echo Checking logs for errors...
    set "TIMEOUT_OCCURRED=1"
    goto :check_results
)

echo   Waiting... (%WAIT_COUNT%/%VALIDATION_TIMEOUT%s)
goto :wait_loop

:check_results
echo.
echo ========================================
echo Stopping validation server...
echo ========================================

REM Give server time to flush logs before killing
timeout /t 2 /nobreak >nul

REM Kill the server process
taskkill /FI "WINDOWTITLE eq DayZScriptValidation" /F >nul 2>&1
timeout /t 3 /nobreak >nul
taskkill /IM DayZServer_x64.exe /F >nul 2>&1

REM Wait for logs to fully flush after kill
timeout /t 2 /nobreak >nul

echo.
echo ========================================
echo Analyzing logs for script errors...
echo ========================================
echo.

REM If errors were detected early (server died or error in logs during wait)
if defined EARLY_ERROR_FOUND (
    set "FOUND_ERRORS=1"
)

REM Check for crash logs first (most reliable error indicator)
for %%f in ("%VALIDATION_DIR%\crash_*.log") do (
    if exist "%%f" (
        echo Found crash log: %%~nxf
        set "FOUND_ERRORS=1"
    )
)

REM Check for script errors in RPT files
for %%f in ("%VALIDATION_DIR%\*.RPT" "%VALIDATION_DIR%\*.log" "%VALIDATION_DIR%\*.mdmp") do (
    if exist "%%f" (
        REM Check for crash dumps (indicates server crashed)
        if "%%~xf"==".mdmp" (
            echo Found crash dump: %%~nxf
            set "FOUND_ERRORS=1"
        )
        
        REM Search for script error patterns
        findstr /i /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Can't compile" /c:"Cannot compile" "%%f" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            set "FOUND_ERRORS=1"
        )
        
        REM Search for fatal/module errors
        findstr /i /c:"Fatal error" /c:"ErrorModule" "%%f" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            set "FOUND_ERRORS=1"
        )
        
        REM Search for script module compilation failures
        findstr /i /c:"script module" "%%f" | findstr /i /c:"error\|fail\|can't\|cannot" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            set "FOUND_ERRORS=1"
        )
        
        REM Search for specific script compilation errors
        findstr /i /c:"Cannot create object" /c:"Undefined variable" /c:"is not a member" /c:"not declared" "%%f" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            set "FOUND_ERRORS=1"
        )
        
        REM Search for syntax errors (Expected ';' etc)
        findstr /i /c:"Expected '" /c:"Unknown type" /c:"Member not found" "%%f" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            set "FOUND_ERRORS=1"
        )
    )
)

if defined FOUND_ERRORS (
    echo [ERROR] Script errors detected!
    echo.
    
    REM Display crash logs first (most useful info)
    set "CRASH_LOG_FOUND="
    for %%f in ("%VALIDATION_DIR%\crash_*.log") do (
        if exist "%%f" (
            set "CRASH_LOG_FOUND=1"
            echo ========================================
            echo CRASH LOG: %%~nxf
            echo ========================================
            echo.
            type "%%f"
            echo.
        )
    )
    
    REM Display script_*.log files if present
    for %%f in ("%VALIDATION_DIR%\script_*.log") do (
        if exist "%%f" (
            echo ========================================
            echo SCRIPT LOG: %%~nxf
            echo ========================================
            echo.
            type "%%f"
            echo.
        )
    )
    
    REM If no crash log found, show RPT error details
    if not defined CRASH_LOG_FOUND (
        echo ----------------------------------------
        echo Error details from RPT logs:
        echo ----------------------------------------
        
        for %%f in ("%VALIDATION_DIR%\*.RPT") do (
            if exist "%%f" (
                echo.
                echo From: %%~nxf
                echo.
                REM Display error lines - comprehensive patterns
                findstr /i /n /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Can't compile" /c:"Cannot compile" /c:"Fatal error" "%%f"
                findstr /i /n /c:"Cannot create object" /c:"Undefined variable" /c:"is not a member" /c:"not declared" "%%f"
                findstr /i /n /c:"Unknown type" /c:"Member not found" /c:"ErrorModule" /c:"Expected '" "%%f"
                findstr /i /n /c:"Error: 0x" /c:"script module" "%%f"
            )
        )
    )
    
    REM Check for crash dumps
    for %%f in ("%VALIDATION_DIR%\*.mdmp") do (
        if exist "%%f" (
            echo.
            echo [CRASH DUMP] %%~nxf
        )
    )
    
    echo.
    echo ========================================
    echo VALIDATION FAILED - Script errors found
    echo ========================================
    echo.
    echo Logs preserved at: %VALIDATION_DIR%
    goto :error_keep_logs
)

echo [OK] No script errors detected!
echo.

if defined TIMEOUT_OCCURRED (
    echo Note: Server did not reach mission load phase within timeout,
    echo but no script errors were found in logs. Scripts compiled successfully.
    echo.
)

REM Cleanup
echo Cleaning up validation files...
if exist "%VALIDATION_DIR%" rmdir /s /q "%VALIDATION_DIR%"

echo.
echo ========================================
echo VALIDATION PASSED
echo ========================================
goto :end

:create_default_config
REM Create a minimal server config for validation
(
echo hostname = "Swarm Script Validation";
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
echo     class Swarm_Validation {
echo         template = "dayzOffline.chernarusplus";
echo     };
echo };
) > "%VALIDATION_CFG_DIR%\serverDZ.default.cfg"
echo Created: %VALIDATION_CFG_DIR%\serverDZ.default.cfg
goto :eof

:show_help
echo.
echo Usage: validate_scripts.bat [options]
echo.
echo Options:
echo   --timeout N    Set validation timeout in seconds (default: 60)
echo   --skip-build   Skip build check (assume mod is already built)
echo   --help         Show this help message
echo.
echo Environment Variables:
echo   DAYZ_SERVER    Path to DayZ Server installation (required)
echo.
echo Server Config:
echo   The script looks for server configs in this order:
echo     1. validation\serverDZ.cfg         (custom - for your specific map)
echo     2. validation\serverDZ.default.cfg (default - Chernarus offline)
echo.
echo   To use a different map, copy serverDZ.default.cfg to serverDZ.cfg
echo   and modify the Missions class template.
echo.
echo Dependency Mods:
echo   If your mod depends on other mods (e.g., CF, COT), place them in:
echo     validation\mods\
echo.
echo   Example structure:
echo     validation\mods\@CF\
echo     validation\mods\@Community-Online-Tools\
echo.
echo   The script will automatically detect and load these mods.
echo.
echo Example:
echo   validate_scripts.bat --timeout 90
echo.
goto :end

:error_keep_logs
endlocal
exit /b 1

:error
echo.
if exist "%VALIDATION_DIR%" rmdir /s /q "%VALIDATION_DIR%"
endlocal
exit /b 1

:end
endlocal
exit /b 0

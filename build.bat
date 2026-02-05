@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo Building all Swarm packages...
echo ========================================
echo.

REM Load environment variables from .env file using PowerShell (only if not already set)
set "_envfile=%~dp0.env"
set "_psscript=%~dp0scripts\load_env.ps1"
set "_tmpenv=%TEMP%\_swarm_env.tmp"
if exist "!_envfile!" if exist "!_psscript!" (
    for %%V in (DAYZ_TOOLS DAYZ_SERVER WORKSHOP_ID) do (
        REM Only load from .env if variable is not already set
        call :load_env_var %%V
    )
)
goto :after_load_env

:load_env_var
if not defined %1 (
    powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName %1 > "!_tmpenv!" 2>nul
    if exist "!_tmpenv!" (
        set /p %1=<"!_tmpenv!"
        del "!_tmpenv!" >nul 2>&1
    )
)
goto :eof

:after_load_env

set ADDON_BUILDER=!DAYZ_TOOLS!\Bin\AddonBuilder\AddonBuilder.exe
set VERSION=
set OUTPUT_DIR=%~dp0dist\@Swarm\Addons
set TEMP_DIR=%~dp0.build_temp
set PREPROCESS_SCRIPT=%~dp0scripts\update_version.ps1
set RUN_VALIDATE=
set VALIDATE_TIMEOUT=60

REM Parse arguments
:parse_args
if "%~1"=="" goto :done_args
if /i "%~1"=="--version" (
    set "VERSION=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--validate" (
    set "RUN_VALIDATE=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--timeout" (
    set "VALIDATE_TIMEOUT=%~2"
    shift
    shift
    goto :parse_args
)
shift
goto :parse_args
:done_args

REM Read version from .version file if not specified
if "!VERSION!"=="" (
    if exist "%~dp0.version" (
        set /p VERSION=<"%~dp0.version"
    ) else (
        echo ERROR: No version specified and .version file not found!
        goto :error
    )
) else (
    REM Update .version file with new version
    echo !VERSION!>"%~dp0.version"
    echo Updated .version file to !VERSION!
)

echo Using version: !VERSION!
echo.

REM Prepare output directory
echo Preparing output directory...
if exist "%OUTPUT_DIR%" rmdir /s /q "%OUTPUT_DIR%"
mkdir "%OUTPUT_DIR%"
echo.

REM Count folders in src directory (excluding meta.cpp)
set FOLDER_COUNT=0
for /d %%D in ("%~dp0src\*") do set /a FOLDER_COUNT+=1

REM Build all packages in src directory
set BUILD_INDEX=0
set FIRST_BUILD=1
for /d %%D in ("%~dp0src\*") do (
    set /a BUILD_INDEX+=1
    set "FOLDER_NAME=%%~nxD"
    
    echo [!BUILD_INDEX!/!FOLDER_COUNT!] Building !FOLDER_NAME!...
    set "TEMP_SRC=%TEMP_DIR%\!FOLDER_NAME!"
    powershell -NoProfile -ExecutionPolicy Bypass -File "%PREPROCESS_SCRIPT%" -SourceDir "%%D" -TempDir "!TEMP_SRC!" -Version "!VERSION!"
    
    REM Use -clear flag only on first build
    if !FIRST_BUILD! EQU 1 (
        "%ADDON_BUILDER%" "!TEMP_SRC!" "%OUTPUT_DIR%" -clear -packonly
        set FIRST_BUILD=0
    ) else (
        "%ADDON_BUILDER%" "!TEMP_SRC!" "%OUTPUT_DIR%" -packonly
    )
    
    if !ERRORLEVEL! NEQ 0 (
        echo ERROR: !FOLDER_NAME! build failed!
        goto :error
    )
    echo !FOLDER_NAME! OK
    echo.
)

REM Process root meta.cpp
echo Copying mod metadata...
set "TEMP_META=%TEMP_DIR%\meta.cpp"
copy "%~dp0src\meta.cpp" "!TEMP_META!" >nul

REM Get current timestamp (.NET Framework ticks format)
for /f %%T in ('powershell -NoProfile -Command "[DateTime]::UtcNow.Ticks"') do set "TIMESTAMP=%%T"

REM Set publishedid from WORKSHOP_ID or default to 0
if defined WORKSHOP_ID (
    set "PUBLISHEDID=!WORKSHOP_ID!"
) else (
    set "PUBLISHEDID=0"
)

echo   Timestamp: !TIMESTAMP!
echo   Published ID: !PUBLISHEDID!

REM Replace all placeholders in meta.cpp
powershell -NoProfile -ExecutionPolicy Bypass -Command "$content = Get-Content '!TEMP_META!' -Raw; $content = $content -replace '%%VERSION%%', '!VERSION!'; $content = $content -replace '%%TIMESTAMP%%', '!TIMESTAMP!'; $content = $content -replace '%%PUBLISHEDID%%', '!PUBLISHEDID!'; Set-Content '!TEMP_META!' -Value $content -NoNewline"
copy "!TEMP_META!" "%~dp0dist\@Swarm\meta.cpp" >nul

REM Cleanup temp directory
echo Cleaning up...
if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"

echo ========================================
echo All packages built successfully!
echo Version: !VERSION!
echo Output: %OUTPUT_DIR%
echo ========================================

REM Run validate if --validate flag was specified
if defined RUN_VALIDATE (
    echo.
    echo Running script validate...
    echo.
    call "%~dp0validate.bat" --timeout !VALIDATE_TIMEOUT!
    if !ERRORLEVEL! NEQ 0 (
        goto :validate_failed
    )
)
goto :end

:validate_failed
echo.
echo ========================================
echo Build succeeded but validate FAILED!
echo ========================================
endlocal
exit /b 1

:error
echo ========================================
echo Build failed with errors!
echo ========================================
if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"
endlocal
exit /b 1

:end
endlocal
exit /b 0
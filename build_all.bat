@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo Building all Swarm packages...
echo ========================================
echo.

REM Load environment variables from .env file
if exist "%~dp0.env" (
    for /f "usebackq tokens=1,2 delims==" %%a in ("%~dp0.env") do set "%%a=%%b"
)

set ADDON_BUILDER=%DAYZ_TOOLS%\Bin\AddonBuilder\AddonBuilder.exe
set VERSION=
set OUTPUT_DIR=%~dp0dist\@Swarm\Addons
set TEMP_DIR=%~dp0.build_temp
set PREPROCESS_SCRIPT=%~dp0scripts\update_version.ps1
set RUN_VALIDATION=
set VALIDATION_TIMEOUT=60

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
    set "RUN_VALIDATION=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--timeout" (
    set "VALIDATION_TIMEOUT=%~2"
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

REM Build SwarmTweaks
echo [1/3] Building SwarmTweaks...
set "TEMP_SRC=%TEMP_DIR%\SwarmTweaks"
powershell -NoProfile -ExecutionPolicy Bypass -File "%PREPROCESS_SCRIPT%" -SourceDir "%~dp0src\SwarmTweaks" -TempDir "!TEMP_SRC!" -Version "!VERSION!"
"%ADDON_BUILDER%" "!TEMP_SRC!" "%OUTPUT_DIR%" -clear -packonly
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: SwarmTweaks build failed!
    goto :error
)
echo SwarmTweaks OK
echo.

REM Build SwarmAnimals
echo [2/3] Building SwarmAnimals...
set "TEMP_SRC=%TEMP_DIR%\SwarmAnimals"
powershell -NoProfile -ExecutionPolicy Bypass -File "%PREPROCESS_SCRIPT%" -SourceDir "%~dp0src\SwarmAnimals" -TempDir "!TEMP_SRC!" -Version "!VERSION!"
"%ADDON_BUILDER%" "!TEMP_SRC!" "%OUTPUT_DIR%" -packonly
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: SwarmAnimals build failed!
    goto :error
)
echo SwarmAnimals OK
echo.

REM Build SwarmEarplugs
echo [3/3] Building SwarmEarplugs...
set "TEMP_SRC=%TEMP_DIR%\SwarmEarplugs"
powershell -NoProfile -ExecutionPolicy Bypass -File "%PREPROCESS_SCRIPT%" -SourceDir "%~dp0src\SwarmEarplugs" -TempDir "!TEMP_SRC!" -Version "!VERSION!"
"%ADDON_BUILDER%" "!TEMP_SRC!" "%OUTPUT_DIR%" -packonly
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: SwarmEarplugs build failed!
    goto :error
)
echo SwarmEarplugs OK
echo.

REM Process root meta.cpp
echo Copying mod metadata...
set "TEMP_META=%TEMP_DIR%\meta.cpp"
copy "%~dp0src\meta.cpp" "!TEMP_META!" >nul
powershell -NoProfile -ExecutionPolicy Bypass -Command "(Get-Content '!TEMP_META!' -Raw) -replace '%%VERSION%%', '!VERSION!' | Set-Content '!TEMP_META!' -NoNewline"
copy "!TEMP_META!" "%~dp0dist\@Swarm\meta.cpp" >nul

REM Cleanup temp directory
echo Cleaning up...
if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"

echo ========================================
echo All packages built successfully!
echo Version: !VERSION!
echo Output: %OUTPUT_DIR%
echo ========================================

REM Run validation if --validate flag was specified
if defined RUN_VALIDATION (
    echo.
    echo Running script validation...
    echo.
    call "%~dp0validate_scripts.bat" --timeout !VALIDATION_TIMEOUT!
    if !ERRORLEVEL! NEQ 0 (
        goto :validation_failed
    )
)
goto :end

:validation_failed
echo.
echo ========================================
echo Build succeeded but validation FAILED!
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

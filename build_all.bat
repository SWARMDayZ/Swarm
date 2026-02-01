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

REM Parse arguments
:parse_args
if "%~1"=="" goto :done_args
if /i "%~1"=="--version" (
    set "VERSION=%~2"
    shift
    shift
    goto :parse_args
)
shift
goto :parse_args
:done_args

REM Update versions if specified
if not "!VERSION!"=="" (
    echo Updating all packages to version !VERSION!...
    echo.
    
    set "UPDATE_SCRIPT=%~dp0scripts\update_version.ps1"
    
    REM Update root meta.cpp
    powershell -NoProfile -ExecutionPolicy Bypass -File "!UPDATE_SCRIPT!" -FilePath "%~dp0src\meta.cpp" -Version "!VERSION!"
    
    REM Update each package
    for %%P in (SwarmTweaks SwarmAnimals SwarmEarplugs) do (
        echo   Processing %%P...
        powershell -NoProfile -ExecutionPolicy Bypass -File "!UPDATE_SCRIPT!" -FilePath "%~dp0src\%%P\meta.cpp" -Version "!VERSION!"
        powershell -NoProfile -ExecutionPolicy Bypass -File "!UPDATE_SCRIPT!" -FilePath "%~dp0src\%%P\config.cpp" -Version "!VERSION!"
    )
    echo.
)

REM Prepare output directory
echo Preparing output directory...
if exist "%OUTPUT_DIR%" rmdir /s /q "%OUTPUT_DIR%"
mkdir "%OUTPUT_DIR%"
echo.

REM Build SwarmTweaks
echo [1/3] Building SwarmTweaks...
"%ADDON_BUILDER%" "%~dp0src\SwarmTweaks" "%OUTPUT_DIR%" -clear -packonly
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: SwarmTweaks build failed!
    goto :error
)
echo SwarmTweaks OK
echo.

REM Build SwarmAnimals
echo [2/3] Building SwarmAnimals...
"%ADDON_BUILDER%" "%~dp0src\SwarmAnimals" "%OUTPUT_DIR%" -packonly
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: SwarmAnimals build failed!
    goto :error
)
echo SwarmAnimals OK
echo.

REM Build SwarmEarplugs
echo [3/3] Building SwarmEarplugs...
"%ADDON_BUILDER%" "%~dp0src\SwarmEarplugs" "%OUTPUT_DIR%" -packonly
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: SwarmEarplugs build failed!
    goto :error
)
echo SwarmEarplugs OK
echo.

REM Copy meta.cpp to mod root
echo Copying mod metadata...
copy "%~dp0src\meta.cpp" "%~dp0dist\@Swarm\meta.cpp" >nul

echo ========================================
echo All packages built successfully!
if not "%VERSION%"=="" echo Version: %VERSION%
echo Output: %OUTPUT_DIR%
echo ========================================
goto :end

:error
echo ========================================
echo Build failed with errors!
echo ========================================
goto :end

:end
endlocal
pause

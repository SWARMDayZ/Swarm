@echo off
setlocal EnableDelayedExpansion

REM Load environment variables from .env file
if exist "%~dp0.env" (
    for /f "usebackq tokens=1,2 delims==" %%a in ("%~dp0.env") do set "%%a=%%b"
)

set ADDON_BUILDER=%DAYZ_TOOLS%\Bin\AddonBuilder\AddonBuilder.exe
set SOURCE_DIR=%~dp0src\SwarmAnimals
set OUTPUT_DIR=%~dp0dist\@SwarmAnimals\Addons
set TEMP_DIR=%~dp0.build_temp\SwarmAnimals
set PREPROCESS_SCRIPT=%~dp0scripts\update_version.ps1
set VERSION=

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

REM Read version from .version file if not specified
if "!VERSION!"=="" (
    if exist "%~dp0.version" (
        set /p VERSION=<"%~dp0.version"
    ) else (
        echo ERROR: No version specified and .version file not found!
        goto :end
    )
) else (
    REM Update .version file with new version
    echo !VERSION!>"%~dp0.version"
    echo Updated .version file to !VERSION!
)

echo Building SwarmAnimals...
echo Source: %SOURCE_DIR%
echo Output: %OUTPUT_DIR%
echo Version: !VERSION!

if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM Preprocess source with version
powershell -NoProfile -ExecutionPolicy Bypass -File "%PREPROCESS_SCRIPT%" -SourceDir "%SOURCE_DIR%" -TempDir "%TEMP_DIR%" -Version "!VERSION!"

REM Build from temp directory
"%ADDON_BUILDER%" "%TEMP_DIR%" "%OUTPUT_DIR%" -clear -packonly

if %ERRORLEVEL% EQU 0 (
    echo SwarmAnimals built successfully!
) else (
    echo SwarmAnimals build failed with error code %ERRORLEVEL%
)

REM Cleanup temp directory
if exist "%~dp0.build_temp" rmdir /s /q "%~dp0.build_temp"

:end
endlocal

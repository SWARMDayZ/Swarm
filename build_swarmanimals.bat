@echo off
setlocal EnableDelayedExpansion

REM Load environment variables from .env file
if exist "%~dp0.env" (
    for /f "usebackq tokens=1,2 delims==" %%a in ("%~dp0.env") do set "%%a=%%b"
)

set ADDON_BUILDER=%DAYZ_TOOLS%\Bin\AddonBuilder\AddonBuilder.exe
set SOURCE_DIR=%~dp0src\SwarmAnimals
set OUTPUT_DIR=%~dp0dist\@SwarmAnimals\Addons
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

REM Update version if specified
if not "!VERSION!"=="" (
    echo Updating SwarmAnimals version to !VERSION!...
    set "UPDATE_SCRIPT=%~dp0scripts\update_version.ps1"
    powershell -NoProfile -ExecutionPolicy Bypass -File "!UPDATE_SCRIPT!" -FilePath "!SOURCE_DIR!\meta.cpp" -Version "!VERSION!"
    powershell -NoProfile -ExecutionPolicy Bypass -File "!UPDATE_SCRIPT!" -FilePath "!SOURCE_DIR!\config.cpp" -Version "!VERSION!"
)

echo Building SwarmAnimals...
echo Source: %SOURCE_DIR%
echo Output: %OUTPUT_DIR%

if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

"%ADDON_BUILDER%" "%SOURCE_DIR%" "%OUTPUT_DIR%" -clear -packonly

if %ERRORLEVEL% EQU 0 (
    echo SwarmAnimals built successfully!
) else (
    echo SwarmAnimals build failed with error code %ERRORLEVEL%
)

endlocal

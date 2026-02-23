@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo Swarm Mod Publisher
echo ========================================
echo.

REM ============================================================
REM Load environment variables from .env
REM ============================================================

set "_envfile=%~dp0.env"
set "_psscript=%~dp0scripts\load_env.ps1"
set "_tmpenv=%TEMP%\_swarm_env.tmp"

if exist "!_envfile!" if exist "!_psscript!" (
    for %%V in (DAYZ_TOOLS DAYZ_SERVER STEAM_USERNAME WORKSHOP_ID) do (
        if not defined %%V (
            powershell -NoProfile -ExecutionPolicy Bypass -File "!_psscript!" -EnvFile "!_envfile!" -VarName %%V > "!_tmpenv!" 2>nul
            if exist "!_tmpenv!" (
                set /p %%V=<"!_tmpenv!"
                del "!_tmpenv!" >nul 2>&1
            )
        )
    )
)

REM Check for DayZ Tools
if not defined DAYZ_TOOLS (
    echo ERROR: DAYZ_TOOLS environment variable not set!
    echo Please set it in your .env file.
    goto :error
)

set "ADDON_BUILDER=!DAYZ_TOOLS!\Bin\AddonBuilder\AddonBuilder.exe"
set "DS_SIGN_FILE=!DAYZ_TOOLS!\Bin\DsUtils\DSSignFile.exe"
set "DS_CREATE_KEY=!DAYZ_TOOLS!\Bin\DsUtils\DSCreateKey.exe"
set "PREPROCESS_SCRIPT=%~dp0scripts\update_version.ps1"
set "TEMP_DIR=%~dp0.build_temp"
set "KEYS_DIR=%~dp0keys"
set "KEY_NAME=Swarm"

REM ============================================================
REM Discover packages and their versions
REM ============================================================

set PACKAGE_COUNT=0
set "PACKAGE_LIST="
set "VERSION_LIST="

for /d %%D in ("%~dp0packages\*") do (
    set /a PACKAGE_COUNT+=1
    set "PKG_NAME=%%~nxD"

    REM Read version (package-specific or root)
    set "PKG_VER="
    if exist "%%D\.version" (
        set /p PKG_VER=<"%%D\.version"
    ) else if exist "%~dp0.version" (
        set /p PKG_VER=<"%~dp0.version"
    ) else (
        set "PKG_VER=0.0.1"
    )

    if "!PACKAGE_LIST!"=="" (
        set "PACKAGE_LIST=!PKG_NAME!"
        set "VERSION_LIST=!PKG_VER!"
    ) else (
        set "PACKAGE_LIST=!PACKAGE_LIST!,!PKG_NAME!"
        set "VERSION_LIST=!VERSION_LIST!,!PKG_VER!"
    )
)

if !PACKAGE_COUNT! EQU 0 (
    echo ERROR: No packages found in packages\ directory!
    goto :error
)

REM ============================================================
REM Interactive selection (via PowerShell)
REM ============================================================

set "MENU_OUTPUT=%TEMP%\_swarm_publish_menu.tmp"

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\publish_prompt.ps1" ^
    -Packages "!PACKAGE_LIST!" ^
    -Versions "!VERSION_LIST!" ^
    -OutputFile "!MENU_OUTPUT!"

if !ERRORLEVEL! NEQ 0 (
    echo.
    del "!MENU_OUTPUT!" >nul 2>&1
    goto :end
)

if not exist "!MENU_OUTPUT!" (
    echo No packages selected!
    goto :error
)

REM ============================================================
REM Process each selected package
REM ============================================================

echo.
echo ========================================
echo Publish Configuration
echo ========================================

REM Display selections
for /f "usebackq tokens=1,* delims==" %%A in ("!MENU_OUTPUT!") do (
    echo   %%A = %%B
)
echo ========================================
echo.

REM Process each package line from the menu output
for /f "usebackq tokens=1,* delims==" %%A in ("!MENU_OUTPUT!") do (
    set "PUB_PKG=%%A"
    set "PUB_VERSION=%%B"
    call :publish_package "!PUB_PKG!" "!PUB_VERSION!"
    if !ERRORLEVEL! NEQ 0 (
        echo.
        echo ERROR: Publishing !PUB_PKG! failed!
        echo.
    )
)

del "!MENU_OUTPUT!" >nul 2>&1

echo.
echo ========================================
echo Publish Complete
echo ========================================
echo.
goto :end

REM ============================================================
REM SUBROUTINE: Publish a single package
REM ============================================================
:publish_package
set "PKG=%~1"
set "VERSION=%~2"

echo ########################################
echo Publishing: !PKG! v!VERSION!
echo ########################################
echo.

set "PKG_DIR=%~dp0packages\!PKG!"
set "MOD_DIR=%~dp0dist\@!PKG!"
set "OUTPUT_DIR=!MOD_DIR!\Addons"

REM ----------------------------------------
REM Step 1: Update version file
REM ----------------------------------------

echo [Step 1/5] Updating version...
echo !VERSION!> "!PKG_DIR!\.version"
echo   Saved version !VERSION! to !PKG_DIR!\.version
echo.

REM ----------------------------------------
REM Step 2: Build
REM ----------------------------------------

echo [Step 2/5] Building !PKG!...
echo.

if exist "!OUTPUT_DIR!" rmdir /s /q "!OUTPUT_DIR!"
mkdir "!OUTPUT_DIR!"

REM Count and build subfolders
set ADDON_COUNT=0
set ADDON_INDEX=0
for /d %%D in ("!PKG_DIR!\*") do set /a ADDON_COUNT+=1

if !ADDON_COUNT! EQU 0 (
    echo   WARNING: No addon folders found in !PKG_DIR!
) else (
    for /d %%D in ("!PKG_DIR!\*") do (
        set /a ADDON_INDEX+=1
        set "ADDON_NAME=%%~nxD"
        echo   [!ADDON_INDEX!/!ADDON_COUNT!] Building !ADDON_NAME!...

        set "TEMP_SRC=!TEMP_DIR!\!ADDON_NAME!"
        powershell -NoProfile -ExecutionPolicy Bypass -File "!PREPROCESS_SCRIPT!" -SourceDir "%%D" -TempDir "!TEMP_SRC!" -Version "!VERSION!"

        "!ADDON_BUILDER!" "!TEMP_SRC!" "!OUTPUT_DIR!" -clear -packonly

        if !ERRORLEVEL! NEQ 0 (
            echo   ERROR: !ADDON_NAME! build failed!
            exit /b 1
        )
        echo   !ADDON_NAME! OK
    )
)

REM Process meta.cpp
if exist "!PKG_DIR!\meta.cpp" (
    echo.
    echo   Processing meta.cpp...
    if not exist "!TEMP_DIR!" mkdir "!TEMP_DIR!"
    set "TEMP_META=!TEMP_DIR!\meta_!PKG!.cpp"
    copy "!PKG_DIR!\meta.cpp" "!TEMP_META!" >nul

    for /f %%T in ('powershell -NoProfile -Command "[DateTime]::UtcNow.Ticks"') do set "TIMESTAMP=%%T"

    REM Try to get package-specific workshop ID, fall back to global, then 0
    set "PUBLISHEDID=0"
    if defined WORKSHOP_ID set "PUBLISHEDID=!WORKSHOP_ID!"

    REM Check for package-specific workshop ID file
    if exist "!PKG_DIR!\.workshop_id" (
        set /p PUBLISHEDID=<"!PKG_DIR!\.workshop_id"
    )

    powershell -NoProfile -ExecutionPolicy Bypass -Command "$c = Get-Content '!TEMP_META!' -Raw; $c = $c -replace '%%VERSION%%', '!VERSION!'; $c = $c -replace '%%TIMESTAMP%%', '!TIMESTAMP!'; $c = $c -replace '%%PUBLISHEDID%%', '!PUBLISHEDID!'; Set-Content '!TEMP_META!' -Value $c -NoNewline"
    copy "!TEMP_META!" "!MOD_DIR!\meta.cpp" >nul
)

REM Cleanup temp
if exist "!TEMP_DIR!" rmdir /s /q "!TEMP_DIR!"

echo.
echo   Build complete.
echo.

REM ----------------------------------------
REM Step 3: Validate
REM ----------------------------------------

echo [Step 3/5] Validating scripts...

if not defined DAYZ_SERVER (
    echo   WARNING: DAYZ_SERVER not set, skipping validation.
    echo.
    goto :pub_sign
)

if not exist "!DAYZ_SERVER!\DayZServer_x64.exe" (
    echo   WARNING: DayZ Server not found, skipping validation.
    echo.
    goto :pub_sign
)

REM Check if PBOs exist to validate
set "PBO_CHECK=0"
for %%F in ("!OUTPUT_DIR!\*.pbo") do set /a PBO_CHECK+=1

if !PBO_CHECK! EQU 0 (
    echo   No PBOs to validate.
    echo.
    goto :pub_sign
)

set "VALIDATE_DIR=%~dp0.validate_temp"
set "VALIDATE_CFG_DIR=%~dp0validate"
set "VALIDATE_PORT=2399"
set "VALIDATE_TIMEOUT=15"

set "VALIDATE_CFG="
if exist "!VALIDATE_CFG_DIR!\serverDZ.cfg" (
    set "VALIDATE_CFG=!VALIDATE_CFG_DIR!\serverDZ.cfg"
) else if exist "!VALIDATE_CFG_DIR!\serverDZ.default.cfg" (
    set "VALIDATE_CFG=!VALIDATE_CFG_DIR!\serverDZ.default.cfg"
) else (
    echo   WARNING: No validate config found, skipping validation.
    echo.
    goto :pub_sign
)

if exist "!VALIDATE_DIR!" rmdir /s /q "!VALIDATE_DIR!"
mkdir "!VALIDATE_DIR!"
copy "!VALIDATE_CFG!" "!VALIDATE_DIR!\serverDZ.cfg" >nul

REM Build mod list for validation
set "VAL_MOD_LIST=!MOD_DIR!"
set "VALIDATE_MODS_DIR=%~dp0validate\mods"
if defined MODS (
    set "VAL_MOD_LIST="
    for %%m in ("!MODS:;=" "!") do (
        set "MOD_NAME=%%~m"
        set "MOD_NAME=!MOD_NAME:"=!"
        if exist "!VALIDATE_MODS_DIR!\!MOD_NAME!" (
            if "!VAL_MOD_LIST!"=="" (
                set "VAL_MOD_LIST=!VALIDATE_MODS_DIR!\!MOD_NAME!"
            ) else (
                set "VAL_MOD_LIST=!VAL_MOD_LIST!;!VALIDATE_MODS_DIR!\!MOD_NAME!"
            )
        )
    )
    set "VAL_MOD_LIST=!VAL_MOD_LIST!;!MOD_DIR!"
) else (
    if exist "!VALIDATE_MODS_DIR!" (
        set "VAL_MOD_LIST="
        for /d %%d in ("!VALIDATE_MODS_DIR!\@*") do (
            if "!VAL_MOD_LIST!"=="" (
                set "VAL_MOD_LIST=%%d"
            ) else (
                set "VAL_MOD_LIST=!VAL_MOD_LIST!;%%d"
            )
        )
        set "VAL_MOD_LIST=!VAL_MOD_LIST!;!MOD_DIR!"
    )
)

echo   Starting validation server...
start "DayZScriptValidate" /B /MIN "!DAYZ_SERVER!\DayZServer_x64.exe" ^
    "-mod=!VAL_MOD_LIST!" ^
    -config=serverDZ.cfg ^
    -port=!VALIDATE_PORT! ^
    "-profiles=!VALIDATE_DIR!" ^
    -doLogs ^
    -adminLog

timeout /t !VALIDATE_TIMEOUT! /nobreak >nul
timeout /t 2 /nobreak >nul
taskkill /FI "WINDOWTITLE eq DayZScriptValidate" /F >nul 2>&1
timeout /t 3 /nobreak >nul
taskkill /IM DayZServer_x64.exe /F >nul 2>&1
timeout /t 2 /nobreak >nul

REM Check for errors
set "VAL_ERRORS="
for %%f in ("!VALIDATE_DIR!\crash_*.log") do if exist "%%f" set "VAL_ERRORS=1"
for %%f in ("!VALIDATE_DIR!\*.mdmp") do if exist "%%f" set "VAL_ERRORS=1"
for %%f in ("!VALIDATE_DIR!\*.RPT") do (
    if exist "%%f" (
        findstr /i /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Can't compile" /c:"Fatal error" /c:"ErrorModule" "%%f" >nul 2>&1
        if !ERRORLEVEL! EQU 0 set "VAL_ERRORS=1"
    )
)

if defined VAL_ERRORS (
    echo   [ERROR] Script errors detected!
    for %%f in ("!VALIDATE_DIR!\*.RPT") do (
        if exist "%%f" (
            findstr /i /n /c:"SCRIPT ERROR" /c:"SCRIPT  (E)" /c:"Compile error" /c:"Fatal error" /c:"ErrorModule" "%%f"
        )
    )
    echo.
    echo   VALIDATION FAILED - Fix errors before publishing.
    echo   Logs: !VALIDATE_DIR!
    exit /b 1
)

echo   [OK] Validation passed.
if exist "!VALIDATE_DIR!" rmdir /s /q "!VALIDATE_DIR!"
echo.

REM ----------------------------------------
REM Step 4: Sign
REM ----------------------------------------
:pub_sign
echo [Step 4/5] Signing PBOs...

if not exist "!DS_SIGN_FILE!" (
    echo   WARNING: DSSignFile.exe not found, skipping signing.
    echo.
    goto :pub_upload
)

REM Create keys if needed
set "PRIVATE_KEY=!KEYS_DIR!\!KEY_NAME!.biprivatekey"
set "PUBLIC_KEY=!KEYS_DIR!\!KEY_NAME!.bikey"

if not exist "!PRIVATE_KEY!" (
    echo   Creating new key pair: !KEY_NAME!
    if not exist "!KEYS_DIR!" mkdir "!KEYS_DIR!"
    pushd "!KEYS_DIR!"
    "!DS_CREATE_KEY!" !KEY_NAME!
    popd
    if not exist "!PRIVATE_KEY!" (
        echo   ERROR: Failed to create key pair!
        echo.
        goto :pub_upload
    )
    echo   Key pair created.
)

REM Create Keys folder in mod and copy public key
set "MOD_KEYS=!MOD_DIR!\Keys"
if not exist "!MOD_KEYS!" mkdir "!MOD_KEYS!"
copy "!PUBLIC_KEY!" "!MOD_KEYS!\" >nul 2>&1

REM Sign PBOs
set SIGN_COUNT=0
for %%F in ("!OUTPUT_DIR!\*.pbo") do (
    echo   Signing: %%~nxF
    del "!OUTPUT_DIR!\%%~nF.*.bisign" 2>nul
    "!DS_SIGN_FILE!" "!PRIVATE_KEY!" "%%F"
    set /a SIGN_COUNT+=1
)

echo   Signed !SIGN_COUNT! PBO files.
echo.

REM ----------------------------------------
REM Step 5: Publish to Workshop
REM ----------------------------------------
:pub_upload
echo [Step 5/5] Publishing to Steam Workshop...

REM Get workshop ID for this package
set "PUB_WORKSHOP_ID="
if exist "!PKG_DIR!\.workshop_id" (
    set /p PUB_WORKSHOP_ID=<"!PKG_DIR!\.workshop_id"
)
if "!PUB_WORKSHOP_ID!"=="" (
    if defined WORKSHOP_ID (
        set "PUB_WORKSHOP_ID=!WORKSHOP_ID!"
    )
)

if "!PUB_WORKSHOP_ID!"=="" (
    echo   WARNING: No Workshop ID found for !PKG!
    echo.
    echo   For first-time publishing:
    echo     1. Open DayZ Tools Publisher GUI
    echo     2. Create your mod item manually
    echo     3. Save the Workshop ID to: !PKG_DIR!\.workshop_id
    echo     4. Or add WORKSHOP_ID=XXXXXXXXXX to your .env file
    echo.
    echo   Mod is built and signed at: !MOD_DIR!
    echo.
    exit /b 0
)

echo   Workshop ID: !PUB_WORKSHOP_ID!

REM Create/update mod.cpp
(
    echo name = "!PKG!";
    echo picture = "";
    echo actionName = "";
    echo action = "";
    echo logo = "";
    echo logoSmall = "";
    echo logoOver = "";
    echo tooltip = "!PKG! - DayZ Mod";
    echo overview = "";
    echo author = "";
    echo version = "!VERSION!";
) > "!MOD_DIR!\mod.cpp"

REM Check for SteamCMD
set "STEAMCMD=%~dp0scripts\steamcmd.exe"
if not exist "!STEAMCMD!" (
    echo   ERROR: steamcmd.exe not found at: !STEAMCMD!
    echo   Please download SteamCMD and place it in the scripts folder.
    echo.
    echo   Mod is built and signed at: !MOD_DIR!
    exit /b 0
)

if not defined STEAM_USERNAME (
    echo   ERROR: STEAM_USERNAME not set in .env file.
    echo   Mod is built and signed at: !MOD_DIR!
    exit /b 0
)

echo   Steam user: !STEAM_USERNAME!

REM Set changelog
set "CHANGELOG=Version !VERSION!"

REM Create VDF file
set "VDF_FILE=%~dp0workshop_upload_!PKG!.vdf"
(
    echo "workshopitem"
    echo {
    echo     "appid" "221100"
    echo     "publishedfileid" "!PUB_WORKSHOP_ID!"
    echo     "contentfolder" "!MOD_DIR!"
    echo     "changenote" "!CHANGELOG!"
    echo }
) > "!VDF_FILE!"

echo.
echo   Uploading to Steam Workshop...
echo.

"!STEAMCMD!" +login "!STEAM_USERNAME!" +workshop_build_item "!VDF_FILE!" +quit
set "PUB_RESULT=!ERRORLEVEL!"

del "!VDF_FILE!" 2>nul

if !PUB_RESULT! EQU 0 (
    echo.
    echo   SUCCESS: !PKG! v!VERSION! published!
    echo   View: https://steamcommunity.com/sharedfiles/filedetails/?id=!PUB_WORKSHOP_ID!
) else (
    echo.
    echo   WARNING: SteamCMD returned error code !PUB_RESULT!
    echo   You may need to run SteamCMD manually to cache credentials.
)

echo.
echo   Summary:
echo     Package:    !PKG!
echo     Version:    !VERSION!
echo     Mod folder: !MOD_DIR!
echo     Workshop:   https://steamcommunity.com/sharedfiles/filedetails/?id=!PUB_WORKSHOP_ID!
if defined SIGN_COUNT echo     PBOs signed: !SIGN_COUNT!
echo.
exit /b !PUB_RESULT!

REM ============================================================
REM Error / End handlers
REM ============================================================

:error
echo.
endlocal
exit /b 1

:end
endlocal
exit /b 0

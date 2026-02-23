# Interactive prompts for publish_v2.bat
# Handles package selection and per-package version input
param(
    [Parameter(Mandatory)][string]$Packages,
    [Parameter(Mandatory)][string]$Versions,
    [Parameter(Mandatory)][string]$OutputFile
)

function Get-ConsoleWidth {
    try { return [Math]::Max(60, [Console]::BufferWidth - 1) }
    catch { return 79 }
}

function Show-MultiSelect {
    param(
        [string[]]$Labels,
        [string[]]$Values = @(),
        [bool[]]$Defaults = @()
    )

    if ($Values.Count -eq 0) { $Values = $Labels }
    $count = $Labels.Count
    if ($count -eq 0) { return @() }

    $selected = [bool[]]::new($count)
    for ($i = 0; $i -lt [Math]::Min($Defaults.Count, $count); $i++) {
        $selected[$i] = [bool]$Defaults[$i]
    }

    $current = 0
    $savedVisible = [Console]::CursorVisible
    [Console]::CursorVisible = $false

    # Reserve space for the menu to handle bottom-of-buffer scrolling
    $totalLines = $count + 2
    for ($i = 0; $i -lt $totalLines; $i++) { Write-Host "" }
    $startTop = [Console]::CursorTop - $totalLines

    try {
        $done = $false
        while (-not $done) {
            [Console]::SetCursorPosition(0, $startTop)
            $w = Get-ConsoleWidth

            for ($i = 0; $i -lt $count; $i++) {
                $check = if ($selected[$i]) { "*" } else { " " }
                $pointer = if ($i -eq $current) { " <" } else { "  " }
                Write-Host ("($check) $($Labels[$i])$pointer".PadRight($w))
            }
            Write-Host ("".PadRight($w))
            Write-Host ("(use space to toggle, enter to validate)".PadRight($w)) -ForegroundColor DarkGray

            $key = [Console]::ReadKey($true)
            switch ($key.Key) {
                "UpArrow"   { if ($current -gt 0) { $current-- } }
                "DownArrow" { if ($current -lt $count - 1) { $current++ } }
                "Spacebar"  { $selected[$current] = -not $selected[$current] }
                "Enter"     { $done = $true }
            }
        }

        # Final clean render (no pointer)
        [Console]::SetCursorPosition(0, $startTop)
        $w = Get-ConsoleWidth
        for ($i = 0; $i -lt $count; $i++) {
            $check = if ($selected[$i]) { "*" } else { " " }
            Write-Host ("($check) $($Labels[$i])".PadRight($w))
        }
        Write-Host ("".PadRight($w))
        Write-Host ("".PadRight($w))
    }
    finally {
        [Console]::CursorVisible = $savedVisible
    }

    $result = @()
    for ($i = 0; $i -lt $count; $i++) {
        if ($selected[$i]) { $result += $Values[$i] }
    }
    return $result
}

# ============================================================
# Main Flow
# ============================================================

$packageList = $Packages -split ","
$versionList = $Versions -split ","

# Step 1: Package selection
Write-Host "What mod do you want to publish?"
Write-Host ""

$pkgLabels = $packageList | ForEach-Object { "packages/$_" }
$pkgDefaults = [bool[]]::new($packageList.Count)

$selPkgs = @(Show-MultiSelect -Labels $pkgLabels -Values $packageList -Defaults $pkgDefaults)

if ($selPkgs.Count -eq 0) {
    Write-Host "No packages selected!" -ForegroundColor Red
    exit 1
}

# Step 2: Version input per selected package
Write-Host "----------"
Write-Host ""

$results = @()

foreach ($pkg in $selPkgs) {
    $pkgIndex = [Array]::IndexOf($packageList, $pkg)
    $defaultVersion = if ($pkgIndex -ge 0 -and $pkgIndex -lt $versionList.Count) { $versionList[$pkgIndex] } else { "0.0.1" }

    Write-Host "What version for " -NoNewline
    Write-Host "packages/$pkg" -ForegroundColor Cyan -NoNewline
    Write-Host " ?"

    Write-Host "version " -NoNewline
    Write-Host "[$defaultVersion]" -ForegroundColor DarkGray -NoNewline
    Write-Host ": " -NoNewline

    $userVersion = (Read-Host).Trim()
    if ([string]::IsNullOrWhiteSpace($userVersion)) {
        $userVersion = $defaultVersion
    }

    $results += "$pkg=$userVersion"
    Write-Host ""
}

# Write results (one PACKAGE=VERSION per line)
$results | Set-Content -Path $OutputFile -Encoding ASCII
exit 0

# Interactive prompts for build_v2.bat
# Handles settings reuse, package selection, and parameter selection
param(
    [Parameter(Mandatory)][string]$Packages,
    [string]$SavedPackages = "",
    [string]$SavedParams = "",
    [Parameter(Mandatory)][string]$OutputFile
)

function Get-ConsoleWidth {
    try { return [Math]::Max(60, [Console]::BufferWidth - 1) }
    catch { return 79 }
}

function Clear-Section {
    param([int]$FromLine)
    [Console]::SetCursorPosition(0, $FromLine)
    # ESC[0J = clear from cursor to end of display
    [Console]::Write("$([char]27)[0J")
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
            if ($key.Key -eq 'C' -and $key.Modifiers -band [ConsoleModifiers]::Control) {
                [Console]::CursorVisible = $savedVisible
                Write-Host ""
                exit 130
            }
            switch ($key.Key) {
                "UpArrow" { if ($current -gt 0) { $current-- } }
                "DownArrow" { if ($current -lt $count - 1) { $current++ } }
                "Spacebar" { $selected[$current] = -not $selected[$current] }
                "Enter" { $done = $true }
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
$hasSaved = (-not [string]::IsNullOrWhiteSpace($SavedPackages)) -and (-not [string]::IsNullOrWhiteSpace($SavedParams))

# Step 1: If saved settings exist, ask to reuse
$sectionStart = [Console]::CursorTop
if ($hasSaved) {
    $displayPkgs = $SavedPackages -replace ";", ", "
    $displayParams = $SavedParams -replace ";", ", "

    Write-Host "Do you want to build with latest settings?"
    Write-Host ""
    Write-Host "  Packages: $displayPkgs" -ForegroundColor Cyan
    Write-Host "  Params:   $displayParams" -ForegroundColor Cyan
    Write-Host ""

    Write-Host "(y/n) > " -NoNewline
    $response = ""
    while ($response -ne "y" -and $response -ne "n") {
        $key = [Console]::ReadKey($true)
        if ($key.Key -eq 'C' -and $key.Modifiers -band [ConsoleModifiers]::Control) {
            Write-Host ""
            exit 130
        }
        $response = $key.KeyChar.ToString().ToLower()
    }
    Write-Host $response
    Write-Host ""

    if ($response -eq "y") {
        @("PACKAGES=$SavedPackages", "PARAMS=$SavedParams") | Set-Content -Path $OutputFile -Encoding ASCII
        exit 0
    }
}

# Step 2: Package selection
Clear-Section $sectionStart
$sectionStart = [Console]::CursorTop
Write-Host "What mod do you want to build?"
Write-Host ""

$pkgLabels = $packageList | ForEach-Object { "packages/$_" }
$pkgDefaults = [bool[]]::new($packageList.Count)

if ($hasSaved) {
    $savedList = $SavedPackages -split ";"
    for ($i = 0; $i -lt $packageList.Count; $i++) {
        $pkgDefaults[$i] = $savedList -contains $packageList[$i]
    }
}
elseif ($packageList.Count -gt 0) {
    $pkgDefaults[0] = $true
}

$selPkgs = @(Show-MultiSelect -Labels $pkgLabels -Values $packageList -Defaults $pkgDefaults)

if ($selPkgs.Count -eq 0) {
    Write-Host "No packages selected!" -ForegroundColor Red
    exit 1
}

$selectedPackages = $selPkgs -join ";"

# Step 3: Params selection
Clear-Section $sectionStart
Write-Host "What should we do with the build?"
Write-Host ""

$paramLabels = @(
    "validate using server validation",
    "sign the pbos in the mods",
    "launch testing server & client"
)
$paramValues = @("validate", "sign", "launch")
$paramDefaults = [bool[]]::new(3)

if ($hasSaved) {
    $savedParamList = $SavedParams -split ";"
    for ($i = 0; $i -lt $paramValues.Count; $i++) {
        $paramDefaults[$i] = $savedParamList -contains $paramValues[$i]
    }
}
else {
    $paramDefaults = @($true, $true, $true)
}

$selParams = @(Show-MultiSelect -Labels $paramLabels -Values $paramValues -Defaults $paramDefaults)
$selectedParams = $selParams -join ";"

# Write results
@("PACKAGES=$selectedPackages", "PARAMS=$selectedParams") | Set-Content -Path $OutputFile -Encoding ASCII
exit 0

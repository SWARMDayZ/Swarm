param(
    [Parameter(Mandatory=$true)]
    [string]$SourceDir,
    
    [Parameter(Mandatory=$true)]
    [string]$TempDir,
    
    [Parameter(Mandatory=$true)]
    [string]$Version
)

# Copy source to temp directory
if (Test-Path $TempDir) {
    Remove-Item -Path $TempDir -Recurse -Force
}
Copy-Item -Path $SourceDir -Destination $TempDir -Recurse

# Replace %VERSION% placeholder in all .cpp files
Get-ChildItem -Path $TempDir -Filter "*.cpp" -Recurse | ForEach-Object {
    $content = Get-Content -Path $_.FullName -Raw
    if ($content -match '%VERSION%') {
        $newContent = $content -replace '%VERSION%', $Version
        [System.IO.File]::WriteAllText($_.FullName, $newContent)
        Write-Host "  Preprocessed: $($_.Name)"
    }
}

Write-Host "  Version: $Version"

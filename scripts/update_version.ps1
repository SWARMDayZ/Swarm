param(
    [Parameter(Mandatory=$true)]
    [string]$FilePath,
    
    [Parameter(Mandatory=$true)]
    [string]$Version
)

if (-not (Test-Path $FilePath)) {
    Write-Error "File not found: $FilePath"
    exit 1
}

$content = Get-Content -Path $FilePath -Raw
$newContent = $content -replace 'version\s*=\s*"[^"]*"', "version = `"$Version`""

if ($content -ne $newContent) {
    [System.IO.File]::WriteAllText($FilePath, $newContent)
    Write-Host "  Updated: $FilePath"
} else {
    Write-Host "  No change: $FilePath"
}

# Load environment variables from .env file
# Usage: load_env.ps1 [-EnvFile path] [-VarName name]
# Returns the value of the specified variable, or all variables as VAR=VALUE lines

param(
    [string]$EnvFile = ".env",
    [string]$VarName = ""
)

if (-not (Test-Path $EnvFile)) {
    exit 0
}

Get-Content $EnvFile | ForEach-Object {
    $line = $_.Trim()
    # Skip empty lines and comments
    if ($line -and -not $line.StartsWith('#')) {
        if ($line -match '^([^=]+)=(.*)$') {
            $key = $matches[1].Trim()
            $value = $matches[2].Trim()
            # Remove surrounding quotes if present
            if ($value -match '^"(.*)"$' -or $value -match "^'(.*)'$") {
                $value = $matches[1]
            }
            if ($VarName) {
                if ($key -eq $VarName) {
                    Write-Output $value
                }
            } else {
                Write-Output "$key=$value"
            }
        }
    }
}

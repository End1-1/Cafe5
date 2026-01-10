$path = Join-Path $PSScriptRoot "version.h"

if (!(Test-Path $path)) {
    Write-Host "version.h not found"
    exit 1
}

$content = Get-Content $path -Raw

if ($content -notmatch '#define\s+VER_BUILD\s+(\d+)') {
    Write-Host "VER_BUILD not found"
    exit 1
}

$build = [int]$matches[1] + 1
$content = $content -replace '#define\s+VER_BUILD\s+\d+', "#define VER_BUILD $build"

Set-Content -Path $path -Value $content -Encoding ASCII

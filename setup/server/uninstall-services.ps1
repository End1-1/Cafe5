#Requires -Version 5.1
param(
    [Parameter(Mandatory = $true)]
    [string]$Root
)

$ErrorActionPreference = "Continue"
$Root = $Root.TrimEnd('\')
$service5 = Join-Path $Root "service5\service5.exe"

function Stop-Named([string]$Name) {
    & sc.exe stop $Name | Out-Null
    Start-Sleep -Seconds 2
    & sc.exe delete $Name | Out-Null
}

$httpd = Join-Path $Root "apache\bin\httpd.exe"
Get-Process httpd -ErrorAction SilentlyContinue | Where-Object {
    $_.Path -and $_.Path.StartsWith($Root, [System.StringComparison]::OrdinalIgnoreCase)
} | Stop-Process -Force -ErrorAction SilentlyContinue
Stop-Named "PicassoApache"
$apacheQc = & sc.exe qc Apache2.4 2>&1 | Out-String
if ($apacheQc -match [regex]::Escape($httpd)) {
    Stop-Named "Apache2.4"
}

$mysqld = Join-Path $Root "mysql\bin\mysqld.exe"
if (Test-Path $mysqld) {
    & sc.exe stop PicassoMariaDB | Out-Null
}
Stop-Named "PicassoMariaDB"

$q = & sc.exe qc Breeze 2>&1 | Out-String
if ($q -match [regex]::Escape($service5)) {
    Stop-Named "Breeze"
}

$hostsPath = Join-Path $env:SystemRoot "System32\drivers\etc\hosts"
if (Test-Path $hostsPath) {
    $lines = Get-Content $hostsPath | Where-Object { $_ -notmatch '^\s*127\.0\.0\.1\s+picassoapp\.local\s*$' }
    Set-Content -Path $hostsPath -Value $lines -Encoding ASCII
}

& netsh advfirewall firewall delete rule name="Picasso Apache 80" | Out-Null
& netsh advfirewall firewall delete rule name="Picasso Apache httpd" | Out-Null

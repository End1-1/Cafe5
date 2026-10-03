#Requires -Version 5.1
param(
    [Parameter(Mandatory = $true)]
    [string]$Root
)

$ErrorActionPreference = "Stop"
$Root = $Root.TrimEnd('\')
$Log = Join-Path $Root "install-services.log"
$Slash = ($Root -replace '\\', '/')
$DbPass = "root5"
$DbName = "picasso"

function Log([string]$Message) {
    $line = "{0} {1}" -f (Get-Date -Format "yyyy-MM-dd HH:mm:ss"), $Message
    Add-Content -Path $Log -Value $line -Encoding UTF8
    Write-Host $line
}

trap {
    Log ("ERROR " + $_.Exception.Message)
    if ($_.ScriptStackTrace) { Log $_.ScriptStackTrace }
    exit 1
}

Remove-Item (Join-Path $Root "install-services.ok") -Force -ErrorAction SilentlyContinue

function Replace-Token([string]$Path) {
    if (-not (Test-Path $Path)) { return }
    $text = [IO.File]::ReadAllText($Path)
    if ($text.Contains("__PICASSO_ROOT__")) {
        $text = $text.Replace("__PICASSO_ROOT__", $Slash)
        [IO.File]::WriteAllText($Path, $text)
        Log "paths $Path"
    }
}

function Invoke-Checked([string]$File, [string[]]$ArgumentList, [string]$What) {
    Log "$What : $File $($ArgumentList -join ' ')"
    $pref = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & $File @ArgumentList 2>&1 | ForEach-Object { Log ("$_") }
    $ErrorActionPreference = $pref
    if ($LASTEXITCODE -ne 0) {
        throw "$What failed (exit $LASTEXITCODE)"
    }
}

function Assert-Service([string]$Name) {
    $svc = Get-Service -Name $Name -ErrorAction SilentlyContinue
    if (-not $svc) { throw "service $Name was not registered" }
    Log "service $Name is $($svc.Status)"
}

Log "install root $Root"

$confFiles = @(
    (Join-Path $Root "apache\conf\httpd.conf")
    (Join-Path $Root "apache\conf\extra\httpd-xampp.conf")
    (Join-Path $Root "apache\conf\extra\httpd-vhosts.conf")
    (Join-Path $Root "php\php.ini")
    (Join-Path $Root "mysql\my.ini")
)
Get-ChildItem (Join-Path $Root "apache\conf") -Recurse -Include *.conf,*.ini -File -ErrorAction SilentlyContinue |
    ForEach-Object { $confFiles += $_.FullName }
$confFiles | Select-Object -Unique | ForEach-Object { Replace-Token $_ }

$httpd = Join-Path $Root "apache\bin\httpd.exe"
$mysqld = Join-Path $Root "mysql\bin\mysqld.exe"
$installDb = Join-Path $Root "mysql\bin\mysql_install_db.exe"
$mysql = Join-Path $Root "mysql\bin\mysql.exe"
$myIni = Join-Path $Root "mysql\my.ini"
$dataDir = Join-Path $Root "mysql\data"
$service5 = Join-Path $Root "service5\service5.exe"

if (-not (Test-Path (Join-Path $dataDir "mysql"))) {
    if (Test-Path $dataDir) {
        Get-ChildItem $dataDir -Force | Remove-Item -Recurse -Force
    }
    New-Item -ItemType Directory -Force -Path $dataDir | Out-Null
    Invoke-Checked $installDb @(
        "--datadir=$dataDir",
        "--service=PicassoMariaDB",
        "--password=$DbPass",
        "--port=3306",
        "--config=$myIni",
        "--allow-remote-root-access"
    ) "initialize MariaDB"
} else {
    Log "MariaDB data already present"
}

$maria = Get-Service -Name "PicassoMariaDB" -ErrorAction SilentlyContinue
if (-not $maria) {
    Invoke-Checked $mysqld @("--install", "PicassoMariaDB", "--defaults-file=$myIni") "install PicassoMariaDB"
}
Assert-Service "PicassoMariaDB"
try {
    Start-Service PicassoMariaDB
} catch {
    $err = Get-ChildItem $dataDir -Filter "*.err" -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if ($err) {
        Log "MariaDB error log $($err.FullName)"
        Get-Content $err.FullName -Tail 40 | ForEach-Object { Log $_ }
    }
    throw
}
$ready = $false
for ($i = 0; $i -lt 30; $i++) {
    & $mysql --protocol=tcp -h 127.0.0.1 -P 3306 -u root -e "SELECT 1" 2>$null
    if ($LASTEXITCODE -eq 0) { $ready = $true; break }
    & $mysql --protocol=tcp -h 127.0.0.1 -P 3306 -u root "-p$DbPass" -e "SELECT 1" 2>$null
    if ($LASTEXITCODE -eq 0) { $ready = $true; break }
    Start-Sleep -Seconds 1
}
if (-not $ready) { throw "MariaDB did not accept connections on 127.0.0.1:3306" }

$sql = @"
CREATE DATABASE IF NOT EXISTS $DbName CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
ALTER USER 'root'@'localhost' IDENTIFIED BY '$DbPass';
CREATE USER IF NOT EXISTS 'root'@'127.0.0.1' IDENTIFIED BY '$DbPass';
GRANT ALL PRIVILEGES ON *.* TO 'root'@'127.0.0.1' WITH GRANT OPTION;
FLUSH PRIVILEGES;
"@
& $mysql --protocol=tcp -h 127.0.0.1 -P 3306 -u root -e $sql 2>$null
if ($LASTEXITCODE -ne 0) {
    & $mysql --protocol=tcp -h 127.0.0.1 -P 3306 -u root "-p$DbPass" -e $sql
    if ($LASTEXITCODE -ne 0) { throw "failed to set MariaDB root password / database" }
}
Log "database password set"

$sqlDir = Join-Path $Root "sql"
foreach ($dumpName in @("picasso.sql", "service5.sql")) {
    $dump = Join-Path $sqlDir $dumpName
    if (-not (Test-Path $dump)) {
        throw "missing $dump"
    }
    Log "import $dumpName"
    $import = "`"$mysql`" --protocol=tcp -h 127.0.0.1 -P 3306 -u root -p$DbPass --default-character-set=utf8mb4 < `"$dump`""
    cmd /c $import
    if ($LASTEXITCODE -ne 0) { throw "import $dumpName failed ($LASTEXITCODE)" }
}
Log "databases picasso and service5 imported"

New-Item -ItemType Directory -Force -Path (Join-Path $Root "tmp"), (Join-Path $Root "apache\logs"), (Join-Path $Root "php\logs") | Out-Null
$phpIni = Join-Path $Root "php\php.ini"
if (Test-Path $phpIni) {
    $phpIniText = [IO.File]::ReadAllText($phpIni)
    $phpIniNext = [regex]::Replace($phpIniText, '(?m)^browscap\s*=.*$', ';browscap is not shipped')
    if ($phpIniNext -ne $phpIniText) {
        [IO.File]::WriteAllText($phpIni, $phpIniNext)
        Log "disabled php browscap"
    }
}
$apacheName = "Apache2.4"
$apacheBin = "`"$httpd`" -k runservice"
$apacheQc = & sc.exe qc $apacheName 2>&1 | Out-String
if ($apacheQc -match "SERVICE_NAME:\s*Apache2\.4") {
    Log "configure service Apache2.4"
    & sc.exe config $apacheName binPath= $apacheBin start= auto DisplayName= "Apache2.4"
    if ($LASTEXITCODE -ne 0) { throw "sc config Apache2.4 failed ($LASTEXITCODE)" }
} else {
    Log "create service Apache2.4"
    & sc.exe create $apacheName binPath= $apacheBin start= auto DisplayName= "Apache2.4" depend= Tcpip/Afd
    if ($LASTEXITCODE -ne 0) { throw "sc create Apache2.4 failed ($LASTEXITCODE)" }
}
& sc.exe description $apacheName "Picasso Apache" | Out-Null
& netsh advfirewall firewall add rule name="Picasso Apache 80" dir=in action=allow protocol=TCP localport=80 | Out-Null
& netsh advfirewall firewall add rule name="Picasso Apache httpd" dir=in action=allow program="$httpd" enable=yes | Out-Null
Log "firewall rules added before Apache start"
Get-Process httpd -ErrorAction SilentlyContinue | Where-Object {
    $_.Path -and $_.Path.StartsWith($Root, [System.StringComparison]::OrdinalIgnoreCase)
} | Stop-Process -Force -ErrorAction SilentlyContinue
& sc.exe stop $apacheName | Out-Null
Log "start Apache2.4"
& sc.exe start $apacheName 2>&1 | ForEach-Object { Log "$_" }
$apacheRunning = $false
$apacheStopped = $false
for ($i = 0; $i -lt 60; $i++) {
    $svc = Get-Service -Name $apacheName -ErrorAction SilentlyContinue
    if ($svc -and $svc.Status -eq "Running") { $apacheRunning = $true; break }
    if ($svc -and $svc.Status -eq "Stopped") { $apacheStopped = $true; break }
    Start-Sleep -Seconds 1
}
if (-not $apacheRunning) {
    $pref = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    & sc.exe query $apacheName 2>&1 | ForEach-Object { Log "$_" }
    $apacheErr = Join-Path $Root "apache\logs\error.log"
    if (Test-Path $apacheErr) {
        Log "apache error.log"
        Get-Content $apacheErr -Tail 40 | ForEach-Object { Log $_ }
    } else {
        Log "apache error.log is missing"
    }
    $ErrorActionPreference = $pref
    if ($apacheStopped) { throw "Apache2.4 stopped during start" }
    throw "Apache2.4 stayed pending"
}

if (-not (Test-Path $service5)) { throw "service5.exe not found" }
$q = & sc.exe query Breeze 2>&1 | Out-String
if ($q -match "SERVICE_NAME:\s*Breeze") {
    & sc.exe stop Breeze | Out-Null
    Start-Sleep -Seconds 2
    & sc.exe config Breeze binPath= "`"$service5`"" start= auto
    if ($LASTEXITCODE -ne 0) { throw "sc config Breeze failed" }
} else {
    & $service5 --install
    if ($LASTEXITCODE -ne 0) { throw "service5 --install failed ($LASTEXITCODE)" }
}
& sc.exe start Breeze | Out-Null
Assert-Service "Breeze"
Assert-Service "Apache2.4"
Log "Breeze started"

$hostsPath = Join-Path $env:SystemRoot "System32\drivers\etc\hosts"
$hostsText = [IO.File]::ReadAllText($hostsPath)
if ($hostsText -notmatch "(?m)^\s*127\.0\.0\.1\s+picassoapp\.local\s*$") {
    [IO.File]::AppendAllText($hostsPath, "`r`n127.0.0.1 picassoapp.local`r`n")
    Log "hosts picassoapp.local"
}

Set-Content -Path (Join-Path $Root "install-services.ok") -Value "ok" -Encoding ASCII
Log "done"

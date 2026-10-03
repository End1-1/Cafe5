#Requires -Version 5.1
<#
  Builds setup/staging-server for Server.iss:
  Apache + PHP from the local XAMPP, MariaDB 10.11 binaries (no data),
  web.picassoapp, and Service5.
#>
param(
    [string]$Xampp = "C:\Development\xampp",
    [string]$MariaDb = "C:\Program Files\MariaDB 10.11",
    [string]$BuildRoot = "D:\build.6.10.2",
    [string]$HeidiSql = "C:\Program Files\HeidiSQL"
)

$ErrorActionPreference = "Stop"
$SetupDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Split-Path -Parent $SetupDir
$Stage = Join-Path $SetupDir "staging-server"
$ServerFiles = Join-Path $SetupDir "server"

function Copy-Tree([string]$Source, [string]$Destination, [string[]]$ExcludeDirs) {
    if (-not (Test-Path $Source)) { throw "Missing: $Source" }
    New-Item -ItemType Directory -Force -Path $Destination | Out-Null
    $robo = @($Source, $Destination, "/E", "/NFL", "/NDL", "/NJH", "/NJS", "/NC", "/NS")
    if ($ExcludeDirs -and $ExcludeDirs.Count -gt 0) {
        $robo += @("/XD") + $ExcludeDirs
    }
    & robocopy @robo | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "robocopy failed ($LASTEXITCODE): $Source" }
}

function Read-PhpString([string]$File, [string]$Name) {
    $text = [IO.File]::ReadAllText($File)
    $pattern = '\$' + [regex]::Escape($Name) + '\s*=\s*"([^"]*)"'
    $match = [regex]::Match($text, $pattern)
    if (-not $match.Success) { throw "missing `$$Name in $File" }
    return $match.Groups[1].Value
}

function Export-InstallerDatabases([string]$Www) {
    $cnf = Join-Path $RepoRoot "web.picassoapp\engine\cnf.php"
    $srcHost = Read-PhpString $cnf "dbhost"
    $srcDb = Read-PhpString $cnf "dbname"
    $srcUser = Read-PhpString $cnf "dbuser"
    $srcPass = Read-PhpString $cnf "dbpass"
    $mysql = Join-Path $MariaDb "bin\mysql.exe"
    $dump = Join-Path $MariaDb "bin\mysqldump.exe"
    if (-not (Test-Path $mysql)) { throw "Missing: $mysql" }
    if (-not (Test-Path $dump)) { throw "Missing: $dump" }

    $tmpDb = "picasso_installer_build"
    $sqlOut = Join-Path $Stage "sql"
    New-Item -ItemType Directory -Force -Path $sqlOut | Out-Null
    $raw = Join-Path $env:TEMP "picassodev-installer-raw.sql"
    $env:MYSQL_PWD = $srcPass
    try {
        & $mysql --protocol=tcp -h $srcHost -P 3306 -u $srcUser -N -e "SELECT 1" | Out-Null
        if ($LASTEXITCODE -ne 0) { throw "cannot connect to $srcHost as $srcUser" }

        $keyLines = & $mysql --protocol=tcp -h $srcHost -P 3306 -u $srcUser -N -e "SELECT fkey FROM service5.dblist WHERE fdb='$srcDb' AND fpath='127.0.0.1' LIMIT 1"
        if ($LASTEXITCODE -ne 0) { throw "cannot read service5.dblist" }
        $wsKey = @($keyLines) | Where-Object { $_ -and $_.ToString().Trim() } | Select-Object -First 1
        if (-not $wsKey) { throw "service5.dblist has no row for $srcDb at 127.0.0.1" }
        $wsKey = $wsKey.ToString().Trim()

        Write-Host "copy $srcDb -> $tmpDb (live database is not modified)"
        & $mysql --protocol=tcp -h $srcHost -P 3306 -u $srcUser -e "DROP DATABASE IF EXISTS $tmpDb; CREATE DATABASE $tmpDb CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;"
        if ($LASTEXITCODE -ne 0) { throw "cannot create $tmpDb" }

        & $dump --protocol=tcp -h $srcHost -P 3306 -u $srcUser --single-transaction --routines --triggers --events --default-character-set=utf8mb4 --hex-blob --result-file=$raw $srcDb
        if ($LASTEXITCODE -ne 0) { throw "mysqldump $srcDb failed" }

        $restore = "`"$mysql`" --protocol=tcp -h $srcHost -P 3306 -u $srcUser --default-character-set=utf8mb4 $tmpDb < `"$raw`""
        cmd /c $restore
        if ($LASTEXITCODE -ne 0) { throw "restore into $tmpDb failed" }

        Write-Host "clear $tmpDb (cleardb all)"
        $clear = Join-Path $ServerFiles "clear-for-installer.sql"
        $clearCmd = "`"$mysql`" --protocol=tcp -h $srcHost -P 3306 -u $srcUser --default-character-set=utf8mb4 $tmpDb < `"$clear`""
        cmd /c $clearCmd
        if ($LASTEXITCODE -ne 0) { throw "clear of $tmpDb failed" }

        Write-Host "menu seed into $tmpDb"
        $menuSeed = Join-Path $ServerFiles "menu-seed.sql"
        $menuCmd = "`"$mysql`" --protocol=tcp -h $srcHost -P 3306 -u $srcUser --default-character-set=utf8mb4 $tmpDb < `"$menuSeed`""
        cmd /c $menuCmd
        if ($LASTEXITCODE -ne 0) { throw "menu seed of $tmpDb failed" }

        $picassoSql = Join-Path $sqlOut "picasso.sql"
        & $dump --protocol=tcp -h $srcHost -P 3306 -u $srcUser --single-transaction --routines --triggers --events --default-character-set=utf8mb4 --hex-blob --result-file=$picassoSql $tmpDb
        if ($LASTEXITCODE -ne 0) { throw "dump of cleared database failed" }

        $prefixed = "$picassoSql.prefixed"
        $utf8 = New-Object System.Text.UTF8Encoding $false
        $writer = New-Object IO.StreamWriter($prefixed, $false, $utf8)
        try {
            $writer.WriteLine("DROP DATABASE IF EXISTS ``picasso``;")
            $writer.WriteLine("CREATE DATABASE ``picasso`` CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;")
            $writer.WriteLine("USE ``picasso``;")
            $writer.WriteLine()
        } finally {
            $writer.Dispose()
        }
        $inStream = [IO.File]::OpenRead($picassoSql)
        $outStream = [IO.File]::Open($prefixed, "Append")
        try { $inStream.CopyTo($outStream) } finally { $inStream.Dispose(); $outStream.Dispose() }
        Move-Item $prefixed $picassoSql -Force

        $serviceSql = @"
DROP DATABASE IF EXISTS ``service5``;
CREATE DATABASE ``service5`` CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE ``service5``;
CREATE TABLE ``dblist`` (
  ``fid`` int(11) NOT NULL AUTO_INCREMENT,
  ``fdb`` tinytext DEFAULT NULL,
  ``fname`` tinytext DEFAULT NULL,
  ``fcomment`` tinytext DEFAULT NULL,
  ``fkey`` tinytext DEFAULT NULL,
  ``fpath`` tinytext DEFAULT NULL,
  ``fsettings`` tinytext DEFAULT NULL,
  ``fdescription`` tinytext DEFAULT NULL,
  PRIMARY KEY (``fid``)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
INSERT INTO ``dblist`` (``fdb``, ``fname``, ``fcomment``, ``fkey``, ``fpath``, ``fsettings``, ``fdescription``)
VALUES ('picasso', 'picasso', 'picasso', '$wsKey', '127.0.0.1', '', '');
"@
        [IO.File]::WriteAllText((Join-Path $sqlOut "service5.sql"), $serviceSql, $utf8)

        $cnfOut = Join-Path $Www "engine\cnf.php"
        $cnfText = [IO.File]::ReadAllText($cnfOut).Replace("__WS_KEY__", $wsKey)
        if ($cnfText.Contains("__WS_KEY__")) { throw "websocket key was not written into cnf.php" }
        [IO.File]::WriteAllText($cnfOut, $cnfText)
        Write-Host "sql\picasso.sql and sql\service5.sql ready"
    } finally {
        if ((Test-Path $mysql) -and $srcHost) {
            $prev = $ErrorActionPreference
            $ErrorActionPreference = "Continue"
            & $mysql --protocol=tcp -h $srcHost -P 3306 -u $srcUser -e "DROP DATABASE IF EXISTS $tmpDb" 2>$null
            $ErrorActionPreference = $prev
        }
        Remove-Item $raw -Force -ErrorAction SilentlyContinue
        Remove-Item Env:MYSQL_PWD -ErrorAction SilentlyContinue
    }
}

function Rewrite-XamppPaths([string]$Path) {
    Get-ChildItem $Path -Recurse -Include *.conf,*.ini -File | ForEach-Object {
        $text = [IO.File]::ReadAllText($_.FullName)
        $next = $text.Replace("C:/Development/xampp", "__PICASSO_ROOT__").Replace("C:\Development\xampp", "__PICASSO_ROOT__")
        if ($next -ne $text) {
            [IO.File]::WriteAllText($_.FullName, $next)
        }
    }
}

if (Test-Path $Stage) {
    Remove-Item $Stage -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $Stage | Out-Null

Write-Host "Apache"
$apacheSrc = Join-Path $Xampp "apache"
Copy-Tree $apacheSrc (Join-Path $Stage "apache") @("logs", "manual")
New-Item -ItemType Directory -Force -Path (Join-Path $Stage "apache\logs") | Out-Null
Rewrite-XamppPaths (Join-Path $Stage "apache\conf")
Copy-Item (Join-Path $ServerFiles "httpd-vhosts.conf") (Join-Path $Stage "apache\conf\extra\httpd-vhosts.conf") -Force
$httpdConf = Join-Path $Stage "apache\conf\httpd.conf"
$httpdText = [IO.File]::ReadAllText($httpdConf)
$httpdText = $httpdText.Replace("__PICASSO_ROOT__/htdocs", "__PICASSO_ROOT__/www")
$httpdText = $httpdText.Replace("Include conf/extra/httpd-ssl.conf", "#Include conf/extra/httpd-ssl.conf")
[IO.File]::WriteAllText($httpdConf, $httpdText)
$xamppConf = Join-Path $Stage "apache\conf\extra\httpd-xampp.conf"
$xamppText = [IO.File]::ReadAllText($xamppConf)
$xamppText = $xamppText.Replace('\\xampp\\mysql\\bin', '__PICASSO_ROOT__/mysql/bin')
$xamppText = $xamppText.Replace('\\xampp\\php', '__PICASSO_ROOT__/php')
$xamppText = $xamppText.Replace('\\xampp\\tmp', '__PICASSO_ROOT__/tmp')
[IO.File]::WriteAllText($xamppConf, $xamppText)
New-Item -ItemType Directory -Force -Path (Join-Path $Stage "tmp") | Out-Null

Write-Host "PHP"
Copy-Tree (Join-Path $Xampp "php") (Join-Path $Stage "php") @("pear", "docs", "extras")
Rewrite-XamppPaths (Join-Path $Stage "php")
$phpIni = Join-Path $Stage "php\php.ini"
$phpIniText = [IO.File]::ReadAllText($phpIni)
$phpIniText = [regex]::Replace($phpIniText, '(?m)^browscap\s*=.*$', ';browscap is not shipped')
[IO.File]::WriteAllText($phpIni, $phpIniText)
New-Item -ItemType Directory -Force -Path (Join-Path $Stage "php\logs") | Out-Null

Write-Host "MariaDB binaries (no data)"
$mysqlStage = Join-Path $Stage "mysql"
foreach ($dir in @("bin", "include", "lib", "share")) {
    Copy-Tree (Join-Path $MariaDb $dir) (Join-Path $mysqlStage $dir) @()
}
Copy-Item (Join-Path $ServerFiles "my.ini") (Join-Path $mysqlStage "my.ini") -Force
New-Item -ItemType Directory -Force -Path (Join-Path $mysqlStage "data") | Out-Null

Write-Host "web.picassoapp"
$www = Join-Path $Stage "www"
Copy-Tree (Join-Path $RepoRoot "web.picassoapp") $www @(".git", ".idea", ".vscode")
Copy-Item (Join-Path $ServerFiles "cnf.php") (Join-Path $www "engine\cnf.php") -Force
$media = Join-Path $www "engine\media"
if (Test-Path $media) { Remove-Item $media -Recurse -Force }
New-Item -ItemType Directory -Force -Path $media | Out-Null
[IO.File]::WriteAllText((Join-Path $media "index.html"), "")

Write-Host "Databases from $((Read-PhpString (Join-Path $RepoRoot 'web.picassoapp\engine\cnf.php') 'dbname'))"
Export-InstallerDatabases $www

Write-Host "Service5"
& (Join-Path $SetupDir "stage.ps1") -Component service5 -BuildRoot $BuildRoot
$svcSrc = Join-Path $SetupDir "staging"
$svcDst = Join-Path $Stage "service5"
New-Item -ItemType Directory -Force -Path $svcDst | Out-Null
if (-not (Test-Path (Join-Path $svcSrc "service5.exe"))) {
    throw "service5.exe was not staged. Build Service5 Release first."
}
Copy-Item (Join-Path $svcSrc "service5.exe") $svcDst -Force
foreach ($dll in @("libcrypto-3-x64.dll", "libssl-3-x64.dll", "libmariadb.dll")) {
    $from = Join-Path $svcSrc $dll
    if (Test-Path $from) { Copy-Item $from $svcDst -Force }
}
Get-ChildItem $svcSrc -Filter "Qt6*.dll" | Copy-Item -Destination $svcDst -Force
foreach ($plug in @("platforms", "sqldrivers", "imageformats", "plugins")) {
    $from = Join-Path $svcSrc $plug
    if (Test-Path $from) {
        Copy-Tree $from (Join-Path $svcDst $plug) @()
    }
}
Copy-Item (Join-Path $ServerFiles "config.ini") (Join-Path $svcDst "config.ini") -Force

Write-Host "HeidiSQL"
$heidiDst = Join-Path $Stage "heidisql"
Copy-Tree $HeidiSql $heidiDst @()
Remove-Item (Join-Path $heidiDst "unins000.exe"), (Join-Path $heidiDst "unins000.dat") -Force -ErrorAction SilentlyContinue
Copy-Item (Join-Path $ServerFiles "portable_settings.txt") (Join-Path $heidiDst "portable_settings.txt") -Force

Copy-Item (Join-Path $ServerFiles "install-services.ps1") (Join-Path $Stage "install-services.ps1") -Force
Copy-Item (Join-Path $ServerFiles "uninstall-services.ps1") (Join-Path $Stage "uninstall-services.ps1") -Force

$redist = Join-Path $SetupDir "redist\vc_redist.x64.exe"
if (Test-Path $redist) {
    Copy-Item $redist (Join-Path $Stage "vc_redist.x64.exe") -Force
}

Write-Host "Staging ready: $Stage"

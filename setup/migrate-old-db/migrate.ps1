param(
    [string]$Source = "tarber",
    [string]$Target = "tarber2",
    [string]$Template = "picassodev",
    [string]$DbHost = "localhost",
    [string]$User = "root",
    [string]$Password = "root5",
    [string]$MariaDbBin = "C:\Program Files\MariaDB 10.11\bin",
    [switch]$Recreate
)

$ErrorActionPreference = "Stop"
$mysql = Join-Path $MariaDbBin "mysql.exe"
$mysqldump = Join-Path $MariaDbBin "mysqldump.exe"
$auth = @("--user=$User", "--password=$Password", "--host=$DbHost", "--default-character-set=utf8mb4")

function Invoke-Sql([string]$Db, [string]$File) {
    cmd /c "`"$mysql`" $($auth -join ' ') $Db < `"$File`""
    if ($LASTEXITCODE -ne 0) { throw "mysql failed on $File" }
}

if ($Recreate) {
    & $mysql @auth -e "drop database if exists ``$Target``; create database ``$Target`` character set utf8mb4 collate utf8mb4_general_ci;"
    if ($LASTEXITCODE -ne 0) { throw "cannot recreate $Target" }
}

$tables = & $mysql @auth -N -e "select count(*) from information_schema.tables where table_schema='$Target'"
if ([int]$tables -ne 0) { throw "$Target is not empty ($tables tables)" }

$version = & $mysql @auth -N -e "select f_version from $Template.s_app where lower(f_app)='db'"
Write-Host "Template $Template schema version $version"

$tmp = Join-Path $env:TEMP "migrate-$Target"
New-Item -ItemType Directory -Force -Path $tmp | Out-Null
$schemaFile = Join-Path $tmp "schema.sql"
$dataFile = Join-Path $tmp "data.sql"

& $mysqldump @auth --no-data --routines --triggers --skip-comments --result-file="$schemaFile" $Template
if ($LASTEXITCODE -ne 0) { throw "mysqldump failed" }
$utf8 = New-Object System.Text.UTF8Encoding($false)
$schema = [IO.File]::ReadAllText($schemaFile, $utf8) -replace ' AUTO_INCREMENT=\d+', ''
[IO.File]::WriteAllText($schemaFile, $schema, $utf8)
Invoke-Sql $Target $schemaFile
Write-Host "Schema created in $Target"

$sql = [IO.File]::ReadAllText((Join-Path $PSScriptRoot "migrate.sql"), $utf8)
$sql = $sql.Replace("{SRC}", "``$Source``").Replace("{DST}", "``$Target``").Replace("{TPL}", "``$Template``")
[IO.File]::WriteAllText($dataFile, $sql, $utf8)
Invoke-Sql $Target $dataFile
Write-Host "Data migrated from $Source to $Target"

& $mysql @auth -e "select 'c_groups' t, count(*) n from $Target.c_groups union all select 'c_goods', count(*) from $Target.c_goods union all select 'c_menu', count(*) from $Target.c_menu union all select 'c_goods_multiscancode', count(*) from $Target.c_goods_multiscancode union all select 'h_halls', count(*) from $Target.h_halls union all select 'h_tables', count(*) from $Target.h_tables union all select 's_user', count(*) from $Target.s_user union all select 'c_partners', count(*) from $Target.c_partners union all select 'cash_box', count(*) from $Target.cash_box"

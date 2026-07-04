#Requires -Version 5.1
<#
.SYNOPSIS
  Prepares setup/staging for Picasso.iss

  Copies the shared Qt/runtime tree from D:\build.6.10.2\deploy.desktop.picasso
  and refreshes application EXEs from per-project Release build folders.
#>
param(
    [string]$BuildRoot = "D:\build.6.10.2",
    [string]$DeploySrc = "",
    [string]$QtBin = ""
)

$ErrorActionPreference = "Stop"
$SetupDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$StagingDir = Join-Path $SetupDir "staging"
$RepoRoot = Split-Path -Parent $SetupDir

if (-not $DeploySrc) {
    $DeploySrc = Join-Path $BuildRoot "deploy.desktop.picasso"
}

$ReleasePaths = @{
    OfficeN  = Join-Path $BuildRoot "officen\release\OfficeN.exe"
    Shop_net = Join-Path $BuildRoot "shop\release\Shop_net.exe"
    Waiter   = Join-Path $BuildRoot "waiter\release\Waiter.exe"
    service5 = Join-Path $BuildRoot "service5\release\service5.exe"
}

function Find-QtWindeployQt {
    param([string]$Hint)
    if ($Hint -and (Test-Path $Hint)) { return $Hint }

    $candidates = @(
        "D:\Qt\6.10.2\msvc2022_64\bin\windeployqt.exe",
        "C:\Qt\6.10.2\msvc2022_64\bin\windeployqt.exe",
        "$env:USERPROFILE\Qt\6.10.2\msvc2022_64\bin\windeployqt.exe"
    )
    foreach ($path in $candidates) {
        if (Test-Path $path) { return $path }
    }
    return $null
}

function Read-AppVersion {
    param([string]$VersionHeader)
    if (-not (Test-Path $VersionHeader)) {
        return "1.0.0.0"
    }
    $text = Get-Content $VersionHeader -Raw
    $major = if ($text -match '#define\s+VER_MAJOR\s+(\d+)') { $Matches[1] } else { "0" }
    $minor = if ($text -match '#define\s+VER_MINOR\s+(\d+)') { $Matches[1] } else { "0" }
    $patch = if ($text -match '#define\s+VER_PATCH\s+(\d+)') { $Matches[1] } else { "0" }
    $build = if ($text -match '#define\s+VER_BUILD\s+(\d+)') { $Matches[1] } else { "0" }
    return "$major.$minor.$patch.$build"
}

function Copy-Tree {
    param(
        [string]$Source,
        [string]$Destination
    )
    if (-not (Test-Path $Source)) {
        throw "Source path not found: $Source"
    }
    New-Item -ItemType Directory -Force -Path $Destination | Out-Null
    Copy-Item -Path (Join-Path $Source "*") -Destination $Destination -Recurse -Force
}

function Copy-FirstExisting {
    param(
        [string[]]$Sources,
        [string]$Destination
    )
    foreach ($src in $Sources) {
        if (Test-Path $src) {
            Copy-Item $src $Destination -Force
            Write-Host "  $(Split-Path $Destination -Leaf) <- $src"
            return $true
        }
    }
    return $false
}

Write-Host "Staging directory: $StagingDir"

if (Test-Path $StagingDir) {
    Remove-Item $StagingDir -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $StagingDir | Out-Null

if (-not (Test-Path $DeploySrc)) {
    throw "Deploy source not found: $DeploySrc`nBuild deploy.desktop.picasso first or pass -DeploySrc."
}

Write-Host "Copying shared runtime from $DeploySrc"
Copy-Tree $DeploySrc $StagingDir

$excludeExe = @(
    "Resort.exe", "SelfBoard.exe", "Smart.exe", "appPicasso_PrinterHost.exe",
    "OfficeN.exe", "Shop_net.exe", "Waiter.exe", "service5.exe",
    "create_breeze.bat", "deploy.desktop.picasso.rar"
)
foreach ($name in $excludeExe) {
    $path = Join-Path $StagingDir $name
    if (Test-Path $path) {
        Remove-Item $path -Force
    }
}

Write-Host "Refreshing application binaries from Release builds"
foreach ($entry in $ReleasePaths.GetEnumerator()) {
    $fileName = switch ($entry.Key) {
        "OfficeN"  { "OfficeN.exe" }
        "Shop_net" { "Shop_net.exe" }
        "Waiter"   { "Waiter.exe" }
        "service5" { "service5.exe" }
        default    { "$($entry.Key).exe" }
    }
    $target = Join-Path $StagingDir $fileName
    if (-not (Test-Path $entry.Value)) {
        Write-Warning "Missing build output: $($entry.Value)"
        continue
    }
    Copy-Item $entry.Value $target -Force
    Write-Host "  $fileName <- $($entry.Value)"
}

Write-Host "Copying application styles"
$StyleFiles = @{
    "officestyle.css" = @(
        (Join-Path $BuildRoot "officen\debug\officestyle.css")
        (Join-Path $BuildRoot "officen\release\officestyle.css")
        (Join-Path $RepoRoot "FrontDesk\officestyle.css")
        (Join-Path $DeploySrc "officestyle.css")
    )
    "shop.css" = @(
        (Join-Path $BuildRoot "shop\debug\shop.css")
        (Join-Path $BuildRoot "shop\release\shop.css")
        (Join-Path $RepoRoot "Shop\shop.css")
        (Join-Path $DeploySrc "shop.css")
    )
    "waiter.css" = @(
        (Join-Path $BuildRoot "waiter\debug\waiter.css")
        (Join-Path $BuildRoot "waiter\release\waiter.css")
        (Join-Path $DeploySrc "waiter.css")
        (Join-Path $RepoRoot "Waiter\resources\waiter.qss")
        (Join-Path $RepoRoot "resources\waiter.qss")
    )
}
foreach ($styleName in $StyleFiles.Keys) {
    $dest = Join-Path $StagingDir $styleName
    if (-not (Copy-FirstExisting $StyleFiles[$styleName] $dest)) {
        Write-Warning "$styleName not found in debug/release build folders or repo"
    }
}

$ZkfpRuntimeDlls = @(
    "libzkfp.dll"
    "ZKFPCap.dll"
    "fpslib.dll"
)

function Find-ZkfpRuntimeDll {
    param([string]$FileName)

    $dirs = @(
        (Join-Path $RepoRoot "ztk\lib")
        (Join-Path $BuildRoot "officen\release")
        (Join-Path $BuildRoot "waiter\release")
        $DeploySrc
        (Join-Path $env:SystemRoot "System32")
    )

    foreach ($dir in $dirs) {
        if (-not (Test-Path $dir)) { continue }
        $hit = Get-ChildItem $dir -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -ieq $FileName } |
            Select-Object -First 1
        if ($hit) { return $hit.FullName }
    }
    return $null
}

$missingZkfp = @()
foreach ($dllName in $ZkfpRuntimeDlls) {
    $source = Find-ZkfpRuntimeDll -FileName $dllName
    if (-not $source) {
        $missingZkfp += $dllName
        continue
    }

    $destName = Split-Path $source -Leaf
    Copy-Item $source (Join-Path $StagingDir $destName) -Force
    Write-Host "  $destName <- $source"

    $libDest = Join-Path $RepoRoot "ztk\lib\$destName"
    if (-not (Test-Path $libDest)) {
        Copy-Item $source $libDest -Force
        Write-Host "  Cached $destName to ztk\lib\"
    }
}

if ($missingZkfp.Count -gt 0) {
    throw @"
ZKTeco fingerprint runtime DLL(s) not found:
  $($missingZkfp -join ', ')

Required chain for libzkfp.dll:
  libzkfp.dll, ZKFPCap.dll, fpslib.dll

Copy them to:
  $($RepoRoot)\ztk\lib\

Or install the ZKTeco fingerprint driver (DLLs are in C:\Windows\System32\).
"@
}

$windeployqt = Find-QtWindeployQt -Hint $(if ($QtBin) { Join-Path $QtBin "windeployqt.exe" } else { "" })
if ($windeployqt) {
    Write-Host "Running windeployqt: $windeployqt"
    $exes = @("OfficeN.exe", "Shop_net.exe", "Waiter.exe", "service5.exe")
    foreach ($exe in $exes) {
        $full = Join-Path $StagingDir $exe
        if (Test-Path $full) {
            & $windeployqt --release --no-translations --no-compiler-runtime --dir $StagingDir $full
            if ($LASTEXITCODE -ne 0) {
                Write-Warning "windeployqt returned $LASTEXITCODE for $exe"
            }
        }
    }
} else {
    Write-Warning "windeployqt not found - using DLL set from deploy.desktop.picasso only"
}

$version = Read-AppVersion (Join-Path $RepoRoot "FrontDesk\version.h")
$versionIss = @(
    "; Generated by stage.ps1 - do not edit by hand",
    "#define MyAppVersion `"$version`""
)
Set-Content -Path (Join-Path $SetupDir "version.iss") -Value $versionIss -Encoding ASCII

Write-Host "Done. Version: $version"
Write-Host "Staging ready: $StagingDir"

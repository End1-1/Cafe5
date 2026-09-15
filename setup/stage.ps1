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
    [string]$QtBin = "",
    [switch]$Bump,
    # all | frontdesk | shop | waiter | cookingprogress | service5 — what to require / bump
    [string]$Component = "all"
)

$ErrorActionPreference = "Stop"
$SetupDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$StagingDir = Join-Path $SetupDir "staging"
$RepoRoot = Split-Path -Parent $SetupDir

$Component = $Component.Trim().ToLowerInvariant()
if ($Component -eq "office" -or $Component -eq "officen") { $Component = "frontdesk" }
$validComponents = @("all", "frontdesk", "shop", "waiter", "cookingprogress", "service5", "full")
if ($validComponents -notcontains $Component) {
    throw "Unknown -Component '$Component'. Use: all, frontdesk, shop, waiter, cookingprogress, service5"
}
if ($Component -eq "full") { $Component = "all" }

function Component-Selected {
    param([string]$Name)
    return ($Component -eq "all") -or ($Component -eq $Name)
}

if (-not $DeploySrc) {
    $DeploySrc = Join-Path $BuildRoot "deploy.desktop.picasso"
}

$ReleasePaths = @{
    OfficeN         = Join-Path $BuildRoot "officen\release\OfficeN.exe"
    Shop_net        = Join-Path $BuildRoot "shop\release\Shop_net.exe"
    Waiter          = Join-Path $BuildRoot "waiter\release\Waiter.exe"
    WaiterDesigner  = Join-Path $BuildRoot "WaiterDesigner\release\WaiterDesigner.exe"
    CookingProgress = Join-Path $BuildRoot "cafe5.cookingprogress\release\CookingProgress.exe"
    service5        = Join-Path $BuildRoot "service5\release\service5.exe"
    Updater         = Join-Path $BuildRoot "updater\release\Updater.exe"
}
# Fallbacks if Qt Creator used a different folder name
if (-not (Test-Path $ReleasePaths["CookingProgress"])) {
    foreach ($alt in @(
        (Join-Path $BuildRoot "cookingprogress\release\CookingProgress.exe"),
        (Join-Path $BuildRoot "CookingProgress\release\CookingProgress.exe")
    )) {
        if (Test-Path $alt) {
            $ReleasePaths["CookingProgress"] = $alt
            break
        }
    }
}

function Find-QtWindeployQt {
    param([string]$Hint)
    if ($Hint -and (Test-Path $Hint)) { return $Hint }

    $candidates = @(
        "C:\Development\Qt\6.10.2\msvc2022_64\bin\windeployqt.exe",
        "D:\Qt\6.10.2\msvc2022_64\bin\windeployqt.exe",
        "C:\Qt\6.10.2\msvc2022_64\bin\windeployqt.exe",
        "$env:USERPROFILE\Qt\6.10.2\msvc2022_64\bin\windeployqt.exe"
    )
    foreach ($path in $candidates) {
        if (Test-Path $path) { return $path }
    }
    return $null
}

function Ensure-QtImageFormatPlugin {
    param(
        [string]$PluginName,
        [string]$StagingRoot,
        [string]$WindeployQtPath
    )
    $dstDir = Join-Path $StagingRoot "imageformats"
    $dst = Join-Path $dstDir $PluginName
    if (Test-Path $dst) {
        Write-Host "  imageformats\$PluginName already present"
        return
    }

    $srcCandidates = @()
    if ($WindeployQtPath) {
        $qtRoot = Split-Path -Parent (Split-Path -Parent $WindeployQtPath)
        $srcCandidates += (Join-Path $qtRoot "plugins\imageformats\$PluginName")
    }
    if ($QtBin) {
        $qtRootFromBin = Split-Path -Parent $QtBin
        $srcCandidates += (Join-Path $qtRootFromBin "plugins\imageformats\$PluginName")
    }
    $srcCandidates += @(
        "C:\Development\Qt\6.10.2\msvc2022_64\plugins\imageformats\$PluginName"
        "D:\Qt\6.10.2\msvc2022_64\plugins\imageformats\$PluginName"
        "C:\Qt\6.10.2\msvc2022_64\plugins\imageformats\$PluginName"
        "$env:USERPROFILE\Qt\6.10.2\msvc2022_64\plugins\imageformats\$PluginName"
    )

    foreach ($src in $srcCandidates) {
        if (Test-Path $src) {
            New-Item -ItemType Directory -Force -Path $dstDir | Out-Null
            Copy-Item $src $dst -Force
            Write-Host "  imageformats\$PluginName <- $src"
            return
        }
    }
    Write-Warning "Qt imageformats plugin not found: $PluginName (WebP upload will fail on clients)"
}

function Read-AppVersionParts {
    param([string]$VersionHeader)
    $major = 0; $minor = 0; $patch = 0; $build = 0
    if (Test-Path $VersionHeader) {
        $text = Get-Content $VersionHeader -Raw
        if ($text -match '#define\s+VER_MAJOR\s+(\d+)') { $major = [int]$Matches[1] }
        if ($text -match '#define\s+VER_MINOR\s+(\d+)') { $minor = [int]$Matches[1] }
        if ($text -match '#define\s+VER_PATCH\s+(\d+)') { $patch = [int]$Matches[1] }
        if ($text -match '#define\s+VER_BUILD\s+(\d+)') { $build = [int]$Matches[1] }
    }
    return @{ Major = $major; Minor = $minor; Patch = $patch; Build = $build }
}

function Format-AppVersion {
    param($Parts)
    return "$($Parts.Major).$($Parts.Minor).$($Parts.Patch).$($Parts.Build)"
}

function Format-AppVersionShort {
    param($Parts)
    return "$($Parts.Major).$($Parts.Minor).$($Parts.Patch)"
}

function Read-AppVersion {
    param([string]$VersionHeader)
    return Format-AppVersion (Read-AppVersionParts $VersionHeader)
}

function Bump-VersionHeader {
    param(
        [string]$VersionHeader,
        [switch]$BumpPatch
    )
    if (-not (Test-Path $VersionHeader)) {
        throw "version.h not found: $VersionHeader"
    }
    $text = Get-Content $VersionHeader -Raw
    $parts = Read-AppVersionParts $VersionHeader
    $parts.Build = $parts.Build + 1
    if ($BumpPatch) {
        $parts.Patch = $parts.Patch + 1
    }
    $text = [regex]::Replace($text, '#define\s+VER_PATCH\s+\d+', "#define VER_PATCH $($parts.Patch)")
    $text = [regex]::Replace($text, '#define\s+VER_BUILD\s+\d+', "#define VER_BUILD $($parts.Build)")
    # Keep a trailing newline for editors / compilers
    if (-not $text.EndsWith("`n")) { $text += "`r`n" }
    $utf8 = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllText($VersionHeader, $text, $utf8)
    return $parts
}

function Update-CheckAppVersions {
    param([hashtable]$ShortVersions)
    $checkApp = Join-Path $RepoRoot "web.picassoapp\engine\v2\check-app.php"
    if (-not (Test-Path $checkApp)) {
        Write-Warning "check-app.php not found: $checkApp"
        return
    }
    $text = Get-Content $checkApp -Raw
    foreach ($entry in $ShortVersions.GetEnumerator()) {
        $key = $entry.Key
        $ver = $entry.Value
        $text = [regex]::Replace(
            $text,
            "(`"$([regex]::Escape($key))`"\s*=>\s*`")[^`"]+(`")",
            "`${1}$ver`${2}"
        )
    }
    $utf8 = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllText($checkApp, $text, $utf8)
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

# By default keep version.h as-is (installer name = current app version).
# Pass -Bump to increment VER_PATCH/VER_BUILD (only for selected -Component).
if ($Bump) {
    Write-Host "Bumping versions for component: $Component"
} else {
    Write-Host "Using current versions from version.h (no bump); component: $Component"
}

if ($Bump -and (Component-Selected "frontdesk")) {
    $fdParts = Bump-VersionHeader (Join-Path $RepoRoot "FrontDesk\version.h") -BumpPatch
} else {
    $fdParts = Read-AppVersionParts (Join-Path $RepoRoot "FrontDesk\version.h")
}
if ($Bump -and (Component-Selected "shop")) {
    $shopParts = Bump-VersionHeader (Join-Path $RepoRoot "Shop\version.h") -BumpPatch
} else {
    $shopParts = Read-AppVersionParts (Join-Path $RepoRoot "Shop\version.h")
}
if ($Bump -and (Component-Selected "waiter")) {
    $waiterParts = Bump-VersionHeader (Join-Path $RepoRoot "Waiter\version.h") -BumpPatch
} else {
    $waiterParts = Read-AppVersionParts (Join-Path $RepoRoot "Waiter\version.h")
}
if ($Bump -and (Component-Selected "cookingprogress")) {
    $cpParts = Bump-VersionHeader (Join-Path $RepoRoot "CookingProgress\version.h") -BumpPatch
} else {
    $cpParts = Read-AppVersionParts (Join-Path $RepoRoot "CookingProgress\version.h")
}

$service5VerFile = Join-Path $SetupDir "service5_version.txt"
if (-not (Test-Path $service5VerFile)) {
    Set-Content -Path $service5VerFile -Value "1.0.0" -Encoding ASCII
}
$service5Short = (Get-Content $service5VerFile -Raw).Trim()
if ($Bump -and (Component-Selected "service5")) {
    if ($service5Short -match '^(\d+)\.(\d+)\.(\d+)$') {
        $sMajor = [int]$Matches[1]; $sMinor = [int]$Matches[2]; $sPatch = [int]$Matches[3] + 1
        $service5Short = "$sMajor.$sMinor.$sPatch"
    } else {
        $service5Short = "1.0.1"
    }
    Set-Content -Path $service5VerFile -Value $service5Short -Encoding ASCII
}

$fdShort = Format-AppVersionShort $fdParts
$shopShort = Format-AppVersionShort $shopParts
$waiterShort = Format-AppVersionShort $waiterParts
$cpShort = Format-AppVersionShort $cpParts
$fdFull = Format-AppVersion $fdParts

if ($Bump) {
    $checkUpdates = @{}
    if (Component-Selected "frontdesk") { $checkUpdates["officen"] = $fdShort }
    if (Component-Selected "shop") { $checkUpdates["shop"] = $shopShort }
    if (Component-Selected "waiter") { $checkUpdates["waiter"] = $waiterShort }
    if (Component-Selected "cookingprogress") { $checkUpdates["cookingprogress"] = $cpShort }
    if ($checkUpdates.Count -gt 0) {
        Update-CheckAppVersions $checkUpdates
    }
}

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
    "OfficeN.exe", "Shop_net.exe", "Waiter.exe", "WaiterDesigner.exe", "CookingProgress.exe", "service5.exe",
    "create_breeze.bat", "deploy.desktop.picasso.rar"
)
foreach ($name in $excludeExe) {
    $path = Join-Path $StagingDir $name
    if (Test-Path $path) {
        Remove-Item $path -Force
    }
}

Write-Host "Refreshing application binaries from Release builds"
$exeToComponent = @{
    "OfficeN"         = "frontdesk"
    "Shop_net"        = "shop"
    "Waiter"          = "waiter"
    "CookingProgress" = "cookingprogress"
    "service5"        = "service5"
}
$requiredExeNames = @{
    "frontdesk"       = "OfficeN.exe"
    "shop"            = "Shop_net.exe"
    "waiter"          = "Waiter.exe"
    "cookingprogress" = "CookingProgress.exe"
    "service5"        = "service5.exe"
}
foreach ($entry in $ReleasePaths.GetEnumerator()) {
    if ($entry.Key -eq "Updater" -or $entry.Key -eq "WaiterDesigner") { continue }
    $compKey = $exeToComponent[$entry.Key]
    $fileName = $requiredExeNames[$compKey]
    $target = Join-Path $StagingDir $fileName
    if (-not (Test-Path $entry.Value)) {
        if (Component-Selected $compKey) {
            throw "Missing required build output for $compKey`: $($entry.Value)"
        }
        Write-Warning "Missing build output (skipped): $($entry.Value)"
        continue
    }
    Copy-Item $entry.Value $target -Force
    Write-Host "  $fileName <- $($entry.Value)"
}

if (Component-Selected "waiter") {
    $designerSrc = $ReleasePaths["WaiterDesigner"]
    $designerDst = Join-Path $StagingDir "WaiterDesigner.exe"
    if (-not (Test-Path $designerSrc)) {
        throw "Missing required build output for WaiterDesigner: $designerSrc"
    }
    Copy-Item $designerSrc $designerDst -Force
    Write-Host "  WaiterDesigner.exe <- $designerSrc"
}

# Every module installer ships a fresh shared updater.
$updaterSrc = $ReleasePaths["Updater"]
$updaterDst = Join-Path $StagingDir "Updater.exe"
if (-not (Test-Path $updaterSrc)) {
    throw "Missing required build output for Updater: $updaterSrc"
}
Copy-Item $updaterSrc $updaterDst -Force
Write-Host "  Updater.exe <- $updaterSrc"

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
        (Join-Path $RepoRoot "Waiter\waiter.css")
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
    "libzksensorcore.dll"
    "libusb0.dll"
    "zkfpslibLow.dll"
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

function Find-ZkfpSensorsDir {
    $dirs = @(
        (Join-Path $RepoRoot "ztk\lib\ZKFPSensors")
        (Join-Path $BuildRoot "officen\release\ZKFPSensors")
        (Join-Path $BuildRoot "waiter\release\ZKFPSensors")
        (Join-Path $DeploySrc "ZKFPSensors")
        (Join-Path $env:SystemRoot "System32\ZKFPSensors")
    )
    foreach ($dir in $dirs) {
        if ((Test-Path $dir) -and (Get-ChildItem $dir -Filter "*.dll" -ErrorAction SilentlyContinue)) {
            return $dir
        }
    }
    return $null
}

# ZKTeco fingerprint runtime is only needed for FrontDesk / Waiter (and full suite).
$needZkfp = (Component-Selected "frontdesk") -or (Component-Selected "waiter")
if ($needZkfp) {
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

    $sensorsSrc = Find-ZkfpSensorsDir
    if ($sensorsSrc) {
        $sensorsDst = Join-Path $StagingDir "ZKFPSensors"
        New-Item -ItemType Directory -Force -Path $sensorsDst | Out-Null
        Copy-Item (Join-Path $sensorsSrc "*") $sensorsDst -Force
        Write-Host "  ZKFPSensors\ <- $sensorsSrc"

        $libSensors = Join-Path $RepoRoot "ztk\lib\ZKFPSensors"
        if (-not (Test-Path $libSensors)) {
            New-Item -ItemType Directory -Force -Path $libSensors | Out-Null
            Copy-Item (Join-Path $sensorsSrc "*") $libSensors -Force
            Write-Host "  Cached ZKFPSensors\ to ztk\lib\"
        }
    } else {
        $missingZkfp += "ZKFPSensors\*.dll"
    }

    if ($missingZkfp.Count -gt 0) {
        throw @"
ZKTeco fingerprint runtime DLL(s) not found:
  $($missingZkfp -join ', ')

Required chain for libzkfp.dll:
  libzkfp.dll, ZKFPCap.dll, fpslib.dll,
  libzksensorcore.dll, libusb0.dll, zkfpslibLow.dll,
  ZKFPSensors\*.dll

Copy them to:
  $($RepoRoot)\ztk\lib\

Or install the ZKTeco fingerprint driver (DLLs are in C:\Windows\System32\).
"@
    }
} else {
    Write-Host "Skipping ZKTeco DLL check (not needed for component: $Component)"
}

$windeployqt = Find-QtWindeployQt -Hint $(if ($QtBin) { Join-Path $QtBin "windeployqt.exe" } else { "" })
if ($windeployqt) {
    Write-Host "Running windeployqt: $windeployqt"
    $exes = @()
    if (Component-Selected "frontdesk") { $exes += "OfficeN.exe" }
    if (Component-Selected "shop") { $exes += "Shop_net.exe" }
    if (Component-Selected "waiter") {
        $exes += "Waiter.exe"
        $exes += "WaiterDesigner.exe"
    }
    if (Component-Selected "cookingprogress") { $exes += "CookingProgress.exe" }
    if (Component-Selected "service5") { $exes += "service5.exe" }
    $exes += "Updater.exe"
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

# windeployqt often skips unused image plugins; FrontDesk needs WebP for goods images.
Write-Host "Ensuring Qt imageformats plugins"
Ensure-QtImageFormatPlugin -PluginName "qwebp.dll" -StagingRoot $StagingDir -WindeployQtPath $windeployqt

function Find-VCRedist {
    $cached = Join-Path $SetupDir "redist\vc_redist.x64.exe"
    if (Test-Path $cached) { return $cached }

    $vsRoots = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022"
        "${env:ProgramFiles}\Microsoft Visual Studio\2022"
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019"
    )
    foreach ($root in $vsRoots) {
        if (-not (Test-Path $root)) { continue }
        $hit = Get-ChildItem $root -Recurse -Filter "vc_redist.x64.exe" -ErrorAction SilentlyContinue |
            Select-Object -First 1
        if ($hit) { return $hit.FullName }
    }
    return $null
}

Write-Host "Preparing VC++ Redistributable (vc_redist.x64.exe)"
$RedistDir = Join-Path $SetupDir "redist"
New-Item -ItemType Directory -Force -Path $RedistDir | Out-Null
$RedistCached = Join-Path $RedistDir "vc_redist.x64.exe"
$RedistSrc = Find-VCRedist
if (-not $RedistSrc) {
    $url = "https://aka.ms/vs/17/release/vc_redist.x64.exe"
    Write-Host "  Downloading $url"
    try {
        Invoke-WebRequest -Uri $url -OutFile $RedistCached -UseBasicParsing
        $RedistSrc = $RedistCached
    } catch {
        throw "Failed to download VC++ Redistributable.`nPlace vc_redist.x64.exe in:`n  $RedistDir"
    }
} elseif ($RedistSrc -ne $RedistCached) {
    Copy-Item $RedistSrc $RedistCached -Force
    Write-Host "  Cached from $RedistSrc"
    $RedistSrc = $RedistCached
}
Copy-Item $RedistSrc (Join-Path $StagingDir "vc_redist.x64.exe") -Force
Write-Host "  vc_redist.x64.exe <- $RedistSrc"

$versionIss = @(
    "; Generated by stage.ps1 - do not edit by hand"
    "#define MyAppVersion `"$fdFull`""
    "#define MyAppVersionShort `"$fdShort`""
    "#define FrontDeskVersionShort `"$fdShort`""
    "#define ShopVersionShort `"$shopShort`""
    "#define WaiterVersionShort `"$waiterShort`""
    "#define CookingProgressVersionShort `"$cpShort`""
    "#define Service5VersionShort `"$service5Short`""
)
Set-Content -Path (Join-Path $SetupDir "version.iss") -Value $versionIss -Encoding ASCII

$versionsJson = @{
    frontdesk = $fdShort
    shop = $shopShort
    waiter = $waiterShort
    cookingprogress = $cpShort
    service5 = $service5Short
    full = $fdFull
} | ConvertTo-Json
Set-Content -Path (Join-Path $SetupDir "versions.json") -Value $versionsJson -Encoding UTF8

Write-Host "Done. FrontDesk: $fdShort  Shop: $shopShort  Waiter: $waiterShort  CookingProgress: $cpShort  Service5: $service5Short"
Write-Host "Staging ready: $StagingDir (component: $Component)"
if ($Bump) {
    Write-Host ""
    Write-Host "NOTE: version was bumped for '$Component'. Rebuild Release apps so EXE matches installer version,"
    if ($Component -eq "all") {
        Write-Host "then run: setup\build_installer.bat"
    } else {
        Write-Host "then run: setup\build_installer.bat $Component"
    }
}

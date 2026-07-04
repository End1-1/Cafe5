# Picasso installer (Inno Setup)

Unified Windows installer for **FrontDesk**, **Shop**, **Waiter**, and **Service5** (Windows service **Breeze**).

## Requirements

- Inno Setup 6 (`ISCC.exe`)
- Release builds in `D:\build.6.10.2\` (Qt Creator kit **Desktop Qt 6.10.2 MSVC2022 64bit**)
- Deploy tree `D:\build.6.10.2\deploy.desktop.picasso` (shared Qt/OpenSSL/MariaDB runtime)
- Optional: `windeployqt` from Qt 6.10.2 (refreshes DLL set after copying new EXEs)

## Build paths (Release)

| Application | Path |
|-------------|------|
| FrontDesk (OfficeN) | `D:\build.6.10.2\officen\release\OfficeN.exe` |
| Shop | `D:\build.6.10.2\shop\release\Shop_net.exe` |
| Waiter | `D:\build.6.10.2\waiter\release\Waiter.exe` |
| Service5 | `D:\build.6.10.2\service5\release\service5.exe` |

Style files (`officestyle.css`, `shop.css`, `waiter.css`) are taken from **debug** build folders first, then release, repo, deploy.

## Build installer

```bat
setup\build_installer.bat
```

Or step by step:

```powershell
setup\stage.ps1
& "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" setup\Picasso.iss
```

Output: `setup\output\Picasso_Setup_x64.exe`

### stage.ps1 parameters

```powershell
setup\stage.ps1 -BuildRoot "D:\build.6.10.2"
setup\stage.ps1 -QtBin "D:\Qt\6.10.2\msvc2022_64\bin"
```

Place ZKTeco x64 runtime DLLs in `ztk\lib\` for reproducible builds:

- `libzkfp.dll` — main SDK
- `ZKFPCap.dll` — capture layer (required by libzkfp)
- `fpslib.dll` — matching library (required by libzkfp)

If they are missing there, `stage.ps1` falls back to `C:\Windows\System32\` (after ZKTeco driver install). All three are included in the installer for FrontDesk and Waiter.

Note: `zkfpslibLow.dll` and `zkfinger10.dll` may exist on the system but are not direct dependencies of `libzkfp.dll`.

## Install defaults

- Default directory: `C:\Program Files\Picasso` (`{autopf}\Picasso`), user can change it
- Components: FrontDesk, Shop, Waiter, Service5
- Upgrade: same `AppId`, keeps install path, overwrites binaries (`ignoreversion`)
- Before install: warning if OfficeN / Shop / Waiter / service5 processes are running; Inno Restart Manager closes them after confirmation
- Service5: creates/starts Windows service **Breeze** with  
  `"<app>\service5.exe" --config="<app>\config.ini"`
- No extra handler DLLs from `Service5/dlls/`
- `libzkfp.dll`, `ZKFPCap.dll`, `fpslib.dll` (ZKTeco fingerprint) — installed with FrontDesk or Waiter

## Uninstall

- Removes selected application files
- Stops and deletes service **Breeze** if Service5 was installed (registry flag `HKLM\Software\Picasso\Service5Installed`)

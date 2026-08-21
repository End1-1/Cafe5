# Picasso installer (Inno Setup)

Unified Windows installer for **FrontDesk**, **Shop**, **Waiter**, **CookingProgress**, and **Service5** (Windows service **Breeze**).

## Requirements

- Inno Setup 6 (`ISCC.exe`)
- Release builds in `D:\build.6.10.2\` (Qt Creator kit **Desktop Qt 6.10.2 MSVC2022 64bit**)
- Deploy tree `D:\build.6.10.2\deploy.desktop.picasso` (shared Qt/OpenSSL/MariaDB runtime)
- Optional: `windeployqt` from Qt 6.10.2 (refreshes DLL set after copying new EXEs)
- `stage.ps1` always ships `imageformats\qwebp.dll` (WebP goods images in FrontDesk) even if windeployqt omits it
- VC++ Redistributable x64 (`vc_redist.x64.exe`) — `stage.ps1` copies from `setup\redist\`, Visual Studio, or downloads from Microsoft if missing

## Build paths (Release)

| Application | Path |
|-------------|------|
| FrontDesk (OfficeN) | `D:\build.6.10.2\officen\release\OfficeN.exe` |
| Shop | `D:\build.6.10.2\shop\release\Shop_net.exe` |
| Waiter | `D:\build.6.10.2\waiter\release\Waiter.exe` |
| WaiterDesigner (with Waiter) | `D:\build.6.10.2\WaiterDesigner\release\WaiterDesigner.exe` |
| CookingProgress | `D:\build.6.10.2\cafe5.cookingprogress\release\CookingProgress.exe` |
| Service5 | `D:\build.6.10.2\service5\release\service5.exe` |

Style files (`officestyle.css`, `shop.css`, `waiter.css`) are taken from **debug** build folders first, then release, repo, deploy.

## Build installer

### One package

```bat
setup\build_shop.bat
setup\build_frontdesk.bat
setup\build_waiter.bat
setup\build_cookingprogress.bat
setup\build_service5.bat
```

Or via the main script:

```bat
setup\build_installer.bat shop
setup\build_installer.bat frontdesk
setup\build_installer.bat waiter
setup\build_installer.bat cookingprogress
setup\build_installer.bat service5
```

Only that component’s EXE is required (Waiter also needs `WaiterDesigner.exe`); ZKTeco DLLs are checked only for FrontDesk / Waiter.

### Full suite (all packages)

```bat
setup\build_installer.bat
```

Same as `setup\build_installer.bat all` — builds unified `Picasso_Setup_x64.exe` plus all component installers.

Packages current versions from `version.h` (e.g. FrontDesk `2.10.45` → `frontdesk_setup_2.10.45.exe`).

### Bump version

Increment `VER_PATCH` / `VER_BUILD` (and `check-app.php` for desktop apps) only for the selected package:

```bat
setup\build_shop.bat bump
setup\build_installer.bat shop bump
setup\build_installer.bat bump
```

Then rebuild Release apps and run again **without** bump so EXE and installer match.

### Step by step (PowerShell)

```powershell
setup\stage.ps1 -Component shop              # current versions, no bump
# or: setup\stage.ps1 -Component shop -Bump
powershell -File setup\build_components.ps1 -Iscc "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" -Component shop
```

Full suite:

```powershell
setup\stage.ps1
& "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" setup\Picasso.iss
powershell -File setup\build_components.ps1 -Iscc "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
```

### Output

| Installer | Path |
|-----------|------|
| Full suite | `setup\output\Picasso_Setup_x64.exe` |
| FrontDesk | `setup\output\frontdesk\frontdesk_setup_X.Y.Z.exe` |
| Shop | `setup\output\shop\shop_setup_X.Y.Z.exe` |
| Waiter | `setup\output\waiter\waiter_setup_X.Y.Z.exe` |
| CookingProgress | `setup\output\cookingprogress\cookingprogress_setup_X.Y.Z.exe` |
| Service5 | `setup\output\service5\service5_setup_X.Y.Z.exe` |

Publish component installers to `https://picasso.am/files/` (same filenames).

### Auto-update (desktop apps)

When the server returns HTTP 426 (version mismatch in `check-app.php`):

1. The app shows **one** question: update is required to continue (Yes / No).
2. **No** — application exits.
3. **Yes** — starts `{app}\Updater.exe`, then exits.
4. **Updater self-update / unlock:** `Updater.exe` copies itself to `%TEMP%\PicassoUpdateHost.exe` and relaunches from TEMP. Qt DLLs are copied to `%TEMP%\PicassoUpdateRt\` (not loaded from `{app}`), so Inno can overwrite `{app}\Qt6*.dll` while the host is still running.
5. Host stops Breeze, kills Picasso desktop apps, downloads `{component}_setup_X.Y.Z.exe` from `https://picasso.am/files/` using **WinHTTP**, and runs it with `/SILENT /NORESTART /CLOSEAPPLICATIONS /FORCECLOSEAPPLICATIONS`. Windows UAC may still appear because installers require admin.
6. The component installer always ships a fresh `Updater.exe` next to FrontDesk / Shop / Waiter / CookingProgress — so the next update uses the new updater.

**Compatibility rule:** keep Updater CLI (`--app`, `--version`) and the URL pattern `https://picasso.am/files/{component}_setup_{version}.exe` stable (or backward-compatible). An old updater on the machine must still be able to download and run the new setup.

**Breeze service:** all component packages (FrontDesk / Shop / Waiter / CookingProgress / Service5) and the full suite installer stop Windows service **Breeze** when it is present (shared Qt DLLs in `{app}`). After a successful install they start it again. Only the **Service5** package and the full suite (when the service5 component is selected) create/register the service.

### stage.ps1 parameters

```powershell
setup\stage.ps1 -BuildRoot "D:\build.6.10.2"
setup\stage.ps1 -QtBin "D:\Qt\6.10.2\msvc2022_64\bin"
setup\stage.ps1 -Component shop          # only require Shop EXE; skip ZKTeco
setup\stage.ps1 -Component frontdesk -Bump
```

Place ZKTeco x64 runtime DLLs in `ztk\lib\` for reproducible builds:

- `libzkfp.dll` — main SDK
- `ZKFPCap.dll` — capture layer (required by libzkfp)
- `fpslib.dll` — matching library (required by libzkfp)
- `libzksensorcore.dll` — required by ZKFPCap
- `libusb0.dll` — required by libzksensorcore
- `zkfpslibLow.dll` — required by fpslib
- `ZKFPSensors\*.dll` — sensor plugins loaded by ZKFPCap (`libzklibcap.dll`, …)

If they are missing there, `stage.ps1` falls back to `C:\Windows\System32\` (after ZKTeco driver install). All of the above are included in the installer for FrontDesk and Waiter.

Note: USB filter driver (`libusb0.sys`) still comes from the ZKTeco device driver install; shipping `libusb0.dll` alone is not a substitute for the driver.
## Install defaults

- Default directory: `C:\Program Files\Picasso` (`{autopf}\Picasso`), user can change it
- Components: FrontDesk, Shop, Waiter, CookingProgress, Service5
- Upgrade: same `AppId`, keeps install path, overwrites binaries (`ignoreversion`)
- Shortcuts (Start Menu / desktop): created on fresh install or only for a **newly added** component; existing shortcuts are not overwritten on update
- Before install: warning if OfficeN / Shop / Waiter / CookingProgress / service5 processes are running; Inno Restart Manager closes them after confirmation
- VC++ 2015–2022 x64: if registry `VisualStudio\14.0\VC\Runtimes\x64` is missing or older than 14.30 (VS 2022), runs `vc_redist.x64.exe /install /quiet /norestart` before copying app files
- Service5: after all files are installed, creates/starts Windows service **Breeze** via `service5.exe --install` (fallback: `sc create`), writes default `config.ini` if missing
  - Path: `"<app>\service5.exe"` (config: `"<app>\config.ini"`)
- No extra handler DLLs from `Service5/dlls/`
- `libzkfp.dll`, `ZKFPCap.dll`, `fpslib.dll`, `libzksensorcore.dll`, `libusb0.dll`, `zkfpslibLow.dll`, `ZKFPSensors\*` (ZKTeco fingerprint) — installed with FrontDesk or Waiter

## Uninstall

- Warns if OfficeN / Shop / Waiter / CookingProgress / service5 are running; closes them after confirmation
- Stops and deletes Windows service **Breeze** if Service5 was installed
- Removes HKCU connection settings (`Jazzve\Cafe5\FRONTDESK`, `Jazzve\Cafe5\waiter`, `BreezeDevs\Shop\Shop`, `BreezeDevs\CookingProgress\CookingProgress`) and HKLM `Software\Picasso`
- Deletes the install directory (`{app}`, e.g. `C:\Program Files\Picasso`) including leftovers (`config.ini`, logs, untracked DLLs)

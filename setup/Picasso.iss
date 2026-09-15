; Picasso desktop applications — unified installer
; Build: setup\build_installer.bat

#include "version.iss"

#define MyAppName "Picasso"
#define MyAppPublisher "BreezeDevs"
#define MyAppURL "https://www.breezedevs.com"
#define StagingDir "staging"
#define ServiceExe "service5.exe"

[Setup]
AppId={{A3F8C2E1-9B4D-4F6A-8C7E-1D2E3F4A5B6C}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=no
OutputDir=output
OutputBaseFilename=Picasso_Setup_x64
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
UsePreviousAppDir=yes
UsePreviousTasks=yes
CloseApplications=force
CloseApplicationsFilter=OfficeN.exe,Shop_net.exe,Waiter.exe,WaiterDesigner.exe,CookingProgress.exe,service5.exe,Service5.exe,Updater.exe,PicassoUpdateHost.exe
RestartApplications=no
ShowLanguageDialog=auto

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "frontdesk"; Description: "FrontDesk (OfficeN)"; Types: full custom
Name: "shop"; Description: "Shop"; Types: full custom
Name: "waiter"; Description: "Waiter"; Types: full custom
Name: "cookingprogress"; Description: "CookingProgress"; Types: full custom
Name: "service5"; Description: "Service5 (Windows service Breeze)"; Types: full custom

[Tasks]
Name: "desktop_frontdesk"; Description: "Desktop icon: FrontDesk"; GroupDescription: "Desktop icons:"; Components: frontdesk; Flags: unchecked
Name: "desktop_shop"; Description: "Desktop icon: Shop"; GroupDescription: "Desktop icons:"; Components: shop; Flags: unchecked
Name: "desktop_waiter"; Description: "Desktop icon: Waiter"; GroupDescription: "Desktop icons:"; Components: waiter; Flags: unchecked
Name: "desktop_waiter_designer"; Description: "Desktop icon: WaiterDesigner"; GroupDescription: "Desktop icons:"; Components: waiter
Name: "desktop_cookingprogress"; Description: "Desktop icon: CookingProgress"; GroupDescription: "Desktop icons:"; Components: cookingprogress; Flags: unchecked

[Files]
; --- Application binaries (each module in its own subfolder) ---
Source: "{#StagingDir}\OfficeN.exe"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\Shop_net.exe"; DestDir: "{app}\shop"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\Waiter.exe"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\WaiterDesigner.exe"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#StagingDir}\CookingProgress.exe"; DestDir: "{app}\cookingprogress"; Components: cookingprogress; Flags: ignoreversion
Source: "{#StagingDir}\service5.exe"; DestDir: "{app}"; Components: service5; Flags: ignoreversion; BeforeInstall: BeforeServiceInstall
Source: "{#StagingDir}\Updater.exe"; DestDir: "{app}\updater"; Flags: ignoreversion
Source: "{#StagingDir}\Qt6*.dll"; DestDir: "{app}\updater"; Flags: ignoreversion
Source: "{#StagingDir}\platforms\*"; DestDir: "{app}\updater\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs

; --- Styles / assets ---
Source: "{#StagingDir}\officestyle.css"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\shop.css"; DestDir: "{app}\shop"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\waiter.css"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\ahuni.ttf"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\ahuni.ttf"; DestDir: "{app}\shop"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\ahuni.ttf"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\ahuni.ttf"; DestDir: "{app}\cookingprogress"; Components: cookingprogress; Flags: ignoreversion
Source: "{#StagingDir}\templates\*"; DestDir: "{app}\frontdesk\templates"; Components: frontdesk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\templates\*"; DestDir: "{app}\shop\templates"; Components: shop; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\templates\*"; DestDir: "{app}\waiter\templates"; Components: waiter; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\templates\*"; DestDir: "{app}\cookingprogress\templates"; Components: cookingprogress; Flags: ignoreversion recursesubdirs createallsubdirs

; --- Qt / OpenSSL / MariaDB (per module) ---
Source: "{#StagingDir}\Qt6*.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\Qt6*.dll"; DestDir: "{app}\shop"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\Qt6*.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\Qt6*.dll"; DestDir: "{app}\cookingprogress"; Components: cookingprogress; Flags: ignoreversion
Source: "{#StagingDir}\libcrypto-3-x64.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\libcrypto-3-x64.dll"; DestDir: "{app}\shop"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\libcrypto-3-x64.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\libcrypto-3-x64.dll"; DestDir: "{app}\cookingprogress"; Components: cookingprogress; Flags: ignoreversion
Source: "{#StagingDir}\libssl-3-x64.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\libssl-3-x64.dll"; DestDir: "{app}\shop"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\libssl-3-x64.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\libssl-3-x64.dll"; DestDir: "{app}\cookingprogress"; Components: cookingprogress; Flags: ignoreversion
Source: "{#StagingDir}\libmariadb.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\libmariadb.dll"; DestDir: "{app}\shop"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\libmariadb.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\libmariadb.dll"; DestDir: "{app}\cookingprogress"; Components: cookingprogress; Flags: ignoreversion
Source: "{#StagingDir}\libzkfp.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\libzkfp.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\ZKFPCap.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\ZKFPCap.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\fpslib.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\fpslib.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\libzksensorcore.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\libzksensorcore.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\libusb0.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\libusb0.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\zkfpslibLow.dll"; DestDir: "{app}\frontdesk"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\zkfpslibLow.dll"; DestDir: "{app}\waiter"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\ZKFPSensors\*"; DestDir: "{app}\frontdesk\ZKFPSensors"; Components: frontdesk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\ZKFPSensors\*"; DestDir: "{app}\waiter\ZKFPSensors"; Components: waiter; Flags: ignoreversion recursesubdirs createallsubdirs

; --- Qt plugins (per module) ---
Source: "{#StagingDir}\platforms\*"; DestDir: "{app}\frontdesk\platforms"; Components: frontdesk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\platforms\*"; DestDir: "{app}\shop\platforms"; Components: shop; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\platforms\*"; DestDir: "{app}\waiter\platforms"; Components: waiter; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\platforms\*"; DestDir: "{app}\cookingprogress\platforms"; Components: cookingprogress; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\sqldrivers\*"; DestDir: "{app}\frontdesk\sqldrivers"; Components: frontdesk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\sqldrivers\*"; DestDir: "{app}\shop\sqldrivers"; Components: shop; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\sqldrivers\*"; DestDir: "{app}\waiter\sqldrivers"; Components: waiter; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\sqldrivers\*"; DestDir: "{app}\cookingprogress\sqldrivers"; Components: cookingprogress; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\imageformats\*"; DestDir: "{app}\frontdesk\imageformats"; Components: frontdesk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\imageformats\*"; DestDir: "{app}\shop\imageformats"; Components: shop; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\imageformats\*"; DestDir: "{app}\waiter\imageformats"; Components: waiter; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\imageformats\*"; DestDir: "{app}\cookingprogress\imageformats"; Components: cookingprogress; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\plugins\*"; DestDir: "{app}\frontdesk\plugins"; Components: frontdesk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\plugins\*"; DestDir: "{app}\shop\plugins"; Components: shop; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\plugins\*"; DestDir: "{app}\waiter\plugins"; Components: waiter; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\plugins\*"; DestDir: "{app}\cookingprogress\plugins"; Components: cookingprogress; Flags: ignoreversion recursesubdirs createallsubdirs

; --- VC++ Redistributable (MSVC 2015-2022 x64); extracted in PrepareToInstall when needed ---
Source: "{#StagingDir}\vc_redist.x64.exe"; Flags: dontcopy

[Icons]
; Start menu: on upgrade do not recreate shortcuts for already installed apps (preserves user edits).
; Desktop icons: always honor the task checkbox (even on upgrade).
Name: "{group}\FrontDesk"; Filename: "{app}\frontdesk\OfficeN.exe"; Components: frontdesk; Check: ShouldCreateAppIcon('frontdesk')
Name: "{group}\Shop"; Filename: "{app}\shop\Shop_net.exe"; Components: shop; Check: ShouldCreateAppIcon('shop')
Name: "{group}\Waiter"; Filename: "{app}\waiter\Waiter.exe"; Components: waiter; Check: ShouldCreateAppIcon('waiter')
Name: "{group}\WaiterDesigner"; Filename: "{app}\waiter\WaiterDesigner.exe"; Components: waiter
Name: "{group}\CookingProgress"; Filename: "{app}\cookingprogress\CookingProgress.exe"; Components: cookingprogress; Check: ShouldCreateAppIcon('cookingprogress')
Name: "{group}\Service5 Monitor"; Filename: "{app}\service5.exe"; Parameters: "--gui"; Components: service5; Check: ShouldCreateAppIcon('service5')
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\FrontDesk"; Filename: "{app}\frontdesk\OfficeN.exe"; Tasks: desktop_frontdesk
Name: "{autodesktop}\Shop"; Filename: "{app}\shop\Shop_net.exe"; Tasks: desktop_shop
Name: "{autodesktop}\Waiter"; Filename: "{app}\waiter\Waiter.exe"; Tasks: desktop_waiter
Name: "{autodesktop}\WaiterDesigner"; Filename: "{app}\waiter\WaiterDesigner.exe"; Components: waiter; Tasks: desktop_waiter_designer
Name: "{autodesktop}\CookingProgress"; Filename: "{app}\cookingprogress\CookingProgress.exe"; Tasks: desktop_cookingprogress

[Registry]
Root: HKLM; Subkey: "Software\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}\updater"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}\updater"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}\frontdesk"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}\frontdesk"; Components: frontdesk; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}\shop"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}\shop"; Components: shop; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}\waiter"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}\waiter"; Components: waiter; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}\cookingprogress"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}\cookingprogress"; Components: cookingprogress; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}\service5"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Components: service5; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}"; ValueType: dword; ValueName: "Service5Installed"; ValueData: "1"; Components: service5; Flags: uninsdeletekey

; Demo connection defaults (only if not already set)
; FrontDesk
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\FRONTDESK"; ValueType: string; ValueName: "ss_server_address"; ValueData: "development.picassocloud.com"; Components: frontdesk; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\FRONTDESK"; ValueType: dword; ValueName: "ss_secure_connection"; ValueData: "1"; Components: frontdesk; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\FRONTDESK"; ValueType: string; ValueName: "ss_server_key"; ValueData: "9931cfc3-f75e-11f0-ad11-020462cc89"; Components: frontdesk; Flags: createvalueifdoesntexist
; Shop
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: string; ValueName: "ss_server_address"; ValueData: "development.picassocloud.com"; Components: shop; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: dword; ValueName: "ss_secure_connection"; ValueData: "1"; Components: shop; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: string; ValueName: "ss_server_key"; ValueData: "9931cfc3-f75e-11f0-ad11-020462cc89"; Components: shop; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: string; ValueName: "ss_server_username"; ValueData: "0001"; Components: shop; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: string; ValueName: "ss_server_password"; ValueData: "0001"; Components: shop; Flags: createvalueifdoesntexist
; Waiter
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: string; ValueName: "ss_server_address"; ValueData: "development.picassocloud.com"; Components: waiter; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: dword; ValueName: "ss_secure_connection"; ValueData: "1"; Components: waiter; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: string; ValueName: "ss_server_key"; ValueData: "9931cfc3-f75e-11f0-ad11-020462cc89"; Components: waiter; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: string; ValueName: "ss_server_username"; ValueData: "0001"; Components: waiter; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: string; ValueName: "ss_server_password"; ValueData: "0001"; Components: waiter; Flags: createvalueifdoesntexist
; CookingProgress
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: string; ValueName: "ss_server_address"; ValueData: "development.picassocloud.com"; Components: cookingprogress; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: dword; ValueName: "ss_secure_connection"; ValueData: "1"; Components: cookingprogress; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: string; ValueName: "ss_server_key"; ValueData: "9931cfc3-f75e-11f0-ad11-020462cc89"; Components: cookingprogress; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: string; ValueName: "ss_server_username"; ValueData: "0001"; Components: cookingprogress; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: string; ValueName: "ss_server_password"; ValueData: "0001"; Components: cookingprogress; Flags: createvalueifdoesntexist
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "CookingProgress"; ValueData: """{app}\cookingprogress\CookingProgress.exe"""; Components: cookingprogress; Flags: uninsdeletevalue

[Code]
var
  UpgradeMode: Boolean;
  PrevHadFrontDesk: Boolean;
  PrevHadShop: Boolean;
  PrevHadWaiter: Boolean;
  PrevHadCookingProgress: Boolean;
  PrevHadService5: Boolean;
  BreezeStoppedForUpdate: Boolean;

function NeedSharedRuntime: Boolean;
begin
  Result := WizardIsComponentSelected('frontdesk') or
            WizardIsComponentSelected('shop') or
            WizardIsComponentSelected('waiter') or
            WizardIsComponentSelected('cookingprogress') or
            WizardIsComponentSelected('service5');
end;

{ Qt 6 / MSVC2022 builds need VC++ 2015-2022 x64 runtime (14.30+). }
function IsVCRedistInstalled: Boolean;
var
  Installed, Major, Minor: Cardinal;
begin
  Result := False;
  if not RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
       'Installed', Installed) then
    Exit;
  if Installed <> 1 then
    Exit;
  if not RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
       'Major', Major) then
    Exit;
  if not RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
       'Minor', Minor) then
    Exit;
  { VS 2022 redistributable is 14.30+ }
  Result := (Major > 14) or ((Major = 14) and (Minor >= 30));
end;

function VCRedistNeedsInstall: Boolean;
begin
  Result := not IsVCRedistInstalled;
end;

function InstallVCRedist(var NeedsRestart: Boolean): String;
var
  ResultCode: Integer;
  RedistPath: String;
begin
  Result := '';
  NeedsRestart := False;
  if not VCRedistNeedsInstall then
    Exit;

  ExtractTemporaryFile('vc_redist.x64.exe');
  RedistPath := ExpandConstant('{tmp}\vc_redist.x64.exe');
  if not FileExists(RedistPath) then
  begin
    Result := 'Не найден VC++ Redistributable (vc_redist.x64.exe).';
    Exit;
  end;

  WizardForm.StatusLabel.Caption := 'Установка Visual C++ Redistributable...';
  if not Exec(RedistPath, '/install /quiet /norestart', '', SW_HIDE,
              ewWaitUntilTerminated, ResultCode) then
  begin
    Result := 'Не удалось запустить установку Visual C++ Redistributable.';
    Exit;
  end;

  { 0 = OK, 1638 = newer already installed, 3010 = OK reboot required }
  if (ResultCode = 0) or (ResultCode = 1638) then
    Exit;
  if ResultCode = 3010 then
  begin
    NeedsRestart := True;
    Exit;
  end;

  Result := 'Ошибка установки Visual C++ Redistributable (код ' + IntToStr(ResultCode) + ').';
end;

function NeedZkfpDll: Boolean;
begin
  Result := WizardIsComponentSelected('frontdesk') or
            WizardIsComponentSelected('waiter');
end;

{ Create Start Menu / desktop icons only on fresh install or for newly added components. }
function ShouldCreateAppIcon(const CompName: String): Boolean;
begin
  if not WizardIsComponentSelected(CompName) then
  begin
    Result := False;
    Exit;
  end;

  if not UpgradeMode then
  begin
    Result := True;
    Exit;
  end;

  { Upgrade: skip icons for components that were already installed }
  if CompName = 'frontdesk' then
    Result := not PrevHadFrontDesk
  else if CompName = 'shop' then
    Result := not PrevHadShop
  else if CompName = 'waiter' then
    Result := not PrevHadWaiter
  else if CompName = 'cookingprogress' then
    Result := not PrevHadCookingProgress
  else if CompName = 'service5' then
    Result := not PrevHadService5
  else
    Result := True;
end;

function DetectPreviousComponents: Boolean;
var
  InstallPath: String;
begin
  PrevHadFrontDesk := False;
  PrevHadShop := False;
  PrevHadWaiter := False;
  PrevHadCookingProgress := False;
  PrevHadService5 := False;

  InstallPath := '';
  if not RegQueryStringValue(HKLM, 'Software\{#MyAppName}', 'InstallPath', InstallPath) then
  begin
    Result := False;
    Exit;
  end;
  if (InstallPath = '') or (not DirExists(InstallPath)) then
  begin
    Result := False;
    Exit;
  end;

  if InstallPath[Length(InstallPath)] <> '\' then
    InstallPath := InstallPath + '\';

  PrevHadFrontDesk := FileExists(InstallPath + 'frontdesk\OfficeN.exe') or
                      FileExists(InstallPath + 'OfficeN.exe');
  PrevHadShop := FileExists(InstallPath + 'shop\Shop_net.exe') or
                 FileExists(InstallPath + 'Shop_net.exe');
  PrevHadWaiter := FileExists(InstallPath + 'waiter\Waiter.exe') or
                   FileExists(InstallPath + 'Waiter.exe');
  PrevHadCookingProgress := FileExists(InstallPath + 'cookingprogress\CookingProgress.exe') or
                            FileExists(InstallPath + 'CookingProgress.exe');
  PrevHadService5 := FileExists(InstallPath + 'service5.exe') or
                     FileExists(InstallPath + 'Service5.exe');
  Result := True;
end;

function ServiceExists: Boolean;
var
  ResultCode: Integer;
begin
  if Exec(ExpandConstant('{sys}\sc.exe'), 'query Breeze', '', SW_HIDE,
          ewWaitUntilTerminated, ResultCode) then
    Result := (ResultCode = 0)
  else
    Result := False;
end;

function IsProcessRunning(const ExeName: string): Boolean;
var
  WbemLocator, WmiService, Processes: Variant;
begin
  Result := False;
  try
    WbemLocator := CreateOleObject('WbemScripting.SWbemLocator');
    WmiService := WbemLocator.ConnectServer('', 'root\cimv2', '', '');
    Processes := WmiService.ExecQuery(
      'SELECT Name FROM Win32_Process WHERE Name="' + ExeName + '"');
    Result := (Processes.Count > 0);
  except
    Result := False;
  end;
end;

function StopBreezeService: Boolean;
var
  ResultCode: Integer;
  Tries: Integer;
begin
  if not ServiceExists then
  begin
    Result := True;
    Exit;
  end;
  BreezeStoppedForUpdate := True;
  Result := Exec(ExpandConstant('{sys}\sc.exe'), 'stop Breeze', '', SW_HIDE,
                 ewWaitUntilTerminated, ResultCode);
  for Tries := 1 to 30 do
  begin
    if not IsProcessRunning('service5.exe') and not IsProcessRunning('Service5.exe') then
      Break;
    Sleep(400);
  end;
end;

function StartBreezeService: Boolean;
var
  ResultCode: Integer;
  Tries: Integer;
begin
  if not ServiceExists then
  begin
    Result := False;
    Exit;
  end;
  for Tries := 1 to 40 do
  begin
    if not IsProcessRunning('service5.exe') and not IsProcessRunning('Service5.exe') then
      Break;
    Sleep(500);
  end;
  Sleep(500);
  for Tries := 1 to 8 do
  begin
    if Exec(ExpandConstant('{sys}\sc.exe'), 'start Breeze', '', SW_HIDE,
            ewWaitUntilTerminated, ResultCode) then
    begin
      if (ResultCode = 0) or (ResultCode = 1056) then
      begin
        Result := True;
        Exit;
      end;
    end;
    Sleep(1000);
  end;
  Result := False;
end;

function CreateBreezeService: Boolean;
var
  ResultCode: Integer;
  ExePath: string;
  Params: string;
begin
  ExePath := ExpandConstant('{app}\{#ServiceExe}');
  { Must run after all DLLs are installed. Prefer native --install (CreateService API). }
  if Exec(ExePath, '--install', ExpandConstant('{app}'), SW_HIDE,
          ewWaitUntilTerminated, ResultCode) then
  begin
    if ServiceExists then
    begin
      Result := True;
      Exit;
    end;
  end;

  { Fallback: sc create — entire binPath must be one quoted string when path has spaces }
  Params := 'create Breeze binPath= "' + ExePath + '" start= auto DisplayName= "Breeze"';
  Result := Exec(ExpandConstant('{sys}\sc.exe'), Params, '', SW_HIDE,
                 ewWaitUntilTerminated, ResultCode)
           and ServiceExists;
end;

function EnsureConfigIni: Boolean;
var
  ConfigPath: string;
begin
  ConfigPath := ExpandConstant('{app}\config.ini');
  if FileExists(ConfigPath) then
  begin
    Result := True;
    Exit;
  end;
  Result := SaveStringToFile(ConfigPath,
    '[server]' + #13#10 +
    'port=10002' + #13#10,
    False);
end;

function DeleteBreezeService: Boolean;
var
  ResultCode: Integer;
begin
  StopBreezeService;
  if not ServiceExists then
  begin
    Result := True;
    Exit;
  end;
  Result := Exec(ExpandConstant('{sys}\sc.exe'), 'delete Breeze', '', SW_HIDE,
                 ewWaitUntilTerminated, ResultCode);
end;

function CollectInstalledProducts: string;
var
  InstallPath: string;
begin
  Result := '';
  InstallPath := '';
  if not RegQueryStringValue(HKLM, 'Software\{#MyAppName}', 'InstallPath', InstallPath) then
  begin
    { Component-only installs write Software\Picasso\<component>\InstallPath }
    if not RegQueryStringValue(HKLM, 'Software\{#MyAppName}\frontdesk', 'InstallPath', InstallPath) then
      if not RegQueryStringValue(HKLM, 'Software\{#MyAppName}\shop', 'InstallPath', InstallPath) then
        if not RegQueryStringValue(HKLM, 'Software\{#MyAppName}\waiter', 'InstallPath', InstallPath) then
          if not RegQueryStringValue(HKLM, 'Software\{#MyAppName}\cookingprogress', 'InstallPath', InstallPath) then
            if not RegQueryStringValue(HKLM, 'Software\{#MyAppName}\service5', 'InstallPath', InstallPath) then
              InstallPath := ExpandConstant('{autopf}\{#MyAppName}');
  end;
  if (InstallPath = '') or (not DirExists(InstallPath)) then
    Exit;
  if InstallPath[Length(InstallPath)] <> '\' then
    InstallPath := InstallPath + '\';

  if FileExists(InstallPath + 'OfficeN.exe') then
    Result := Result + '  - FrontDesk' + #13#10;
  if FileExists(InstallPath + 'Shop_net.exe') then
    Result := Result + '  - Shop' + #13#10;
  if FileExists(InstallPath + 'Waiter.exe') then
    Result := Result + '  - Waiter' + #13#10;
  if FileExists(InstallPath + 'CookingProgress.exe') then
    Result := Result + '  - CookingProgress' + #13#10;
  if FileExists(InstallPath + 'service5.exe') or FileExists(InstallPath + 'Service5.exe') or ServiceExists then
    Result := Result + '  - Service5 / Breeze service' + #13#10;
end;

function CollectRunningApps: string;
var
  Names: array[0..5] of string;
  Labels: array[0..5] of string;
  I: Integer;
begin
  Names[0] := 'OfficeN.exe';
  Names[1] := 'Shop_net.exe';
  Names[2] := 'Waiter.exe';
  Names[3] := 'WaiterDesigner.exe';
  Names[4] := 'CookingProgress.exe';
  Names[5] := 'service5.exe';
  Labels[0] := 'FrontDesk (OfficeN.exe)';
  Labels[1] := 'Shop (Shop_net.exe)';
  Labels[2] := 'Waiter (Waiter.exe)';
  Labels[3] := 'WaiterDesigner (WaiterDesigner.exe)';
  Labels[4] := 'CookingProgress (CookingProgress.exe)';
  Labels[5] := 'Service5 (service5.exe)';

  Result := '';
  for I := 0 to 5 do
  begin
    if IsProcessRunning(Names[I]) then
    begin
      if Result <> '' then
        Result := Result + #13#10;
      Result := Result + '  - ' + Labels[I];
    end;
  end;
  if ServiceExists and (Pos('Service5', Result) = 0) then
  begin
    if Result <> '' then
      Result := Result + #13#10;
    Result := Result + '  - служба Windows «Breeze» (Service5)';
  end;
end;

procedure KillProcess(const ExeName: string);
var
  ResultCode: Integer;
  Tries: Integer;
begin
  for Tries := 1 to 15 do
  begin
    if not IsProcessRunning(ExeName) then
      Exit;
    { /T = kill process tree; apps often keep helper children locking DLLs }
    Exec(ExpandConstant('{sys}\taskkill.exe'), '/F /T /IM "' + ExeName + '"', '',
         SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Sleep(400);
  end;
end;

procedure ClosePicassoApps;
begin
  { Stop service first — otherwise SCM may restart service5.exe }
  StopBreezeService;
  Sleep(500);
  KillProcess('OfficeN.exe');
  KillProcess('Shop_net.exe');
  KillProcess('Waiter.exe');
  KillProcess('WaiterDesigner.exe');
  KillProcess('CookingProgress.exe');
  KillProcess('service5.exe');
  KillProcess('Service5.exe');
  Sleep(500);
end;

procedure CleanupAppRegistry;
begin
  { Connection / QSettings trees written by the apps }
  RegDeleteKeyIncludingSubkeys(HKCU, 'Software\Jazzve\Cafe5\FRONTDESK');
  RegDeleteKeyIncludingSubkeys(HKCU, 'Software\Jazzve\Cafe5\waiter');
  RegDeleteKeyIncludingSubkeys(HKCU, 'Software\BreezeDevs\Shop\Shop');
  RegDeleteKeyIncludingSubkeys(HKCU, 'Software\BreezeDevs\CookingProgress\CookingProgress');
  RegDeleteKeyIfEmpty(HKCU, 'Software\Jazzve\Cafe5');
  RegDeleteKeyIfEmpty(HKCU, 'Software\Jazzve');
  RegDeleteKeyIfEmpty(HKCU, 'Software\BreezeDevs\Shop');
  RegDeleteKeyIfEmpty(HKCU, 'Software\BreezeDevs\CookingProgress');
  RegDeleteKeyIfEmpty(HKCU, 'Software\BreezeDevs');
  { Installer metadata }
  RegDeleteKeyIncludingSubkeys(HKLM, 'Software\{#MyAppName}');
end;

procedure BeforeServiceInstall;
begin
  if WizardIsComponentSelected('service5') then
    StopBreezeService;
end;

procedure EnsureBreezeService;
begin
  if WizardIsComponentSelected('service5') then
  begin
    EnsureConfigIni;

    if not ServiceExists then
    begin
      if not CreateBreezeService then
      begin
        if not WizardSilent then
          MsgBox('Не удалось создать службу Windows «Breeze».' + #13#10 +
                 'Запустите от имени администратора:' + #13#10 +
                 ExpandConstant('{app}\{#ServiceExe}') + ' --install',
                 mbError, MB_OK);
        Exit;
      end;
    end;

    if not StartBreezeService then
    begin
      if not WizardSilent then
        MsgBox('Служба «Breeze» создана, но не запустилась.' + #13#10 +
               'Проверьте config.ini и зависимости в папке установки.',
               mbInformation, MB_OK);
    end;
    Exit;
  end;

  { Other components selected: we may have stopped Breeze to unlock Qt DLLs. }
  if BreezeStoppedForUpdate and ServiceExists then
  begin
    if not StartBreezeService then
    begin
      if not WizardSilent then
        MsgBox('Служба «Breeze» (Service5) была остановлена для обновления,' + #13#10 +
               'но не запустилась снова. Запустите её вручную в services.msc.',
               mbInformation, MB_OK);
    end;
  end;
end;

function InitializeSetup: Boolean;
var
  Running: string;
  Installed: string;
  Msg: string;
begin
  BreezeStoppedForUpdate := False;
  UpgradeMode := RegKeyExists(HKLM, 'Software\{#MyAppName}');
  if UpgradeMode then
    DetectPreviousComponents;

  Running := CollectRunningApps;
  Installed := CollectInstalledProducts;

  if (Running <> '') or ((Installed <> '') and UpgradeMode) then
  begin
    { Auto-update (/VERYSILENT): close apps without asking }
    if not WizardSilent then
    begin
      if UpgradeMode then
        Msg := 'Обновление Picasso.' + #13#10#13#10
      else
        Msg := 'Установка Picasso.' + #13#10#13#10;

      if Installed <> '' then
        Msg := Msg + 'На этом ПК уже установлено:' + #13#10 + Installed + #13#10;

      if Running <> '' then
        Msg := Msg + 'Сейчас запущено:' + #13#10 + Running + #13#10#13#10
      else
        Msg := Msg + #13#10;

      Msg := Msg + 'Запущенные приложения будут закрыты перед продолжением.' + #13#10;
      if ServiceExists then
        Msg := Msg + 'Служба Breeze (Service5) будет остановлена и после установки запущена снова.' + #13#10;
      Msg := Msg + #13#10 + 'Продолжить?';

      if MsgBox(Msg, mbConfirmation, MB_YESNO or MB_DEFBUTTON2) = IDNO then
      begin
        Result := False;
        Exit;
      end;
    end;
    { Close immediately after confirmation — before wizard / file copy }
    ClosePicassoApps;
  end;

  Result := True;
end;

function InitializeUninstall: Boolean;
var
  Running: string;
begin
  Running := CollectRunningApps;
  if Running <> '' then
  begin
    if MsgBox('Удаление Picasso.' + #13#10#13#10 +
              'Запущены приложения:' + #13#10 + Running + #13#10#13#10 +
              'Они будут закрыты перед удалением.' + #13#10 +
              'Продолжить?', mbConfirmation, MB_YESNO or MB_DEFBUTTON2) = IDNO then
    begin
      Result := False;
      Exit;
    end;
    ClosePicassoApps;
  end;
  Result := True;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssInstall then
    ClosePicassoApps;
  { Create/start after ALL files (Qt DLLs etc.) are on disk }
  if CurStep = ssPostInstall then
    EnsureBreezeService;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
  begin
    ClosePicassoApps;
    if RegValueExists(HKLM, 'Software\{#MyAppName}', 'Service5Installed') then
      DeleteBreezeService;
    CleanupAppRegistry;
  end;
  { After files are removed, force-delete the install dir if leftovers remain }
  if CurUninstallStep = usPostUninstall then
  begin
    DelTree(ExpandConstant('{app}'), True, True, True);
  end;
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
  NeedsRestart := False;
  { Kill apps before file copy — ssInstall is too late (Files in Use / Retry). }
  ClosePicassoApps;
  Result := InstallVCRedist(NeedsRestart);
  if Result = '' then
    ClosePicassoApps;
end;

[UninstallDelete]
; Remove leftover runtime files (config.ini, logs, DLLs not tracked by Inno) and the install dir itself
Type: filesandordirs; Name: "{app}\platforms"
Type: filesandordirs; Name: "{app}\sqldrivers"
Type: filesandordirs; Name: "{app}\imageformats"
Type: filesandordirs; Name: "{app}\plugins"
Type: filesandordirs; Name: "{app}\templates"
Type: filesandordirs; Name: "{app}"

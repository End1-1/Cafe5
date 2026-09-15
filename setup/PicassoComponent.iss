; Single-component Picasso installer
; Compiled via build_installer.bat / stage with component_defines.iss

#include "version.iss"
#include "component_defines.iss"

#define MyAppName "Picasso"
#define MyAppPublisher "BreezeDevs"
#define MyAppURL "https://www.breezedevs.com"
#define StagingDir "staging"
#define ServiceExe "service5.exe"

[Setup]
AppId={#ComponentAppId}
AppName={#ComponentDisplayName}
AppVersion={#MyAppVersionShort}
AppVerName={#ComponentDisplayName} {#MyAppVersionShort}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
#if ComponentKey == "service5"
DefaultDirName={autopf}\{#MyAppName}
#else
DefaultDirName={code:ComponentTargetInstallDir}
#endif
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir={#ComponentOutputDir}
OutputBaseFilename={#ComponentOutputName}
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
UsePreviousAppDir=no
CloseApplications=force
CloseApplicationsFilter={#ComponentCloseFilter},Updater.exe,PicassoUpdateHost.exe
RestartApplications=no
ShowLanguageDialog=auto

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
#if ComponentKey == "frontdesk"
Name: "desktopicon"; Description: "Desktop icon: FrontDesk"; GroupDescription: "Desktop icons:"; Flags: unchecked
#endif
#if ComponentKey == "shop"
Name: "desktopicon"; Description: "Desktop icon: Shop"; GroupDescription: "Desktop icons:"; Flags: unchecked
#endif
#if ComponentKey == "waiter"
Name: "desktopicon"; Description: "Desktop icon: Waiter"; GroupDescription: "Desktop icons:"; Flags: unchecked
Name: "desktopicon_designer"; Description: "Desktop icon: WaiterDesigner"; GroupDescription: "Desktop icons:"
#endif
#if ComponentKey == "cookingprogress"
Name: "desktopicon"; Description: "Desktop icon: CookingProgress"; GroupDescription: "Desktop icons:"; Flags: unchecked
#endif

[Files]
#if ComponentKey == "frontdesk"
Source: "{#StagingDir}\OfficeN.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\officestyle.css"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libzkfp.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\ZKFPCap.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\fpslib.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libzksensorcore.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libusb0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\zkfpslibLow.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\ZKFPSensors\*"; DestDir: "{app}\ZKFPSensors"; Flags: ignoreversion recursesubdirs createallsubdirs
#endif
#if ComponentKey == "shop"
Source: "{#StagingDir}\Shop_net.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\shop.css"; DestDir: "{app}"; Flags: ignoreversion
#endif
#if ComponentKey == "waiter"
Source: "{#StagingDir}\Waiter.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\WaiterDesigner.exe"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#StagingDir}\waiter.css"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libzkfp.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\ZKFPCap.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\fpslib.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libzksensorcore.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libusb0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\zkfpslibLow.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\ZKFPSensors\*"; DestDir: "{app}\ZKFPSensors"; Flags: ignoreversion recursesubdirs createallsubdirs
#endif
#if ComponentKey == "cookingprogress"
Source: "{#StagingDir}\CookingProgress.exe"; DestDir: "{app}"; Flags: ignoreversion
#endif
; Fresh shared updater in every module installer (overwrite on each update).
Source: "{#StagingDir}\Updater.exe"; DestDir: "{code:PicassoUpdaterInstallDir}"; Flags: ignoreversion
Source: "{#StagingDir}\Qt6*.dll"; DestDir: "{code:PicassoUpdaterInstallDir}"; Flags: ignoreversion
Source: "{#StagingDir}\platforms\*"; DestDir: "{code:PicassoUpdaterInstallDir}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
#if ComponentKey == "service5"
Source: "{#StagingDir}\service5.exe"; DestDir: "{app}"; Flags: ignoreversion; BeforeInstall: BeforeServiceInstall
#endif

; Shared runtime
Source: "{#StagingDir}\ahuni.ttf"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\templates\*"; DestDir: "{app}\templates"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\Qt6*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libcrypto-3-x64.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libssl-3-x64.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\libmariadb.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StagingDir}\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\plugins\*"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\vc_redist.x64.exe"; Flags: dontcopy

[Icons]
#if ComponentKey == "frontdesk"
Name: "{group}\FrontDesk"; Filename: "{app}\OfficeN.exe"; Check: ShouldCreateShortcut
Name: "{autodesktop}\FrontDesk"; Filename: "{app}\OfficeN.exe"; Tasks: desktopicon
#endif
#if ComponentKey == "shop"
Name: "{group}\Shop"; Filename: "{app}\Shop_net.exe"; Check: ShouldCreateShortcut
Name: "{autodesktop}\Shop"; Filename: "{app}\Shop_net.exe"; Tasks: desktopicon
#endif
#if ComponentKey == "waiter"
Name: "{group}\Waiter"; Filename: "{app}\Waiter.exe"; Check: ShouldCreateShortcut
Name: "{group}\WaiterDesigner"; Filename: "{app}\WaiterDesigner.exe"
Name: "{autodesktop}\Waiter"; Filename: "{app}\Waiter.exe"; Tasks: desktopicon
Name: "{autodesktop}\WaiterDesigner"; Filename: "{app}\WaiterDesigner.exe"; Tasks: desktopicon_designer
#endif
#if ComponentKey == "cookingprogress"
Name: "{group}\CookingProgress"; Filename: "{app}\CookingProgress.exe"; Check: ShouldCreateShortcut
Name: "{autodesktop}\CookingProgress"; Filename: "{app}\CookingProgress.exe"; Tasks: desktopicon
#endif
#if ComponentKey == "service5"
Name: "{group}\Service5 Monitor"; Filename: "{app}\service5.exe"; Parameters: "--gui"; Check: ShouldCreateShortcut
#endif
Name: "{group}\{cm:UninstallProgram,{#ComponentDisplayName}}"; Filename: "{uninstallexe}"

[Registry]
Root: HKLM; Subkey: "Software\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{autopf}\{#MyAppName}"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "Software\{#MyAppName}\updater"; ValueType: string; ValueName: "InstallPath"; ValueData: "{code:PicassoUpdaterInstallDir}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}\{#ComponentKey}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}\{#ComponentKey}"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersionShort}"; Flags: uninsdeletekey
#if ComponentKey == "service5"
Root: HKLM; Subkey: "Software\{#MyAppName}"; ValueType: dword; ValueName: "Service5Installed"; ValueData: "1"; Flags: uninsdeletevalue
#endif

; Demo connection defaults (only if not already set)
#if ComponentKey == "frontdesk"
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\FRONTDESK"; ValueType: string; ValueName: "ss_server_address"; ValueData: "development.picassocloud.com"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\FRONTDESK"; ValueType: dword; ValueName: "ss_secure_connection"; ValueData: "1"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\FRONTDESK"; ValueType: string; ValueName: "ss_server_key"; ValueData: "9931cfc3-f75e-11f0-ad11-020462cc89"; Flags: createvalueifdoesntexist
#endif
#if ComponentKey == "shop"
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: string; ValueName: "ss_server_address"; ValueData: "development.picassocloud.com"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: dword; ValueName: "ss_secure_connection"; ValueData: "1"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: string; ValueName: "ss_server_key"; ValueData: "9931cfc3-f75e-11f0-ad11-020462cc89"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: string; ValueName: "ss_server_username"; ValueData: "0001"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\Shop\Shop"; ValueType: string; ValueName: "ss_server_password"; ValueData: "0001"; Flags: createvalueifdoesntexist
#endif
#if ComponentKey == "waiter"
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: string; ValueName: "ss_server_address"; ValueData: "development.picassocloud.com"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: dword; ValueName: "ss_secure_connection"; ValueData: "1"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: string; ValueName: "ss_server_key"; ValueData: "9931cfc3-f75e-11f0-ad11-020462cc89"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: string; ValueName: "ss_server_username"; ValueData: "0001"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\Jazzve\Cafe5\waiter"; ValueType: string; ValueName: "ss_server_password"; ValueData: "0001"; Flags: createvalueifdoesntexist
#endif
#if ComponentKey == "cookingprogress"
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: string; ValueName: "ss_server_address"; ValueData: "development.picassocloud.com"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: dword; ValueName: "ss_secure_connection"; ValueData: "1"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: string; ValueName: "ss_server_key"; ValueData: "9931cfc3-f75e-11f0-ad11-020462cc89"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: string; ValueName: "ss_server_username"; ValueData: "0001"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\BreezeDevs\CookingProgress\CookingProgress"; ValueType: string; ValueName: "ss_server_password"; ValueData: "0001"; Flags: createvalueifdoesntexist
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "CookingProgress"; ValueData: """{app}\CookingProgress.exe"""; Flags: uninsdeletevalue
#endif

[Code]
var
  UpgradeMode: Boolean;
  BreezeStoppedForUpdate: Boolean;

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

  if (ResultCode = 0) or (ResultCode = 1638) then
    Exit;
  if ResultCode = 3010 then
  begin
    NeedsRestart := True;
    Exit;
  end;

  Result := 'Ошибка установки Visual C++ Redistributable (код ' + IntToStr(ResultCode) + ').';
end;

function ShouldCreateShortcut: Boolean;
begin
  Result := not UpgradeMode;
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

procedure KillProcess(const ExeName: string);
var
  ResultCode: Integer;
  Tries: Integer;
begin
  for Tries := 1 to 15 do
  begin
    if not IsProcessRunning(ExeName) then
      Exit;
    Exec(ExpandConstant('{sys}\taskkill.exe'), '/F /T /IM "' + ExeName + '"', '',
         SW_HIDE, ewWaitUntilTerminated, ResultCode);
    Sleep(400);
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
  { Wait until stop finishes — starting while STOP_PENDING → access/state errors }
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
      { 0 = started; 1056 = already running }
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
  if Exec(ExePath, '--install', ExpandConstant('{app}'), SW_HIDE,
          ewWaitUntilTerminated, ResultCode) then
  begin
    if ServiceExists then
    begin
      Result := True;
      Exit;
    end;
  end;

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

{ Resolve Picasso root (C:\Program Files\Picasso). }
function PicassoRootDir: string;
begin
  Result := ExpandConstant('{autopf}\{#MyAppName}');
end;

function PicassoUpdaterInstallDir(Param: String): String;
begin
  Result := PicassoRootDir + '\updater';
end;

function ComponentMarkerExe: string;
begin
#if ComponentKey == "frontdesk"
  Result := 'OfficeN.exe';
#elif ComponentKey == "shop"
  Result := 'Shop_net.exe';
#elif ComponentKey == "waiter"
  Result := 'Waiter.exe';
#elif ComponentKey == "cookingprogress"
  Result := 'CookingProgress.exe';
#else
  Result := '';
#endif
end;

{ Target folder for this module. Next update migrates flat legacy install into a subfolder. }
function ComponentTargetInstallDir(Param: String): String;
var
  RegPath, Root, Marker, SubDir: string;
begin
#if ComponentKey == "service5"
  Result := PicassoRootDir;
#else
  SubDir := '{#ComponentKey}';
  Marker := ComponentMarkerExe;
  Root := PicassoRootDir;

  if RegQueryStringValue(HKLM, 'Software\{#MyAppName}\{#ComponentKey}', 'InstallPath', RegPath) and
     (RegPath <> '') and DirExists(RegPath) then
  begin
    if (CompareText(ExtractFileName(RegPath), SubDir) = 0) or
       (not FileExists(RegPath + '\' + Marker)) then
    begin
      Result := RegPath;
      Exit;
    end;
    Result := RegPath + '\' + SubDir;
    Exit;
  end;

  if RegQueryStringValue(HKLM, 'Software\{#MyAppName}', 'InstallPath', RegPath) and
     (RegPath <> '') and DirExists(RegPath) then
    Root := RegPath;

  if Root[Length(Root)] = '\' then
    Delete(Root, Length(Root), 1);

  if FileExists(Root + '\' + Marker) then
    Result := Root + '\' + SubDir
  else
    Result := Root + '\' + SubDir;
#endif
end;

function CollectRunningApps: string;
begin
  Result := '';
#if ComponentKey == "service5"
  if IsProcessRunning('service5.exe') or IsProcessRunning('Service5.exe') then
    Result := '  - Service5 (service5.exe)';
  if ServiceExists and (Pos('Service5', Result) = 0) then
  begin
    if Result <> '' then
      Result := Result + #13#10;
    Result := Result + '  - служба Windows «Breeze» (Service5)';
  end;
#elif ComponentKey == "frontdesk"
  if IsProcessRunning('OfficeN.exe') then
    Result := '  - FrontDesk (OfficeN.exe)';
#elif ComponentKey == "shop"
  if IsProcessRunning('Shop_net.exe') then
    Result := '  - Shop (Shop_net.exe)';
#elif ComponentKey == "waiter"
  if IsProcessRunning('Waiter.exe') then
    Result := '  - Waiter (Waiter.exe)';
  if IsProcessRunning('WaiterDesigner.exe') then
  begin
    if Result <> '' then
      Result := Result + #13#10;
    Result := Result + '  - WaiterDesigner (WaiterDesigner.exe)';
  end;
#elif ComponentKey == "cookingprogress"
  if IsProcessRunning('CookingProgress.exe') then
    Result := '  - CookingProgress (CookingProgress.exe)';
#endif
end;

procedure StopBreezeIfPresent;
begin
  if ServiceExists then
  begin
    BreezeStoppedForUpdate := True;
    StopBreezeService;
  end;
  if IsProcessRunning('service5.exe') or IsProcessRunning('Service5.exe') then
  begin
    if ServiceExists then
      BreezeStoppedForUpdate := True;
    Sleep(300);
    KillProcess('service5.exe');
    KillProcess('Service5.exe');
  end;
end;

procedure CloseComponentApps;
begin
#if ComponentKey == "service5"
  StopBreezeIfPresent;
  Sleep(300);
  KillProcess('service5.exe');
  KillProcess('Service5.exe');
  Sleep(500);
#elif ComponentKey == "frontdesk"
  KillProcess('OfficeN.exe');
#elif ComponentKey == "shop"
  KillProcess('Shop_net.exe');
#elif ComponentKey == "waiter"
  KillProcess('Waiter.exe');
  KillProcess('WaiterDesigner.exe');
#elif ComponentKey == "cookingprogress"
  KillProcess('CookingProgress.exe');
#endif
end;

procedure BeforeServiceInstall;
begin
  StopBreezeIfPresent;
end;

procedure EnsureBreezeService;
begin
#if ComponentKey == "service5"
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
#else
  { Desktop modules are isolated — do not stop Breeze. }
#endif
end;

function InitializeSetup: Boolean;
var
  Running: string;
  Msg: string;
begin
  UpgradeMode := RegKeyExists(HKLM, 'Software\{#MyAppName}\{#ComponentKey}');
  BreezeStoppedForUpdate := False;

  Running := CollectRunningApps;

  if Running <> '' then
  begin
    if not WizardSilent then
    begin
      if UpgradeMode then
        Msg := 'Обновление {#ComponentDisplayName}.' + #13#10#13#10
      else
        Msg := 'Установка {#ComponentDisplayName}.' + #13#10#13#10;

      Msg := Msg + 'Сейчас запущено:' + #13#10 + Running + #13#10#13#10;
#if ComponentKey == "service5"
      if ServiceExists then
        Msg := Msg + 'Служба Breeze (Service5) будет остановлена и после установки запущена снова.' + #13#10;
#endif
      Msg := Msg + 'Перед продолжением нужно закрыть указанные процессы.' + #13#10;
      Msg := Msg + #13#10 + 'Продолжить?';

      if MsgBox(Msg, mbConfirmation, MB_YESNO or MB_DEFBUTTON2) = IDNO then
      begin
        Result := False;
        Exit;
      end;
    end;
    CloseComponentApps;
  end;

  Result := True;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssInstall then
    CloseComponentApps;
  if CurStep = ssPostInstall then
    EnsureBreezeService;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
  begin
#if ComponentKey == "service5"
    CloseComponentApps;
    DeleteBreezeService;
#else
    { Do not stop Breeze when uninstalling other components alone }
#if ComponentKey == "frontdesk"
    KillProcess('OfficeN.exe');
#endif
#if ComponentKey == "shop"
    KillProcess('Shop_net.exe');
#endif
#if ComponentKey == "waiter"
    KillProcess('Waiter.exe');
    KillProcess('WaiterDesigner.exe');
#endif
#if ComponentKey == "cookingprogress"
    KillProcess('CookingProgress.exe');
#endif
#endif
  end;
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
  NeedsRestart := False;
  CloseComponentApps;
  Result := InstallVCRedist(NeedsRestart);
  if Result = '' then
    CloseComponentApps;
end;

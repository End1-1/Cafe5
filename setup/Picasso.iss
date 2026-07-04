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
CloseApplications=yes
CloseApplicationsFilter=OfficeN.exe,Shop_net.exe,Waiter.exe,service5.exe,Service5.exe
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
Name: "service5"; Description: "Service5 (Windows service Breeze)"; Types: full custom

[Tasks]
Name: "desktop_frontdesk"; Description: "Desktop icon: FrontDesk"; GroupDescription: "Desktop icons:"; Components: frontdesk; Flags: unchecked
Name: "desktop_shop"; Description: "Desktop icon: Shop"; GroupDescription: "Desktop icons:"; Components: shop; Flags: unchecked
Name: "desktop_waiter"; Description: "Desktop icon: Waiter"; GroupDescription: "Desktop icons:"; Components: waiter; Flags: unchecked

[Files]
; --- Application binaries ---
Source: "{#StagingDir}\OfficeN.exe"; DestDir: "{app}"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\Shop_net.exe"; DestDir: "{app}"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\Waiter.exe"; DestDir: "{app}"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\service5.exe"; DestDir: "{app}"; Components: service5; Flags: ignoreversion; BeforeInstall: BeforeServiceInstall; AfterInstall: AfterServiceInstall

; --- Styles / assets ---
Source: "{#StagingDir}\officestyle.css"; DestDir: "{app}"; Components: frontdesk; Flags: ignoreversion
Source: "{#StagingDir}\shop.css"; DestDir: "{app}"; Components: shop; Flags: ignoreversion
Source: "{#StagingDir}\waiter.css"; DestDir: "{app}"; Components: waiter; Flags: ignoreversion
Source: "{#StagingDir}\ahuni.ttf"; DestDir: "{app}"; Check: NeedSharedRuntime; Flags: ignoreversion
Source: "{#StagingDir}\templates\*"; DestDir: "{app}\templates"; Check: NeedSharedRuntime; Flags: ignoreversion recursesubdirs createallsubdirs

; --- Qt / OpenSSL / MariaDB ---
Source: "{#StagingDir}\Qt6*.dll"; DestDir: "{app}"; Check: NeedSharedRuntime; Flags: ignoreversion
Source: "{#StagingDir}\libcrypto-3-x64.dll"; DestDir: "{app}"; Check: NeedSharedRuntime; Flags: ignoreversion
Source: "{#StagingDir}\libssl-3-x64.dll"; DestDir: "{app}"; Check: NeedSharedRuntime; Flags: ignoreversion
Source: "{#StagingDir}\libmariadb.dll"; DestDir: "{app}"; Check: NeedSharedRuntime; Flags: ignoreversion
Source: "{#StagingDir}\libzkfp.dll"; DestDir: "{app}"; Check: NeedZkfpDll; Flags: ignoreversion
Source: "{#StagingDir}\ZKFPCap.dll"; DestDir: "{app}"; Check: NeedZkfpDll; Flags: ignoreversion
Source: "{#StagingDir}\fpslib.dll"; DestDir: "{app}"; Check: NeedZkfpDll; Flags: ignoreversion

; --- Qt plugins ---
Source: "{#StagingDir}\platforms\*"; DestDir: "{app}\platforms"; Check: NeedSharedRuntime; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Check: NeedSharedRuntime; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\imageformats\*"; DestDir: "{app}\imageformats"; Check: NeedSharedRuntime; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\plugins\*"; DestDir: "{app}\plugins"; Check: NeedSharedRuntime; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\FrontDesk"; Filename: "{app}\OfficeN.exe"; Components: frontdesk
Name: "{group}\Shop"; Filename: "{app}\Shop_net.exe"; Components: shop
Name: "{group}\Waiter"; Filename: "{app}\Waiter.exe"; Components: waiter
Name: "{group}\Service5 Monitor"; Filename: "{app}\service5.exe"; Parameters: "--gui"; Components: service5
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\FrontDesk"; Filename: "{app}\OfficeN.exe"; Tasks: desktop_frontdesk
Name: "{autodesktop}\Shop"; Filename: "{app}\Shop_net.exe"; Tasks: desktop_shop
Name: "{autodesktop}\Waiter"; Filename: "{app}\Waiter.exe"; Tasks: desktop_waiter

[Registry]
Root: HKLM; Subkey: "Software\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\{#MyAppName}"; ValueType: dword; ValueName: "Service5Installed"; ValueData: "1"; Components: service5; Flags: uninsdeletekey

[Code]
var
  UpgradeMode: Boolean;

function NeedSharedRuntime: Boolean;
begin
  Result := WizardIsComponentSelected('frontdesk') or
            WizardIsComponentSelected('shop') or
            WizardIsComponentSelected('waiter') or
            WizardIsComponentSelected('service5');
end;

function NeedZkfpDll: Boolean;
begin
  Result := WizardIsComponentSelected('frontdesk') or
            WizardIsComponentSelected('waiter');
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

function StopBreezeService: Boolean;
var
  ResultCode: Integer;
begin
  if not ServiceExists then
  begin
    Result := True;
    Exit;
  end;
  Result := Exec(ExpandConstant('{sys}\sc.exe'), 'stop Breeze', '', SW_HIDE,
                 ewWaitUntilTerminated, ResultCode);
end;

function StartBreezeService: Boolean;
var
  ResultCode: Integer;
begin
  if not ServiceExists then
  begin
    Result := False;
    Exit;
  end;
  Result := Exec(ExpandConstant('{sys}\sc.exe'), 'start Breeze', '', SW_HIDE,
                 ewWaitUntilTerminated, ResultCode);
end;

function CreateBreezeService: Boolean;
var
  ResultCode: Integer;
  BinPath: string;
begin
  BinPath := '"' + ExpandConstant('{app}') + '\{#ServiceExe}" --config="' +
             ExpandConstant('{app}') + '\config.ini"';
  Result := Exec(ExpandConstant('{sys}\sc.exe'),
    'create Breeze binPath= ' + BinPath + ' start= auto DisplayName= "Breeze"',
    '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
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

function CollectRunningApps: string;
var
  Names: array[0..3] of string;
  Labels: array[0..3] of string;
  I: Integer;
begin
  Names[0] := 'OfficeN.exe';
  Names[1] := 'Shop_net.exe';
  Names[2] := 'Waiter.exe';
  Names[3] := 'service5.exe';
  Labels[0] := 'FrontDesk (OfficeN.exe)';
  Labels[1] := 'Shop (Shop_net.exe)';
  Labels[2] := 'Waiter (Waiter.exe)';
  Labels[3] := 'Service5 (service5.exe)';

  Result := '';
  for I := 0 to 3 do
  begin
    if IsProcessRunning(Names[I]) then
    begin
      if Result <> '' then
        Result := Result + #13#10;
      Result := Result + '  - ' + Labels[I];
    end;
  end;
end;

procedure BeforeServiceInstall;
begin
  if WizardIsComponentSelected('service5') then
    StopBreezeService;
end;

procedure AfterServiceInstall;
begin
  if not WizardIsComponentSelected('service5') then
    Exit;

  if not ServiceExists then
    CreateBreezeService;
  StartBreezeService;
end;

function InitializeSetup: Boolean;
var
  Running: string;
  Msg: string;
begin
  UpgradeMode := RegKeyExists(HKLM, 'Software\{#MyAppName}');

  Running := CollectRunningApps;
  if Running <> '' then
  begin
    if UpgradeMode then
      Msg := 'Обновление Picasso.' + #13#10#13#10 +
             'Запущены приложения:' + #13#10 + Running + #13#10#13#10 +
             'Установщик попытается закрыть их перед обновлением.' + #13#10 +
             'Продолжить?'
    else
      Msg := 'Запущены приложения:' + #13#10 + Running + #13#10#13#10 +
             'Установщик попытается закрыть их перед установкой.' + #13#10 +
             'Продолжить?';

    if MsgBox(Msg, mbConfirmation, MB_YESNO or MB_DEFBUTTON2) = IDNO then
    begin
      Result := False;
      Exit;
    end;
  end;

  Result := True;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep = ssInstall) and WizardIsComponentSelected('service5') then
    StopBreezeService;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
  begin
    if RegValueExists(HKLM, 'Software\{#MyAppName}', 'Service5Installed') then
      DeleteBreezeService;
  end;
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
  Result := '';
  NeedsRestart := False;
end;

[UninstallDelete]
Type: filesandordirs; Name: "{app}\platforms"
Type: filesandordirs; Name: "{app}\sqldrivers"
Type: filesandordirs; Name: "{app}\imageformats"
Type: filesandordirs; Name: "{app}\plugins"
Type: filesandordirs; Name: "{app}\templates"

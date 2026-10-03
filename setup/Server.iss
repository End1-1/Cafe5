; Picasso web stack: Apache + PHP + MariaDB + web.picassoapp + Service5
; Build: setup\build_server.bat

#define MyAppName "Picasso Web"
#ifndef BuildNumber
#define BuildNumber "1"
#endif
#define MyAppVersion "1.0." + BuildNumber
#define SetupFileName "PicassoWeb_Setup_x64_" + BuildNumber
#define MyAppPublisher "BreezeDevs"
#define StagingDir "staging-server"

[Setup]
AppId={{B7E2A1C4-6D90-4F11-9A33-8C5D2E1F0A11}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName=C:\Picasso\web
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=no
OutputDir=output
OutputBaseFilename={#SetupFileName}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
UsePreviousAppDir=yes
CloseApplications=force
CloseApplicationsFilter=service5.exe,httpd.exe,mysqld.exe

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Files]
Source: "{#StagingDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StagingDir}\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall skipifsourcedoesntexist

[Icons]
Name: "{group}\Picasso site"; Filename: "http://picassoapp.local/"
Name: "{group}\Service5 Monitor"; Filename: "{app}\service5\service5.exe"; Parameters: "--gui"
Name: "{group}\HeidiSQL"; Filename: "{app}\heidisql\heidisql.exe"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Visual C++ runtime"; Flags: waituntilterminated skipifdoesntexist
Filename: "powershell.exe"; Parameters: "-NoProfile -ExecutionPolicy Bypass -File ""{app}\install-services.ps1"" -Root ""{app}"""; StatusMsg: "Starting Apache, MariaDB and Service5"; Flags: waituntilterminated

[UninstallRun]
Filename: "powershell.exe"; Parameters: "-NoProfile -ExecutionPolicy Bypass -File ""{app}\uninstall-services.ps1"" -Root ""{app}"""; Flags: waituntilterminated runhidden

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
  LogFile: String;
  LogText: AnsiString;
begin
  if CurStep <> ssDone then
    Exit;
  if FileExists(ExpandConstant('{app}\install-services.ok')) then
    Exit;
  LogFile := ExpandConstant('{app}\install-services.log');
  LogText := '';
  LoadStringFromFile(LogFile, LogText);
  MsgBox('Apache, MariaDB and Service5 were not registered.' + #13#10 +
    'Log: ' + LogFile + #13#10 + #13#10 + LogText, mbError, MB_OK);
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
var
  ResultCode: Integer;
begin
  Result := '';
  Exec(ExpandConstant('{sys}\sc.exe'), 'stop PicassoApache', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  Exec(ExpandConstant('{sys}\sc.exe'), 'stop Apache2.4', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  Exec(ExpandConstant('{sys}\sc.exe'), 'stop PicassoMariaDB', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  Exec(ExpandConstant('{sys}\sc.exe'), 'stop Breeze', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
end;

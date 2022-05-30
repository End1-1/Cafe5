#define ApplicationName 'BreezeApplicationServer'
#define ApplicationVersion GetVersionNumbersString('Service5.exe')

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{D006344E-C833-4764-9CC2-622DDA5B8BCB}
AppName={#ApplicationName}
AppVerName={#ApplicationName} 1.2
AppVersion={#ApplicationVersion}
VersionInfoVersion={#ApplicationVersion}
AppPublisher=BreezeDevs, Inc.
AppPublisherURL=https://www.breezedevs.com/
;AppSupportURL=https://www.example.com/
;AppUpdatesURL=https://www.example.com/
DefaultDirName={pf64}\Breeze Server
DefaultGroupName=Breeze Server
ChangesAssociations=yes
DisableProgramGroupPage=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputBaseFilename=ApplicationServer
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Files]
Source: "C:\build\Install_Cafe5_x64\cert.pem"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\key.pem"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\Qt5Core.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\Qt5Network.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\Qt5Sql.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\Qt5Widgets.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\Qt5Gui.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\Qt5PrintSupport.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\libmysql.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\fbclient.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\libcrypto-1_1-x64.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\libssl-1_1-x64.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\sqldrivers\qsqlibase.dll"; DestDir: "{app}\sqldrivers"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\sqldrivers\qsqlmysql.dll"; DestDir: "{app}\sqldrivers"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\sqldrivers\qsqlodbc.dll"; DestDir: "{app}\sqldrivers"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\printsupport\windowsprintersupport.dll"; DestDir: "{app}\printsupport"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\imageformats\qjpeg.dll"; DestDir: "{app}\imageformats"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\platforms\qminimal.dll"; DestDir: "{app}\platforms"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: onlyifdoesntexist
Source: "C:\build\Install_Cafe5_x64\vcredist\VC_redist.x64.exe"; DestDir: {tmp}; Flags: deleteafterinstall
Source: "C:\build\Service5\release\Service5.exe"; DestDir: "{app}"; BeforeInstall: BeforeServiceInstall('Breeze'); AfterInstall: AfterServiceInstall('Breeze', 'Service5.exe');
Source: "C:\build\Service5\dlls\standard\release\Standard.dll"; DestDir: "{app}\handlers";

[Code]
#include 'services_unicode.iss';

procedure BeforeServiceInstall(SvcName: String);
var
  S: Longword;
begin
  //If service is installed, it needs to be stopped
  if ServiceExists(SvcName) then begin
    try 
      //S:= SimpleQueryService(SvcName);
      //if S <> SERVICE_STOPPED then begin
        SimpleStopService(SvcName, True, True);
      //end;
    finally 
    end;
  end;
end;

procedure AfterServiceInstall(SvcName, FileName: String);
begin
  //If service is not installed, it needs to be installed now
  if not ServiceExists(SvcName) then begin
    if SimpleCreateService(SvcName, 'Breeze', ExpandConstant('{app}')+'\' + FileName + ' --config=' + ExpandConstant('{app}') + '\config.ini', SERVICE_AUTO_START, '', '', False, True) then begin
      //Service successfully installed
      SimpleStartService(SvcName, True, True);
    end else begin
      //Service failed to install

    end;
  end else begin
    SimpleStartService(SvcName, True, True);
  end;
end;

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
;Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked


[Registry]

[Icons]
;Name: "{autoprograms}\My Program"; Filename: "{app}\Service5.exe"
;Name: "{autodesktop}\My Program"; Filename: "{app}\Service5.exe"; Tasks: desktopicon
Name: "{group}\Monitor"; Filename: "{app}\Service5.exe"; Parameters: "gui";
Name: "{group}\{cm:UninstallProgram,BreezeServer}"; Filename: "{uninstallexe}"

[Run]
Filename: {tmp}\VC_redist.x64.exe; Parameters: "/q /passive /norestart /Q:a /c:""msiexec /q /i vcredist.msi"""; StatusMsg: "Installing VC++ 2015-2019 x64 Redistributables..."

[UninstallRun]
Filename: {sys}\sc.exe; Parameters: "stop breeze" ; Flags: runhidden
Filename: {sys}\sc.exe; Parameters: "delete breeze" ; Flags: runhidden

[UninstallDelete]
Type: files; Name: "{app}\config.ini";

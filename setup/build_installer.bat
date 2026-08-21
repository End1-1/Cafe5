@echo off
setlocal EnableExtensions

set "SCRIPT_DIR=%~dp0"
set "ISCC=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
set "BUMP="
set "COMPONENT=all"

:parse_args
if "%~1"=="" goto :args_done
if /I "%~1"=="bump" set "BUMP=-Bump" & shift & goto :parse_args
if /I "%~1"=="-bump" set "BUMP=-Bump" & shift & goto :parse_args
if /I "%~1"=="--bump" set "BUMP=-Bump" & shift & goto :parse_args
if /I "%~1"=="all" set "COMPONENT=all" & shift & goto :parse_args
if /I "%~1"=="full" set "COMPONENT=all" & shift & goto :parse_args
if /I "%~1"=="frontdesk" set "COMPONENT=frontdesk" & shift & goto :parse_args
if /I "%~1"=="office" set "COMPONENT=frontdesk" & shift & goto :parse_args
if /I "%~1"=="officen" set "COMPONENT=frontdesk" & shift & goto :parse_args
if /I "%~1"=="shop" set "COMPONENT=shop" & shift & goto :parse_args
if /I "%~1"=="waiter" set "COMPONENT=waiter" & shift & goto :parse_args
if /I "%~1"=="cookingprogress" set "COMPONENT=cookingprogress" & shift & goto :parse_args
if /I "%~1"=="cooking" set "COMPONENT=cookingprogress" & shift & goto :parse_args
if /I "%~1"=="service5" set "COMPONENT=service5" & shift & goto :parse_args
if /I "%~1"=="service" set "COMPONENT=service5" & shift & goto :parse_args
echo Unknown argument: %~1
echo Usage: build_installer.bat [frontdesk^|shop^|waiter^|cookingprogress^|service5^|all] [bump]
exit /b 1

:args_done
if not exist "%ISCC%" goto :no_iscc

echo Staging component: %COMPONENT%
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%stage.ps1" -Component "%COMPONENT%" %BUMP%
if errorlevel 1 exit /b 1

if not exist "%SCRIPT_DIR%versions.json" goto :no_staging

if /I "%COMPONENT%"=="all" (
  if not exist "%SCRIPT_DIR%staging\OfficeN.exe" goto :no_staging
  echo.
  echo Building unified installer...
  "%ISCC%" "%SCRIPT_DIR%Picasso.iss"
  if errorlevel 1 exit /b 1

  powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%build_components.ps1" -Iscc "%ISCC%" -Component all
  if errorlevel 1 exit /b 1

  echo.
  echo Installers:
  echo   %SCRIPT_DIR%output\Picasso_Setup_x64.exe
  echo   %SCRIPT_DIR%output\frontdesk\
  echo   %SCRIPT_DIR%output\shop\
  echo   %SCRIPT_DIR%output\waiter\
  echo   %SCRIPT_DIR%output\cookingprogress\
  echo   %SCRIPT_DIR%output\service5\
) else (
  if /I "%COMPONENT%"=="frontdesk" if not exist "%SCRIPT_DIR%staging\OfficeN.exe" goto :no_staging
  if /I "%COMPONENT%"=="shop" if not exist "%SCRIPT_DIR%staging\Shop_net.exe" goto :no_staging
  if /I "%COMPONENT%"=="waiter" if not exist "%SCRIPT_DIR%staging\Waiter.exe" goto :no_staging
  if /I "%COMPONENT%"=="waiter" if not exist "%SCRIPT_DIR%staging\WaiterDesigner.exe" goto :no_staging
  if /I "%COMPONENT%"=="cookingprogress" if not exist "%SCRIPT_DIR%staging\CookingProgress.exe" goto :no_staging
  if /I "%COMPONENT%"=="service5" if not exist "%SCRIPT_DIR%staging\service5.exe" goto :no_staging

  echo.
  echo Building %COMPONENT% installer only...
  powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%build_components.ps1" -Iscc "%ISCC%" -Component "%COMPONENT%"
  if errorlevel 1 exit /b 1

  echo.
  echo Installer:
  echo   %SCRIPT_DIR%output\%COMPONENT%\
)

if defined BUMP (
  echo.
  echo Versions were bumped. Rebuild Release apps so EXE matches the new version,
  echo then run again without bump: setup\build_installer.bat %COMPONENT%
)
exit /b 0

:no_iscc
echo Inno Setup compiler not found:
echo   %ISCC%
exit /b 1

:no_staging
echo Staging folder is incomplete. Build Release targets first.
exit /b 1

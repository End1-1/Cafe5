@echo off
setlocal EnableExtensions

set "SCRIPT_DIR=%~dp0"
set "ISCC=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"

if not exist "%ISCC%" goto :no_iscc

powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%stage.ps1"
if errorlevel 1 exit /b 1

if not exist "%SCRIPT_DIR%staging\OfficeN.exe" goto :no_staging

"%ISCC%" "%SCRIPT_DIR%Picasso.iss"
if errorlevel 1 exit /b 1

echo.
echo Installer: %SCRIPT_DIR%output\Picasso_Setup_x64.exe
exit /b 0

:no_iscc
echo Inno Setup compiler not found:
echo   %ISCC%
exit /b 1

:no_staging
echo Staging folder is incomplete. Build Release targets first.
exit /b 1

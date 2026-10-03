@echo off
setlocal EnableExtensions

set "SCRIPT_DIR=%~dp0"
set "ISCC=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"

if not exist "%ISCC%" goto :noiscc

echo Staging Picasso web server...
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%stage_server.ps1"
if errorlevel 1 exit /b 1

if not exist "%SCRIPT_DIR%staging-server\apache\bin\httpd.exe" goto :incomplete
if not exist "%SCRIPT_DIR%staging-server\php\php.exe" goto :incomplete
if not exist "%SCRIPT_DIR%staging-server\mysql\bin\mysqld.exe" goto :incomplete
if not exist "%SCRIPT_DIR%staging-server\www\engine\cnf.php" goto :incomplete
if not exist "%SCRIPT_DIR%staging-server\sql\picasso.sql" goto :incomplete
if not exist "%SCRIPT_DIR%staging-server\sql\service5.sql" goto :incomplete
if not exist "%SCRIPT_DIR%staging-server\service5\service5.exe" goto :incomplete
if not exist "%SCRIPT_DIR%staging-server\heidisql\heidisql.exe" goto :incomplete

set "BUILD_FILE=%SCRIPT_DIR%server_build.txt"
if not exist "%BUILD_FILE%" >"%BUILD_FILE%" echo 0
set /p BUILD_NUM=<"%BUILD_FILE%"
set /a BUILD_NUM+=1

echo.
echo Building PicassoWeb_Setup_x64_%BUILD_NUM%.exe ...
"%ISCC%" /DBuildNumber=%BUILD_NUM% "%SCRIPT_DIR%Server.iss"
if errorlevel 1 exit /b 1
>"%BUILD_FILE%" echo %BUILD_NUM%

echo.
echo Installer:
echo   %SCRIPT_DIR%output\PicassoWeb_Setup_x64_%BUILD_NUM%.exe
exit /b 0

:incomplete
echo Staging folder is incomplete.
exit /b 1

:noiscc
echo Inno Setup compiler not found:
echo   %ISCC%
exit /b 1

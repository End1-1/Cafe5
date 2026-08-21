@echo off
setlocal EnableExtensions EnableDelayedExpansion

:: Working dir = this script's folder (FrontDesk)
cd /d "%~dp0"

:: Same kit discovery as lupdate.bat (DLL mismatch = "Entry Point Not Found").
set "LRELEASE="
set "QTBIN="

if defined QTDIR if exist "%QTDIR%\bin\lrelease.exe" (
    set "LRELEASE=%QTDIR%\bin\lrelease.exe"
    set "QTBIN=%QTDIR%\bin"
)

if not defined LRELEASE if exist "C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin\lrelease.exe" (
    set "LRELEASE=C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin\lrelease.exe"
    set "QTBIN=C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin"
)

if not defined LRELEASE if exist "C:\Development\Qt\5.15.2\msvc2019\bin\lrelease.exe" (
    set "LRELEASE=C:\Development\Qt\5.15.2\msvc2019\bin\lrelease.exe"
    set "QTBIN=C:\Development\Qt\5.15.2\msvc2019\bin"
)

if not defined LRELEASE if exist "D:\Qt\6.10.2\msvc2022_64\bin\lrelease.exe" (
    set "LRELEASE=D:\Qt\6.10.2\msvc2022_64\bin\lrelease.exe"
    set "QTBIN=D:\Qt\6.10.2\msvc2022_64\bin"
)

if not defined LRELEASE if exist "C:\Qt\6.10.2\msvc2022_64\bin\lrelease.exe" (
    set "LRELEASE=C:\Qt\6.10.2\msvc2022_64\bin\lrelease.exe"
    set "QTBIN=C:\Qt\6.10.2\msvc2022_64\bin"
)

if not defined LRELEASE if exist "%USERPROFILE%\Qt\6.10.2\msvc2022_64\bin\lrelease.exe" (
    set "LRELEASE=%USERPROFILE%\Qt\6.10.2\msvc2022_64\bin\lrelease.exe"
    set "QTBIN=%USERPROFILE%\Qt\6.10.2\msvc2022_64\bin"
)

if not defined LRELEASE (
    where lrelease.exe >nul 2>&1
    if not errorlevel 1 for /f "delims=" %%i in ('where lrelease.exe') do (
        set "LRELEASE=%%i"
        set "QTBIN=%%~dpi"
        goto :have_lrelease
    )
)

:have_lrelease
if not defined LRELEASE (
    echo lrelease.exe not found.
    echo Install Qt Linguist tools, or set QTDIR to a kit whose bin\lrelease.exe matches that kit's DLLs.
    pause
    exit /b 1
)

if not exist "FrontDesk.ts" (
    echo FrontDesk.ts not found in "%CD%"
    pause
    exit /b 1
)

set "PATH=%QTBIN%;%PATH%"

echo Using: "%LRELEASE%"
echo Compiling FrontDesk.ts -^> FrontDesk.qm
"%LRELEASE%" "FrontDesk.ts" -qm "FrontDesk.qm"
if exist "FrontDesk_ru.ts" (
    echo Compiling FrontDesk_ru.ts -^> FrontDesk_ru.qm
    "%LRELEASE%" "FrontDesk_ru.ts" -qm "FrontDesk_ru.qm"
)

echo.
echo Exit code: %ERRORLEVEL%
echo.
echo Next: rebuild FrontDesk so lang.qrc picks up the new .qm files
pause
endlocal

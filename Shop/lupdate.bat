@echo off
setlocal EnableExtensions EnableDelayedExpansion

:: Working dir = this script's folder (Shop)
cd /d "%~dp0"

:: Pick a consistent lupdate + its bin (DLL mismatch = "Entry Point Not Found").
:: NOTE: C:\Development\Qt\6.10.2\...\lupdate.exe on this machine is 6.8.0 and broken.
set "LUPDATE="
set "QTBIN="

if defined QTDIR if exist "%QTDIR%\bin\lupdate.exe" (
    set "LUPDATE=%QTDIR%\bin\lupdate.exe"
    set "QTBIN=%QTDIR%\bin"
)

if not defined LUPDATE if exist "C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin\lupdate.exe" (
    set "LUPDATE=C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin\lupdate.exe"
    set "QTBIN=C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin"
)

if not defined LUPDATE if exist "C:\Development\Qt\5.15.2\msvc2019\bin\lupdate.exe" (
    set "LUPDATE=C:\Development\Qt\5.15.2\msvc2019\bin\lupdate.exe"
    set "QTBIN=C:\Development\Qt\5.15.2\msvc2019\bin"
)

if not defined LUPDATE if exist "D:\Qt\6.10.2\msvc2022_64\bin\lupdate.exe" (
    set "LUPDATE=D:\Qt\6.10.2\msvc2022_64\bin\lupdate.exe"
    set "QTBIN=D:\Qt\6.10.2\msvc2022_64\bin"
)

if not defined LUPDATE if exist "C:\Qt\6.10.2\msvc2022_64\bin\lupdate.exe" (
    set "LUPDATE=C:\Qt\6.10.2\msvc2022_64\bin\lupdate.exe"
    set "QTBIN=C:\Qt\6.10.2\msvc2022_64\bin"
)

if not defined LUPDATE if exist "%USERPROFILE%\Qt\6.10.2\msvc2022_64\bin\lupdate.exe" (
    set "LUPDATE=%USERPROFILE%\Qt\6.10.2\msvc2022_64\bin\lupdate.exe"
    set "QTBIN=%USERPROFILE%\Qt\6.10.2\msvc2022_64\bin"
)

if not defined LUPDATE (
    where lupdate.exe >nul 2>&1
    if not errorlevel 1 for /f "delims=" %%i in ('where lupdate.exe') do (
        set "LUPDATE=%%i"
        set "QTBIN=%%~dpi"
        goto :have_lupdate
    )
)

:have_lupdate
if not defined LUPDATE (
    echo lupdate.exe not found.
    echo Install Qt Linguist tools, or set QTDIR to a kit whose bin\lupdate.exe matches that kit's DLLs.
    echo Example: set QTDIR=C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version
    pause
    exit /b 1
)

:: Must load DLLs from the same kit as lupdate.exe
set "PATH=%QTBIN%;%PATH%"

echo Using: "%LUPDATE%"
echo PATH bin: "%QTBIN%"
"%LUPDATE%" ^
    "." "../Cafe5" "../Classes" "../Forms" ^
    -ts "Shop.ts" -recursive -no-obsolete

echo.
echo Exit code: %ERRORLEVEL%
pause
endlocal

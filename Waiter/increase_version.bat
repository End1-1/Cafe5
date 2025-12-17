@echo off
set FILE=version.h

for /f "tokens=3" %%a in ('findstr VER_BUILD %FILE%') do set BUILD=%%a
set /a BUILD+=1

(
  for /f "usebackq delims=" %%l in ("%FILE%") do (
    echo %%l | findstr VER_BUILD >nul
    if errorlevel 1 (
      echo %%l
    ) else (
      echo #define VER_BUILD %BUILD%
    )
  )
) > %FILE%.tmp

move /y %FILE%.tmp %FILE%

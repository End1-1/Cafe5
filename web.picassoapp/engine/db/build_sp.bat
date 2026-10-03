@echo off
cd /d "%~dp0"
python build_sp_sql.py
if errorlevel 1 (
  echo FAILED
  exit /b 1
)
echo.
echo Ready: sp.sql  — run this file on the server to recreate all routines.
exit /b 0

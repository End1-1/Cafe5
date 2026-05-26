@echo off
setlocal
cd /d "%~dp0.."
dart run tool/bump_build.dart || exit /b 1
flutter build %*

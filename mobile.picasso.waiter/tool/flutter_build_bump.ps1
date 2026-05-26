# Bump pubspec build number (+N), then run flutter build with the same arguments.
# Example: pwsh tool/flutter_build_bump.ps1 windows
#          pwsh tool/flutter_build_bump.ps1 apk --release

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root

dart run tool/bump_build.dart
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
flutter build @args

// Increments the build number after '+' in pubspec.yaml (e.g. 1.0.2+25 -> 1.0.2+26).
// Run from project root: dart run tool/bump_build.dart

import 'dart:io';

void main(List<String> args) {
  final dryRun = args.contains('--dry-run');
  final pubspec = File('pubspec.yaml');
  if (!pubspec.existsSync()) {
    stderr.writeln('pubspec.yaml not found. Run from project root.');
    exitCode = 1;
    return;
  }

  var content = pubspec.readAsStringSync();
  final re = RegExp(r'^version:\s*([\d.]+)\+(\d+)\s*$', multiLine: true);
  final m = re.firstMatch(content);
  if (m == null) {
    stderr.writeln(
        'Could not parse version line (expected e.g. version: 1.0.2+25).');
    exitCode = 1;
    return;
  }

  final ver = m.group(1)!;
  final build = int.parse(m.group(2)!);
  final next = build + 1;
  final newLine = 'version: $ver+$next';

  if (dryRun) {
    stdout.writeln('Would set $newLine (current +$build)');
    return;
  }

  content = content.replaceFirstMapped(re, (_) => newLine);
  pubspec.writeAsStringSync(content);
  stdout.writeln('Version bumped: $ver+$build -> $ver+$next');
}

import 'dart:convert';
import 'dart:io' show Platform;

import 'package:flutter/foundation.dart';
import 'package:http/http.dart' as http;
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/waiter_workstation.dart';

/// POST to workstation [print_server] — same payload shape as [NInterface::query1].
class WaiterPrintClient {
  static Uri resolveUri(String printServer) {
    final trimmed = printServer.trim();
    if (trimmed.startsWith('http://') || trimmed.startsWith('https://')) {
      return Uri.parse(trimmed);
    }
    final path = trimmed.startsWith('/') ? trimmed.substring(1) : trimmed;
    final https = prefs.getBool('https') ?? false;
    final host = prefs.string('serveraddress');
    return https ? Uri.https(host, path) : Uri.http(host, path);
  }

  static String _hostinfo() {
    try {
      return Platform.localHostname;
    } catch (_) {
      return WaiterWorkstation.workstationName();
    }
  }

  static Future<String?> post({
    required String printServer,
    required String printerName,
    required List<Map<String, dynamic>> printData,
  }) async {
    if (printServer.trim().isEmpty) {
      return null;
    }

    final body = <String, dynamic>{
      'app': 'picasso.waiter',
      'config': prefs.string('config'),
      'cashbox_id': prefs.cashboxId(),
      'workingday': prefs.dateMySqlText(prefs.workingDay()),
      'language': 'am',
      'hostinfo': _hostinfo(),
      'printer_name': printerName,
      'print_data': printData,
    };

    final uri = resolveUri(printServer);
    if (kDebugMode) {
      debugPrint('print server POST $uri printer=$printerName cmds=${printData.length}');
    }

    try {
      final response = await http
          .post(
            uri,
            headers: {
              'Content-Type': 'application/json',
              'Authorization': 'Bearer ${prefs.string('token')}',
              'X-Application-Name': 'picasso.waiter',
              'X-Application-Version': prefs.getString('appversion') ?? '',
            },
            body: utf8.encode(jsonEncode(body)),
          )
          .timeout(const Duration(seconds: 15));

      if (response.statusCode >= 200 && response.statusCode < 300) {
        return null;
      }
      return utf8.decode(response.bodyBytes);
    } catch (e) {
      return e.toString();
    }
  }
}

import 'dart:convert';
import 'package:picassowaiter/main.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;


bool _hasV2HallPayload(Map<dynamic, dynamic> m) =>
    m.containsKey('halls') && m.containsKey('tables');

bool _hasV2MenuPayload(Map<dynamic, dynamic> m) =>
    m.containsKey('groups') && m.containsKey('dishes');

bool _hasDataListPayload(Map<dynamic, dynamic> m) =>
    m['data'] is List && m['data'].isNotEmpty;

class HttpQuery {

  bool needlonglog = false;
  final String route;
  final int timeout;
  HttpQuery(this.route, {this.timeout = 20});

  Future<Map<String, dynamic>> request(Map<String, dynamic> inData) async {

    inData['app'] = 'picasso.waiter';
    inData['config'] = prefs.string('config');
    inData['cashbox_id'] = prefs.cashboxId();
    inData['workingday'] = prefs.dateMySqlText(prefs.workingDay());
    // UI am → API hy, same as desktop NDataProvider.
    inData['locale'] = 'hy';
    inData['language'] = 'am';
    inData['debug'] = false && kDebugMode;

    Map<String, Object?> outData = {};
    String strBody = jsonEncode(inData);
    if (kDebugMode) {
      if (needlonglog) {
        debugPrint('request ${prefs.string("serveraddress")}/$route: $strBody');
      } else {
          print('request ${prefs.string("serveraddress")}/$route: $strBody');
      }
    }
    try {
      Uri uri = prefs.getBool('https') ?? false ? Uri.https(prefs.string("serveraddress"), route) : Uri.http(prefs.string("serveraddress"), route);
      // useProgramToken / useWaiterToken write the active bearer into `token`.
      final selected = prefs.string('token');
      final bearer = selected.isNotEmpty ? selected : prefs.bearerToken();
      var response = await http
          .post(
              uri,
              headers: {
                'Content-Type': 'application/json',
                'X-Application-Name': 'picasso.waiter',
                'X-Application-Version': prefs.getString('appversion') ?? '',
                'Authorization': 'Bearer $bearer',
              },
              body: utf8.encode(strBody))
          .timeout(Duration(seconds: timeout), onTimeout: () {
        return http.Response('Timeout', 408);
      });
      String strResponse = utf8.decode(response.bodyBytes);
      if (kDebugMode) {
        if (needlonglog) {
          debugPrint('Row body $strResponse');
        } else {
          print('Row body $strResponse');
        }
      }
      if (response.statusCode < 299) {
        try {
          outData = jsonDecode(strResponse);
          if (!outData.containsKey('status')) {
            final data = outData['data'];
            final hasOrder = outData.containsKey('order') ||
                (data is Map && data.containsKey('order'));
            final hasV2Hall = _hasV2HallPayload(outData) ||
                (data is Map && _hasV2HallPayload(data));
            final hasV2Menu = _hasV2MenuPayload(outData) ||
                (data is Map && _hasV2MenuPayload(data));
            final hasDataList = _hasDataListPayload(outData) ||
                (data is Map && _hasDataListPayload(data)) ||
                data is List;
            final hasSession = outData.containsKey('token') ||
                outData.containsKey('userdata') ||
                (outData['data'] is Map &&
                    (outData['data'] as Map).containsKey('sessionkey'));
            outData['status'] = (hasOrder ||
                    hasV2Hall ||
                    hasV2Menu ||
                    hasDataList ||
                    hasSession)
                ? 1
                : 0;
            if (!hasOrder &&
                !hasV2Hall &&
                !hasV2Menu &&
                !hasDataList &&
                !hasSession &&
                !outData.containsKey('data')) {
              outData['data'] = jsonEncode(outData);
            }
          }
        } catch (e) {
          outData['status'] = 0;
          outData['data'] = '${e.toString()} $strResponse';
        }
      } else {
        outData['status'] = 0;
        outData['error'] = response.statusCode;
        outData['data'] = _errorText(strResponse);
        if (response.statusCode == 401 && !_isAuthRoute(route)) {
          _dropRejectedSession(bearer);
        }
      }
    } catch (e) {
      outData['status'] = 0;
      outData['data'] = e.toString();
    }
    if (kDebugMode) {
      if (needlonglog) {
        debugPrint('Output $outData');
      } else {
        print('Output $outData');
      }
    }
    return outData;
  }
}

bool _isAuthRoute(String route) {
  return route.contains('user-login/') || route.endsWith('engine/login.php');
}

String _errorText(String raw) {
  final t = raw.trim();
  if (t.isEmpty) return 'Request failed';
  try {
    final decoded = jsonDecode(t);
    if (decoded is Map) {
      final err = decoded['error'] ?? decoded['errorMessage'] ?? decoded['data'];
      if (err is String && err.isNotEmpty) return err;
      if (decoded['new_version'] != null) {
        return 'Application update required '
            '(${decoded['old_version']} → ${decoded['new_version']})';
      }
    }
  } catch (_) {}
  return t;
}

/// 401 on an already authenticated call. Wrong PIN stays on the auth routes
/// and must not wipe the program session. During program startup the flag
/// is still false — the caller handles that error and keeps the new token.
void _dropRejectedSession(String bearer) {
  if (!(prefs.getBool('program_ready') ?? false)) {
    return;
  }
  final program = prefs.string('program_token');
  final waiter = prefs.string('waiter_token');
  if (waiter.isNotEmpty && bearer == waiter && program.isNotEmpty) {
    prefs.remove('waiter_token');
    prefs.setInt('userid', 0);
    prefs.remove('permissions');
    prefs.remove('staff_name');
    prefs.setString('token', program);
  } else {
    prefs.setString('sessionkey', '');
    prefs.setString('waiter_token', '');
    prefs.setString('program_token', '');
    prefs.setString('token', '');
    prefs.setBool('program_ready', false);
  }
  final nav = Prefs.navigatorKey.currentState;
  if (nav != null) {
    nav.pushAndRemoveUntil(
      MaterialPageRoute(builder: (_) => const App()),
      (_) => false,
    );
  }
}

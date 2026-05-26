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
      var response = await http
          .post(
              uri,
              headers: {
                'Content-Type': 'application/json',
                'X-Application-Name': 'picasso.waiter',
                'X-Application-Version': prefs.getString('appversion') ?? '',
                'Authorization': 'Bearer ${prefs.bearerToken()}',
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
            outData['status'] =
                (hasOrder || hasV2Hall || hasV2Menu || hasDataList) ? 1 : 0;
            if (!hasOrder &&
                !hasV2Hall &&
                !hasV2Menu &&
                !hasDataList &&
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
        outData['data'] = strResponse;
        if (response.statusCode == 401) {
          prefs.setString('sessionkey', '');
          prefs.setString('waiter_token', '');
          prefs.setString('program_token', '');
          prefs.setString('token', '');
          prefs.setBool('program_ready', false);
          final nav = Prefs.navigatorKey.currentState;
          if (nav != null) {
            nav.pushAndRemoveUntil(
              MaterialPageRoute(builder: (_) => const App()),
              (_) => false,
            );
          }
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

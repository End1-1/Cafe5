import 'dart:convert';

import 'package:flutter/foundation.dart';
import 'package:http/http.dart' as http;

import 'i18n.dart';

class ApiException implements Exception {
  ApiException(this.message);
  final String message;

  @override
  String toString() => message;
}

class Api {
  Api({required this.base, required this.token, required this.locale});

  final String base;
  final String token;
  final String locale;

  Future<Map<String, dynamic>> post(String action, Map<String, dynamic> body) async {
    final root = base.replaceAll(RegExp(r'/+$'), '');
    final uri = Uri.parse('$root/engine/v2/artadramas/workshop/$action');
    final payload = Map<String, dynamic>.from(body);
    payload['locale'] = locale == 'hy' ? 'hy' : 'ru';
    final headers = <String, String>{
      'Content-Type': 'application/json',
      'X-Application-Name': 'artadramas',
      'X-Application-Version': '1.0.0',
    };
    if (token.isNotEmpty) {
      headers['Authorization'] = 'Bearer $token';
    }
    final requestBody = jsonEncode(payload);
    _debug('→ POST $uri\n$requestBody');
    late final http.Response response;
    try {
      response = await http
          .post(uri, headers: headers, body: utf8.encode(requestBody))
          .timeout(const Duration(seconds: 25));
    } catch (e) {
      _debug('← POST $uri failed: $e');
      throw ApiException(S.t(locale, 'networkFail'));
    }
    final text = utf8.decode(response.bodyBytes);
    _debug('← ${response.statusCode} $uri\n$text');
    if (response.statusCode >= 300) {
      throw ApiException(text.trim().isEmpty ? 'HTTP ${response.statusCode}' : text.trim());
    }
    final decoded = jsonDecode(text);
    if (decoded is! Map) {
      throw ApiException(text);
    }
    final map = Map<String, dynamic>.from(decoded);
    if (map['status'] != 1) {
      throw ApiException(map['message']?.toString() ?? text);
    }
    return map;
  }
}

void _debug(String message) {
  if (kDebugMode) {
    debugPrint(message);
  }
}

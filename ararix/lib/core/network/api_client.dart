import 'dart:convert';

import 'package:dio/dio.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import 'api_exception.dart';
import 'backend_hosts.dart';
import '../storage/session_store.dart';

final apiBaseUrlProvider =
    NotifierProvider<ApiBaseUrlNotifier, String>(ApiBaseUrlNotifier.new);

class ApiBaseUrlNotifier extends Notifier<String> {
  @override
  String build() {
    final stored = ref.read(sessionStoreProvider).readApiBase();
    if (stored != null && stored.isNotEmpty) {
      return stored;
    }
    return BackendHosts.defaultBaseUrl;
  }

  Future<void> setBaseUrl(String baseUrl) async {
    await ref.read(sessionStoreProvider).writeApiBase(baseUrl);
    state = baseUrl;
  }
}

final apiClientProvider = Provider<ApiClient>((ref) {
  final session = ref.watch(sessionStoreProvider);
  final baseUrl = ref.watch(apiBaseUrlProvider);
  return ApiClient(session, baseUrl);
});

class ApiClient {
  ApiClient(this._session, String baseUrl) {
    _dio = Dio(
      BaseOptions(
        baseUrl: baseUrl,
        connectTimeout: const Duration(seconds: 20),
        receiveTimeout: const Duration(seconds: 20),
        headers: {
          'Content-Type': 'application/json',
          'X-Application-Name': 'ararix',
          'X-Application-Version': '1.0.0',
        },
      ),
    );

    _dio.interceptors.add(
      InterceptorsWrapper(
        onRequest: (options, handler) async {
          final token = await _session.readToken();
          if (token != null && token.isNotEmpty) {
            options.headers['Authorization'] = 'Bearer $token';
          }
          final locale = await _session.readLocale();
          options.headers['Accept-Language'] = locale;
          handler.next(options);
        },
      ),
    );

    if (kDebugMode) {
      _dio.interceptors.add(_DebugLogInterceptor());
    }
  }

  final SessionStore _session;
  late final Dio _dio;

  Future<Map<String, dynamic>> post(
    String path,
    Map<String, dynamic> body,
  ) async {
    final locale = await _session.readLocale();
    final payload = {...body, 'locale': body['locale'] ?? locale};
    try {
      final response = await _dio.post<dynamic>(path, data: payload);
      final data = response.data;
      if (data is Map) {
        return Map<String, dynamic>.from(data);
      }
      if (data is String) {
        final trimmed = data.trim();
        if (trimmed.startsWith('{')) {
          final decoded = jsonDecode(trimmed);
          if (decoded is Map) {
            return Map<String, dynamic>.from(decoded);
          }
        }
        throw ApiException(trimmed, statusCode: response.statusCode);
      }
      throw ApiException('Unexpected response');
    } on DioException catch (e) {
      final raw = e.response?.data;
      final message = raw is String
          ? raw
          : (raw is Map && raw['error'] != null)
              ? raw['error'].toString()
              : (e.message ?? 'Network error');
      throw ApiException(message, statusCode: e.response?.statusCode);
    }
  }
}

class _DebugLogInterceptor extends Interceptor {
  static const _encoder = JsonEncoder.withIndent('  ');

  String _pretty(dynamic data) {
    if (data == null) return '<empty>';
    if (data is String) {
      try {
        return _encoder.convert(jsonDecode(data));
      } catch (_) {
        return data;
      }
    }
    try {
      return _encoder.convert(data);
    } catch (_) {
      return data.toString();
    }
  }

  @override
  void onRequest(RequestOptions options, RequestInterceptorHandler handler) {
    final uri = options.uri.toString();
    debugPrint('→ ${options.method} $uri');
    debugPrint('  body:\n${_pretty(options.data)}');
    handler.next(options);
  }

  @override
  void onResponse(Response response, ResponseInterceptorHandler handler) {
    final uri = response.requestOptions.uri.toString();
    debugPrint('← ${response.statusCode} $uri');
    debugPrint('  body:\n${_pretty(response.data)}');
    handler.next(response);
  }

  @override
  void onError(DioException err, ErrorInterceptorHandler handler) {
    final uri = err.requestOptions.uri.toString();
    debugPrint('✖ ${err.response?.statusCode ?? '-'} $uri');
    debugPrint('  error: ${err.message}');
    if (err.response?.data != null) {
      debugPrint('  body:\n${_pretty(err.response?.data)}');
    }
    handler.next(err);
  }
}

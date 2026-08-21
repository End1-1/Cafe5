import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:shared_preferences/shared_preferences.dart';

final sharedPreferencesProvider = Provider<SharedPreferences>((ref) {
  throw UnimplementedError('SharedPreferences must be overridden in main');
});

final sessionStoreProvider = Provider<SessionStore>((ref) {
  return SessionStore(ref.watch(sharedPreferencesProvider));
});

class SessionStore {
  SessionStore(this._prefs);

  static const _tokenKey = 'ararix_token';
  static const _localeKey = 'ararix_locale';
  static const _clientJsonKey = 'ararix_client_json';
  static const _apiBaseKey = 'ararix_api_base';

  final SharedPreferences _prefs;

  Future<String?> readToken() async => _prefs.getString(_tokenKey);

  Future<void> writeToken(String? token) async {
    if (token == null || token.isEmpty) {
      await _prefs.remove(_tokenKey);
    } else {
      await _prefs.setString(_tokenKey, token);
    }
  }

  Future<String> readLocale() async => _prefs.getString(_localeKey) ?? 'en';

  Future<void> writeLocale(String locale) async {
    await _prefs.setString(_localeKey, locale);
  }

  Future<String?> readClientJson() async => _prefs.getString(_clientJsonKey);

  Future<void> writeClientJson(String? json) async {
    if (json == null || json.isEmpty) {
      await _prefs.remove(_clientJsonKey);
    } else {
      await _prefs.setString(_clientJsonKey, json);
    }
  }

  String? readApiBase() => _prefs.getString(_apiBaseKey);

  Future<void> writeApiBase(String? baseUrl) async {
    if (baseUrl == null || baseUrl.isEmpty) {
      await _prefs.remove(_apiBaseKey);
    } else {
      await _prefs.setString(_apiBaseKey, baseUrl);
    }
  }

  Future<void> clear() async {
    await writeToken(null);
    await writeClientJson(null);
  }
}

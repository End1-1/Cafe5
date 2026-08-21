import 'dart:convert';

import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/storage/session_store.dart';
import '../data/client_model.dart';

final sessionProvider =
    NotifierProvider<SessionNotifier, SessionState>(SessionNotifier.new);

class SessionState {
  const SessionState({
    this.token,
    this.client,
    this.locale = 'en',
    this.ready = false,
  });

  final String? token;
  final ClientModel? client;
  final String locale;
  final bool ready;

  bool get isAuthenticated => token != null && token!.isNotEmpty;

  SessionState copyWith({
    String? token,
    ClientModel? client,
    String? locale,
    bool? ready,
    bool clearToken = false,
    bool clearClient = false,
  }) {
    return SessionState(
      token: clearToken ? null : (token ?? this.token),
      client: clearClient ? null : (client ?? this.client),
      locale: locale ?? this.locale,
      ready: ready ?? this.ready,
    );
  }
}

class SessionNotifier extends Notifier<SessionState> {
  @override
  SessionState build() {
    Future.microtask(restore);
    return const SessionState();
  }

  SessionStore get _store => ref.read(sessionStoreProvider);

  Future<void> restore() async {
    final token = await _store.readToken();
    final locale = await _store.readLocale();
    final raw = await _store.readClientJson();
    ClientModel? client;
    if (raw != null && raw.isNotEmpty) {
      try {
        client = ClientModel.fromJson(
          jsonDecode(raw) as Map<String, dynamic>,
        );
      } catch (_) {}
    }
    state = SessionState(
      token: token,
      client: client,
      locale: locale,
      ready: true,
    );
  }

  Future<void> setAuthenticated({
    required String token,
    required ClientModel client,
  }) async {
    await _store.writeToken(token);
    await _store.writeClientJson(jsonEncode(client.toJson()));
    state = state.copyWith(token: token, client: client, ready: true);
  }

  Future<void> updateClient(ClientModel client) async {
    await _store.writeClientJson(jsonEncode(client.toJson()));
    state = state.copyWith(client: client);
  }

  Future<void> setLocale(String locale) async {
    await _store.writeLocale(locale);
    state = state.copyWith(locale: locale);
  }

  Future<void> logout() async {
    await _store.clear();
    state = SessionState(locale: state.locale, ready: true);
  }
}

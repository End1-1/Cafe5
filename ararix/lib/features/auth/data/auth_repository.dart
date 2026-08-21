import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';
import 'client_model.dart';

final authRepositoryProvider = Provider<AuthRepository>((ref) {
  return AuthRepository(ref.watch(apiClientProvider));
});

class AuthRepository {
  AuthRepository(this._api);

  final ApiClient _api;

  Future<void> requestOtp({
    required String countryCode,
    required String phone,
    required String channel,
  }) async {
    await _api.post('/ararix/auth/login', {
      'country_code': countryCode,
      'phone': phone,
      'channel': channel,
    });
  }

  Future<({String token, ClientModel client})> verifyOtp({
    required String countryCode,
    required String phone,
    required String code,
  }) async {
    final data = await _api.post('/ararix/auth/check-otp', {
      'country_code': countryCode,
      'phone': phone,
      'code': code,
    });
    return (
      token: data['token'] as String,
      client: ClientModel.fromJson(data['client'] as Map<String, dynamic>),
    );
  }

  Future<void> resendOtp({
    required String countryCode,
    required String phone,
    required String channel,
  }) async {
    await requestOtp(
      countryCode: countryCode,
      phone: phone,
      channel: channel,
    );
  }
}

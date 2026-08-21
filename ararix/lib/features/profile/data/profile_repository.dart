import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';
import '../../auth/data/client_model.dart';

final profileRepositoryProvider = Provider<ProfileRepository>((ref) {
  return ProfileRepository(ref.watch(apiClientProvider));
});

class ProfileRepository {
  ProfileRepository(this._api);

  final ApiClient _api;

  Future<ClientModel> get() async {
    final data = await _api.post('/ararix/profile/get', {});
    return ClientModel.fromJson(data['client'] as Map<String, dynamic>);
  }

  Future<ClientModel> update({
    String? name,
    String? email,
    String? locale,
  }) async {
    final body = <String, dynamic>{};
    if (name != null) body['name'] = name;
    if (email != null) body['email'] = email;
    if (locale != null) body['locale'] = locale;
    final data = await _api.post('/ararix/profile/update', body);
    return ClientModel.fromJson(data['client'] as Map<String, dynamic>);
  }

  Future<void> changePhoneRequest({
    required String countryCode,
    required String phone,
    required String channel,
  }) async {
    await _api.post('/ararix/profile/change-phone-request', {
      'country_code': countryCode,
      'phone': phone,
      'channel': channel,
    });
  }

  Future<ClientModel> changePhoneVerify({
    required String countryCode,
    required String phone,
    required String code,
  }) async {
    final data = await _api.post('/ararix/profile/change-phone-verify', {
      'country_code': countryCode,
      'phone': phone,
      'code': code,
    });
    return ClientModel.fromJson(data['client'] as Map<String, dynamic>);
  }
}

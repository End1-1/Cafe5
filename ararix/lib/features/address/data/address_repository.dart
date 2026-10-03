import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';
import 'address_models.dart';

final addressRepositoryProvider = Provider<AddressRepository>((ref) {
  return AddressRepository(ref.watch(apiClientProvider));
});

class AddressRepository {
  AddressRepository(this._api);

  final ApiClient _api;

  Future<({List<DeliveryAddress> addresses, DeliveryAddress? active})>
      list() async {
    final data = await _api.post('/ararix/addresses/list', {});
    final raw = data['addresses'];
    final list = raw is List
        ? raw
            .whereType<Map>()
            .map((e) => DeliveryAddress.fromJson(Map<String, dynamic>.from(e)))
            .toList()
        : <DeliveryAddress>[];
    DeliveryAddress? active;
    final activeRaw = data['active'];
    if (activeRaw is Map) {
      final a = DeliveryAddress.fromJson(Map<String, dynamic>.from(activeRaw));
      if (a.id != null) active = a;
    }
    return (addresses: list, active: active);
  }

  Future<DeliveryAddress> save(DeliveryAddress draft) async {
    final data = await _api.post('/ararix/addresses/save', draft.toSaveJson());
    final raw = data['address'];
    if (raw is! Map) {
      throw StateError('Invalid save response');
    }
    return DeliveryAddress.fromJson(Map<String, dynamic>.from(raw));
  }

  Future<DeliveryAddress?> setActive(int id) async {
    final data = await _api.post('/ararix/addresses/set-active', {'id': id});
    final raw = data['active'];
    if (raw is! Map) return null;
    final a = DeliveryAddress.fromJson(Map<String, dynamic>.from(raw));
    return a.id != null ? a : null;
  }

  Future<DeliveryAddress?> delete(int id) async {
    final data = await _api.post('/ararix/addresses/delete', {'id': id});
    final raw = data['active'];
    if (raw is! Map) return null;
    final a = DeliveryAddress.fromJson(Map<String, dynamic>.from(raw));
    return a.id != null ? a : null;
  }
}

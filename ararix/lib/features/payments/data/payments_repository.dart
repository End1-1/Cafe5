import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';

class PaymentCard {
  const PaymentCard({
    required this.id,
    required this.brand,
    required this.last4,
    this.holder,
  });

  final int id;
  final String brand;
  final String last4;
  final String? holder;

  factory PaymentCard.fromJson(Map<String, dynamic> json) {
    return PaymentCard(
      id: (json['id'] as num).toInt(),
      brand: json['brand'] as String? ?? 'card',
      last4: json['last4'] as String? ?? '0000',
      holder: json['holder'] as String?,
    );
  }
}

final paymentsRepositoryProvider = Provider<PaymentsRepository>((ref) {
  return PaymentsRepository(ref.watch(apiClientProvider));
});

class PaymentsRepository {
  PaymentsRepository(this._api);

  final ApiClient _api;

  Future<List<PaymentCard>> list() async {
    final data = await _api.post('/ararix/payments/list', {});
    final cards = data['cards'] as List<dynamic>? ?? [];
    return cards
        .map((e) => PaymentCard.fromJson(e as Map<String, dynamic>))
        .toList();
  }
}

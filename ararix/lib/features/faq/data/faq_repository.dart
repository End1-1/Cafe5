import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';

class FaqItem {
  const FaqItem({
    required this.id,
    required this.question,
    required this.answer,
  });

  final int id;
  final String question;
  final String answer;

  factory FaqItem.fromJson(Map<String, dynamic> json) {
    return FaqItem(
      id: (json['id'] as num).toInt(),
      question: json['question'] as String? ?? '',
      answer: json['answer'] as String? ?? '',
    );
  }
}

final faqRepositoryProvider = Provider<FaqRepository>((ref) {
  return FaqRepository(ref.watch(apiClientProvider));
});

class FaqRepository {
  FaqRepository(this._api);

  final ApiClient _api;

  Future<List<FaqItem>> list({required String locale}) async {
    final data = await _api.post('/ararix/faq/list', {'locale': locale});
    final items = data['items'] as List<dynamic>? ?? [];
    return items
        .map((e) => FaqItem.fromJson(e as Map<String, dynamic>))
        .toList();
  }
}

import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';
import 'home_models.dart';

final homeRepositoryProvider = Provider<HomeRepository>((ref) {
  return HomeRepository(ref.watch(apiClientProvider));
});

class HomeRepository {
  HomeRepository(this._api);

  final ApiClient _api;

  Future<HomeFeed> getHome() async {
    final data = await _api.post('/ararix/home/get', {});
    return HomeFeed.fromJson(data);
  }

  Future<List<HomeRestaurant>> searchRestaurants({
    String query = '',
    List<int> nationalityIds = const [],
    int? groupId,
  }) async {
    final body = <String, dynamic>{};
    final q = query.trim();
    if (q.isNotEmpty) {
      body['q'] = q;
    }
    final nats = nationalityIds.where((id) => id > 0).toList();
    if (nats.isNotEmpty) {
      body['nationality_ids'] = nats;
    }
    if (groupId != null && groupId > 0) {
      body['group_id'] = groupId;
    }
    if (body.isEmpty) {
      return const [];
    }

    final data = await _api.post('/ararix/home/search', body);
    final raw = data['restaurants'];
    if (raw is! List) return const [];
    return raw
        .whereType<Map>()
        .map((e) => HomeRestaurant.fromJson(Map<String, dynamic>.from(e)))
        .toList();
  }

  Future<List<HomeSuggestItem>> suggest({
    required String query,
    String? locale,
  }) async {
    final q = query.trim();
    if (q.isEmpty) {
      return const [];
    }
    final body = <String, dynamic>{'q': q};
    if (locale != null && locale.isNotEmpty) {
      body['locale'] = locale;
    }
    final data = await _api.post('/ararix/home/suggest', body);
    final raw = data['suggestions'];
    if (raw is! List) return const [];
    return raw
        .whereType<Map>()
        .map((e) => HomeSuggestItem.fromJson(Map<String, dynamic>.from(e)))
        .toList();
  }
}

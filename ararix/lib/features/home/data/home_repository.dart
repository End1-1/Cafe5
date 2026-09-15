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
    int? nationalityId,
  }) async {
    final body = <String, dynamic>{};
    final q = query.trim();
    if (q.isNotEmpty) {
      body['q'] = q;
    }
    if (nationalityId != null && nationalityId > 0) {
      body['nationality_id'] = nationalityId;
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
}

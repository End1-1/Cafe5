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
}

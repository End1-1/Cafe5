import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';
import '../../home/data/home_models.dart';
import '../../home/data/home_repository.dart';
import '../../order/data/menu_models.dart';
import '../../order/data/restaurant_repository.dart';

final menuBrowseRepositoryProvider = Provider<MenuBrowseRepository>((ref) {
  return MenuBrowseRepository(
    ref.watch(apiClientProvider),
    ref.watch(homeRepositoryProvider),
    ref.watch(restaurantRepositoryProvider),
  );
});

class MenuBrowseRepository {
  MenuBrowseRepository(this._api, this._home, this._restaurants);

  final ApiClient _api;
  final HomeRepository _home;
  final RestaurantRepository _restaurants;

  Future<List<HomeRestaurant>> listRestaurants() async {
    final data = await _api.post('/ararix/home/restaurants', {});
    final raw = data['restaurants'];
    if (raw is! List) return const [];
    return raw
        .whereType<Map>()
        .map((e) => HomeRestaurant.fromJson(Map<String, dynamic>.from(e)))
        .toList();
  }

  Future<List<HomeSuggestItem>> suggest(String query) {
    return _home.suggest(query: query);
  }

  Future<RestaurantMenu> restaurantMenu(int restaurantId) {
    return _restaurants.getMenu(restaurantId);
  }
}

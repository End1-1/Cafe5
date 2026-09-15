import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';
import 'menu_models.dart';

final restaurantRepositoryProvider = Provider<RestaurantRepository>((ref) {
  return RestaurantRepository(ref.watch(apiClientProvider));
});

class RestaurantRepository {
  RestaurantRepository(this._api);

  final ApiClient _api;

  Future<RestaurantInfo> getRestaurant(int restaurantId) async {
    final data = await _api.post('/ararix/restaurant/get', {
      'restaurant_id': restaurantId,
    });
    return RestaurantInfo.fromJson(
      Map<String, dynamic>.from(data['restaurant'] as Map? ?? const {}),
    );
  }

  Future<RestaurantMenu> getMenu(int restaurantId) async {
    final data = await _api.post('/ararix/restaurant/menu', {
      'restaurant_id': restaurantId,
    });
    return RestaurantMenu.fromJson(data);
  }
}

import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_exception.dart';
import '../../home/application/home_notifier.dart';
import '../data/menu_models.dart';
import '../data/restaurant_repository.dart';
import 'cart_notifier.dart';

class RestaurantOrderState {
  const RestaurantOrderState({
    this.menu,
    this.loading = false,
    this.error,
    this.selectedGroupId,
  });

  final RestaurantMenu? menu;
  final bool loading;
  final String? error;
  final int? selectedGroupId;

  List<MenuDish> get dishesInSelectedGroup {
    final m = menu;
    if (m == null) return const [];
    final gid = selectedGroupId;
    if (gid == null) return m.dishes;
    return m.dishes.where((d) => d.groupId == gid).toList();
  }

  RestaurantOrderState copyWith({
    RestaurantMenu? menu,
    bool? loading,
    String? error,
    int? selectedGroupId,
    bool clearError = false,
  }) {
    return RestaurantOrderState(
      menu: menu ?? this.menu,
      loading: loading ?? this.loading,
      error: clearError ? null : (error ?? this.error),
      selectedGroupId: selectedGroupId ?? this.selectedGroupId,
    );
  }
}

final restaurantOrderProvider = NotifierProvider.family<RestaurantOrderNotifier,
    RestaurantOrderState, int>(RestaurantOrderNotifier.new);

class RestaurantOrderNotifier extends Notifier<RestaurantOrderState> {
  RestaurantOrderNotifier(this.restaurantId);

  final int restaurantId;

  @override
  RestaurantOrderState build() {
    Future.microtask(load);
    return const RestaurantOrderState(loading: true);
  }

  Future<void> load() async {
    state = state.copyWith(loading: true, clearError: true);
    ref.read(cartProvider.notifier).ensureRestaurant(restaurantId);
    try {
      final menu =
          await ref.read(restaurantRepositoryProvider).getMenu(restaurantId);
      final firstGroup =
          menu.groups.isNotEmpty ? menu.groups.first.id : null;
      state = state.copyWith(
        menu: menu,
        loading: false,
        selectedGroupId: firstGroup,
      );
    } on ApiException catch (e) {
      state = state.copyWith(loading: false, error: e.message);
    } catch (_) {
      state = state.copyWith(loading: false, error: 'Something went wrong');
    }
  }

  void selectGroup(int groupId) {
    state = state.copyWith(selectedGroupId: groupId);
  }

  void setServiceMode(ServiceMode mode) {
    ref.read(homeProvider.notifier).setServiceMode(mode);
  }
}

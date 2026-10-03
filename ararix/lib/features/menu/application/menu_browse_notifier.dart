import 'dart:async';

import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_exception.dart';
import '../../home/data/home_models.dart';
import '../../order/data/menu_models.dart';
import '../data/menu_browse_repository.dart';

enum MenuBrowseLevel { restaurants, groups, dishes }

class MenuBrowseState {
  const MenuBrowseState({
    this.level = MenuBrowseLevel.restaurants,
    this.restaurants = const [],
    this.loading = false,
    this.error,
    this.searchQuery = '',
    this.suggestions = const [],
    this.suggesting = false,
    this.selectedRestaurant,
    this.menu,
    this.menuLoading = false,
    this.menuError,
    this.selectedGroup,
  });

  final MenuBrowseLevel level;
  final List<HomeRestaurant> restaurants;
  final bool loading;
  final String? error;
  final String searchQuery;
  final List<HomeSuggestItem> suggestions;
  final bool suggesting;
  final HomeRestaurant? selectedRestaurant;
  final RestaurantMenu? menu;
  final bool menuLoading;
  final String? menuError;
  final MenuGroup? selectedGroup;

  bool get showSuggestions =>
      level == MenuBrowseLevel.restaurants &&
      searchQuery.trim().isNotEmpty &&
      suggestions.isNotEmpty;

  List<MenuGroup> get groups {
    final m = menu;
    if (m == null) return const [];
    final used = m.dishes.map((d) => d.groupId).toSet();
    return m.groups.where((g) => used.contains(g.id)).toList();
  }

  List<MenuDish> get dishesInSelectedGroup {
    final g = selectedGroup;
    final m = menu;
    if (g == null || m == null) return const [];
    return m.dishes.where((d) => d.groupId == g.id).toList();
  }

  String get title {
    switch (level) {
      case MenuBrowseLevel.restaurants:
        return '';
      case MenuBrowseLevel.groups:
        return selectedRestaurant?.name ?? '';
      case MenuBrowseLevel.dishes:
        return selectedGroup?.name ?? '';
    }
  }

  MenuBrowseState copyWith({
    MenuBrowseLevel? level,
    List<HomeRestaurant>? restaurants,
    bool? loading,
    String? error,
    String? searchQuery,
    List<HomeSuggestItem>? suggestions,
    bool? suggesting,
    HomeRestaurant? selectedRestaurant,
    RestaurantMenu? menu,
    bool? menuLoading,
    String? menuError,
    MenuGroup? selectedGroup,
    bool clearError = false,
    bool clearSuggestions = false,
    bool clearRestaurant = false,
    bool clearMenu = false,
    bool clearGroup = false,
    bool clearMenuError = false,
  }) {
    return MenuBrowseState(
      level: level ?? this.level,
      restaurants: restaurants ?? this.restaurants,
      loading: loading ?? this.loading,
      error: clearError ? null : (error ?? this.error),
      searchQuery: searchQuery ?? this.searchQuery,
      suggestions:
          clearSuggestions ? const [] : (suggestions ?? this.suggestions),
      suggesting: suggesting ?? this.suggesting,
      selectedRestaurant: clearRestaurant
          ? null
          : (selectedRestaurant ?? this.selectedRestaurant),
      menu: clearMenu ? null : (menu ?? this.menu),
      menuLoading: menuLoading ?? this.menuLoading,
      menuError: clearMenuError ? null : (menuError ?? this.menuError),
      selectedGroup: clearGroup ? null : (selectedGroup ?? this.selectedGroup),
    );
  }
}

final menuBrowseProvider =
    NotifierProvider<MenuBrowseNotifier, MenuBrowseState>(MenuBrowseNotifier.new);

class MenuBrowseNotifier extends Notifier<MenuBrowseState> {
  Timer? _suggestDebounce;
  int _suggestToken = 0;

  @override
  MenuBrowseState build() {
    ref.onDispose(() => _suggestDebounce?.cancel());
    Future.microtask(loadRestaurants);
    return const MenuBrowseState(loading: true);
  }

  Future<void> loadRestaurants() async {
    state = state.copyWith(loading: true, clearError: true);
    try {
      final rows = await ref.read(menuBrowseRepositoryProvider).listRestaurants();
      state = state.copyWith(restaurants: rows, loading: false);
    } on ApiException catch (e) {
      state = state.copyWith(loading: false, error: e.message);
    } catch (_) {
      state = state.copyWith(loading: false, error: 'Something went wrong');
    }
  }

  void setSearchQuery(String value) {
    state = state.copyWith(searchQuery: value);
    if (value.trim().isEmpty) {
      state = state.copyWith(clearSuggestions: true, suggesting: false);
    }
    _suggestDebounce?.cancel();
    _suggestDebounce =
        Timer(const Duration(milliseconds: 220), _runSuggest);
  }

  void clearSuggestions() {
    state = state.copyWith(clearSuggestions: true, suggesting: false);
  }

  Future<void> _runSuggest() async {
    final q = state.searchQuery.trim();
    if (q.isEmpty || state.level != MenuBrowseLevel.restaurants) {
      state = state.copyWith(clearSuggestions: true, suggesting: false);
      return;
    }
    final token = ++_suggestToken;
    state = state.copyWith(suggesting: true);
    try {
      final rows = await ref.read(menuBrowseRepositoryProvider).suggest(q);
      if (token != _suggestToken) return;
      state = state.copyWith(suggestions: rows, suggesting: false);
    } catch (_) {
      if (token != _suggestToken) return;
      state = state.copyWith(clearSuggestions: true, suggesting: false);
    }
  }

  Future<void> openRestaurant(HomeRestaurant restaurant) async {
    state = state.copyWith(
      selectedRestaurant: restaurant,
      level: MenuBrowseLevel.groups,
      clearGroup: true,
      clearMenu: true,
      clearSuggestions: true,
      searchQuery: '',
      menuLoading: true,
      clearMenuError: true,
    );
    try {
      final menu = await ref
          .read(menuBrowseRepositoryProvider)
          .restaurantMenu(restaurant.id);
      state = state.copyWith(menu: menu, menuLoading: false);
    } on ApiException catch (e) {
      state = state.copyWith(menuLoading: false, menuError: e.message);
    } catch (_) {
      state = state.copyWith(
        menuLoading: false,
        menuError: 'Something went wrong',
      );
    }
  }

  void openGroup(MenuGroup group) {
    state = state.copyWith(
      selectedGroup: group,
      level: MenuBrowseLevel.dishes,
    );
  }

  void goBack() {
    switch (state.level) {
      case MenuBrowseLevel.dishes:
        state = state.copyWith(
          level: MenuBrowseLevel.groups,
          clearGroup: true,
        );
        break;
      case MenuBrowseLevel.groups:
        state = state.copyWith(
          level: MenuBrowseLevel.restaurants,
          clearRestaurant: true,
          clearMenu: true,
          clearGroup: true,
          clearMenuError: true,
        );
        break;
      case MenuBrowseLevel.restaurants:
        break;
    }
  }
}

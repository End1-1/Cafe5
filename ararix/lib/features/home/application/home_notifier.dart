import 'dart:async';

import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_exception.dart';
import '../data/home_models.dart';
import '../data/home_repository.dart';

enum ServiceMode { delivery, takeaway, dineIn }

class HomeState {
  const HomeState({
    this.feed,
    this.loading = false,
    this.error,
    this.serviceMode = ServiceMode.delivery,
    this.searchQuery = '',
    this.selectedCountryIds = const {},
    this.selectedGroupId,
    this.searchResults,
    this.searching = false,
    this.suggestions = const [],
    this.suggesting = false,
  });

  final HomeFeed? feed;
  final bool loading;
  final String? error;
  final ServiceMode serviceMode;
  final String searchQuery;
  /// Multi-select cuisine / restaurant nationality chips.
  final Set<int> selectedCountryIds;
  /// Selected home goods group (burger, pizza, …); null = not filtering by group.
  final int? selectedGroupId;
  /// When non-null, UI shows API search/filter results instead of top list.
  final List<HomeRestaurant>? searchResults;
  final bool searching;
  final List<HomeSuggestItem> suggestions;
  final bool suggesting;

  bool get hasActiveFilter => selectedCountryIds.isNotEmpty;

  bool get showSuggestions =>
      searchQuery.trim().isNotEmpty && suggestions.isNotEmpty;

  List<HomeRestaurant> get filteredRestaurants {
    // Text search only drives suggestions, not the Top Restaurants strip.
    // Dish groups on home do not filter top restaurants (Menu tab is separate).
    if (hasActiveFilter) {
      return searchResults ?? const [];
    }
    return feed?.topRestaurants ?? const [];
  }

  HomeState copyWith({
    HomeFeed? feed,
    bool? loading,
    String? error,
    ServiceMode? serviceMode,
    String? searchQuery,
    Set<int>? selectedCountryIds,
    int? selectedGroupId,
    List<HomeRestaurant>? searchResults,
    bool? searching,
    List<HomeSuggestItem>? suggestions,
    bool? suggesting,
    bool clearError = false,
    bool clearCountries = false,
    bool clearGroup = false,
    bool clearSearchResults = false,
    bool clearSuggestions = false,
  }) {
    return HomeState(
      feed: feed ?? this.feed,
      loading: loading ?? this.loading,
      error: clearError ? null : (error ?? this.error),
      serviceMode: serviceMode ?? this.serviceMode,
      searchQuery: searchQuery ?? this.searchQuery,
      selectedCountryIds:
          clearCountries ? const {} : (selectedCountryIds ?? this.selectedCountryIds),
      selectedGroupId: clearGroup ? null : (selectedGroupId ?? this.selectedGroupId),
      searchResults:
          clearSearchResults ? null : (searchResults ?? this.searchResults),
      searching: searching ?? this.searching,
      suggestions: clearSuggestions ? const [] : (suggestions ?? this.suggestions),
      suggesting: suggesting ?? this.suggesting,
    );
  }
}

final homeProvider = NotifierProvider<HomeNotifier, HomeState>(HomeNotifier.new);

class HomeNotifier extends Notifier<HomeState> {
  Timer? _debounce;
  Timer? _suggestDebounce;
  int _searchToken = 0;
  int _suggestToken = 0;

  @override
  HomeState build() {
    ref.onDispose(() {
      _debounce?.cancel();
      _suggestDebounce?.cancel();
    });
    Future.microtask(load);
    return const HomeState(loading: true);
  }

  Future<void> load() async {
    state = state.copyWith(loading: true, clearError: true);
    try {
      final feed = await ref.read(homeRepositoryProvider).getHome();
      state = state.copyWith(feed: feed, loading: false);
      if (state.hasActiveFilter) {
        await _runSearch();
      }
    } on ApiException catch (e) {
      state = state.copyWith(loading: false, error: e.message);
    } catch (_) {
      state = state.copyWith(loading: false, error: 'Something went wrong');
    }
  }

  void setServiceMode(ServiceMode mode) {
    state = state.copyWith(serviceMode: mode);
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

  /// Toggle nationality chip (multi-select). Tap again to deselect.
  void toggleCountry(int id) {
    final next = Set<int>.from(state.selectedCountryIds);
    if (next.contains(id)) {
      next.remove(id);
    } else {
      next.add(id);
    }
    state = state.copyWith(selectedCountryIds: next);
    _debounce?.cancel();
    _runSearch();
  }

  /// Visual selection only — does not filter top restaurants (Menu tab is separate).
  void toggleGroup(int id) {
    final next = state.selectedGroupId == id ? null : id;
    if (next == null) {
      state = state.copyWith(clearGroup: true);
    } else {
      state = state.copyWith(selectedGroupId: next);
    }
  }

  void clearFilters() {
    state = state.copyWith(
      clearCountries: true,
      clearGroup: true,
      searchQuery: '',
      clearSearchResults: true,
      clearSuggestions: true,
      searching: false,
      suggesting: false,
    );
    _debounce?.cancel();
    _suggestDebounce?.cancel();
  }

  Future<void> _runSuggest() async {
    final q = state.searchQuery.trim();
    if (q.isEmpty) {
      state = state.copyWith(clearSuggestions: true, suggesting: false);
      return;
    }

    final token = ++_suggestToken;
    state = state.copyWith(suggesting: true);
    try {
      final rows = await ref.read(homeRepositoryProvider).suggest(query: q);
      if (token != _suggestToken) return;
      state = state.copyWith(suggestions: rows, suggesting: false);
    } on ApiException {
      if (token != _suggestToken) return;
      state = state.copyWith(clearSuggestions: true, suggesting: false);
    } catch (_) {
      if (token != _suggestToken) return;
      state = state.copyWith(clearSuggestions: true, suggesting: false);
    }
  }

  Future<void> _runSearch() async {
    if (!state.hasActiveFilter) {
      state = state.copyWith(clearSearchResults: true, searching: false);
      return;
    }

    final token = ++_searchToken;
    state = state.copyWith(searching: true);
    try {
      final rows = await ref.read(homeRepositoryProvider).searchRestaurants(
            nationalityIds: state.selectedCountryIds.toList()..sort(),
          );
      if (token != _searchToken) return;
      state = state.copyWith(searchResults: rows, searching: false);
    } on ApiException {
      if (token != _searchToken) return;
      state = state.copyWith(searchResults: const [], searching: false);
    } catch (_) {
      if (token != _searchToken) return;
      state = state.copyWith(searchResults: const [], searching: false);
    }
  }
}

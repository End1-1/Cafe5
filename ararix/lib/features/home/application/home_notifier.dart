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
    this.selectedCountryId,
    this.searchResults,
    this.searching = false,
  });

  final HomeFeed? feed;
  final bool loading;
  final String? error;
  final ServiceMode serviceMode;
  final String searchQuery;
  final int? selectedCountryId;
  /// When non-null, UI shows API search/filter results instead of top list.
  final List<HomeRestaurant>? searchResults;
  final bool searching;

  bool get hasActiveFilter =>
      searchQuery.trim().isNotEmpty || selectedCountryId != null;

  List<HomeRestaurant> get filteredRestaurants {
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
    int? selectedCountryId,
    List<HomeRestaurant>? searchResults,
    bool? searching,
    bool clearError = false,
    bool clearCountry = false,
    bool clearSearchResults = false,
  }) {
    return HomeState(
      feed: feed ?? this.feed,
      loading: loading ?? this.loading,
      error: clearError ? null : (error ?? this.error),
      serviceMode: serviceMode ?? this.serviceMode,
      searchQuery: searchQuery ?? this.searchQuery,
      selectedCountryId:
          clearCountry ? null : (selectedCountryId ?? this.selectedCountryId),
      searchResults:
          clearSearchResults ? null : (searchResults ?? this.searchResults),
      searching: searching ?? this.searching,
    );
  }
}

final homeProvider = NotifierProvider<HomeNotifier, HomeState>(HomeNotifier.new);

class HomeNotifier extends Notifier<HomeState> {
  Timer? _debounce;
  int _searchToken = 0;

  @override
  HomeState build() {
    ref.onDispose(() => _debounce?.cancel());
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
    _debounce?.cancel();
    _debounce = Timer(const Duration(milliseconds: 350), _runSearch);
  }

  void setCountry(int? id) {
    if (id == null) {
      state = state.copyWith(clearCountry: true);
    } else {
      state = state.copyWith(selectedCountryId: id);
    }
    _debounce?.cancel();
    _runSearch();
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
            query: state.searchQuery,
            nationalityId: state.selectedCountryId,
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

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
  });

  final HomeFeed? feed;
  final bool loading;
  final String? error;
  final ServiceMode serviceMode;
  final String searchQuery;
  final int? selectedCountryId;

  List<HomeRestaurant> get filteredRestaurants {
    final list = feed?.topRestaurants ?? const [];
    final q = searchQuery.trim().toLowerCase();
    if (q.isEmpty) return list;
    return list.where((r) => r.name.toLowerCase().contains(q)).toList();
  }

  HomeState copyWith({
    HomeFeed? feed,
    bool? loading,
    String? error,
    ServiceMode? serviceMode,
    String? searchQuery,
    int? selectedCountryId,
    bool clearError = false,
    bool clearCountry = false,
  }) {
    return HomeState(
      feed: feed ?? this.feed,
      loading: loading ?? this.loading,
      error: clearError ? null : (error ?? this.error),
      serviceMode: serviceMode ?? this.serviceMode,
      searchQuery: searchQuery ?? this.searchQuery,
      selectedCountryId:
          clearCountry ? null : (selectedCountryId ?? this.selectedCountryId),
    );
  }
}

final homeProvider = NotifierProvider<HomeNotifier, HomeState>(HomeNotifier.new);

class HomeNotifier extends Notifier<HomeState> {
  @override
  HomeState build() {
    Future.microtask(load);
    return const HomeState(loading: true);
  }

  Future<void> load() async {
    state = state.copyWith(loading: true, clearError: true);
    try {
      final feed = await ref.read(homeRepositoryProvider).getHome();
      state = state.copyWith(feed: feed, loading: false);
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
  }

  void setCountry(int? id) {
    if (id == null) {
      state = state.copyWith(clearCountry: true);
    } else {
      state = state.copyWith(selectedCountryId: id);
    }
  }
}

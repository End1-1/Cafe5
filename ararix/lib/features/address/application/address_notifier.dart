import 'dart:async';

import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:geolocator/geolocator.dart';

import '../../../core/network/api_exception.dart';
import '../data/address_models.dart';
import '../data/address_repository.dart';
import '../data/maps_config.dart';
import '../data/places_service.dart';

enum AddressFlowStep {
  where,
  search,
  confirm,
  buildingType,
  details,
}

class AddressDraft {
  const AddressDraft({
    this.id,
    this.street = '',
    this.label = '',
    this.lat = MapsConfig.defaultLat,
    this.lng = MapsConfig.defaultLng,
    this.entranceLat,
    this.entranceLng,
    this.buildingType = BuildingType.house,
    this.floor = '',
    this.door = '',
    this.comment = '',
    this.adjustingEntrance = false,
  });

  final int? id;
  final String street;
  final String label;
  final double lat;
  final double lng;
  final double? entranceLat;
  final double? entranceLng;
  final BuildingType buildingType;
  final String floor;
  final String door;
  final String comment;
  final bool adjustingEntrance;

  DeliveryAddress toAddress() {
    return DeliveryAddress(
      id: id,
      label: label.trim().isNotEmpty ? label.trim() : street.trim(),
      street: street.trim().isNotEmpty ? street.trim() : label.trim(),
      lat: lat,
      lng: lng,
      entranceLat: entranceLat,
      entranceLng: entranceLng,
      buildingType: buildingType,
      floor: floor.trim().isEmpty ? null : floor.trim(),
      door: door.trim().isEmpty ? null : door.trim(),
      comment: comment.trim().isEmpty ? null : comment.trim(),
      isActive: true,
    );
  }

  AddressDraft copyWith({
    int? id,
    String? street,
    String? label,
    double? lat,
    double? lng,
    double? entranceLat,
    double? entranceLng,
    BuildingType? buildingType,
    String? floor,
    String? door,
    String? comment,
    bool? adjustingEntrance,
    bool clearEntrance = false,
  }) {
    return AddressDraft(
      id: id ?? this.id,
      street: street ?? this.street,
      label: label ?? this.label,
      lat: lat ?? this.lat,
      lng: lng ?? this.lng,
      entranceLat: clearEntrance ? null : (entranceLat ?? this.entranceLat),
      entranceLng: clearEntrance ? null : (entranceLng ?? this.entranceLng),
      buildingType: buildingType ?? this.buildingType,
      floor: floor ?? this.floor,
      door: door ?? this.door,
      comment: comment ?? this.comment,
      adjustingEntrance: adjustingEntrance ?? this.adjustingEntrance,
    );
  }

  factory AddressDraft.fromAddress(DeliveryAddress a) {
    return AddressDraft(
      id: a.id,
      street: a.street,
      label: a.label,
      lat: a.lat,
      lng: a.lng,
      entranceLat: a.entranceLat,
      entranceLng: a.entranceLng,
      buildingType: a.buildingType,
      floor: a.floor ?? '',
      door: a.door ?? '',
      comment: a.comment ?? '',
    );
  }
}

class AddressState {
  const AddressState({
    this.addresses = const [],
    this.active,
    this.loading = false,
    this.saving = false,
    this.error,
    this.step = AddressFlowStep.where,
    this.draft = const AddressDraft(),
    this.searchQuery = '',
    this.suggestions = const [],
    this.suggesting = false,
  });

  final List<DeliveryAddress> addresses;
  final DeliveryAddress? active;
  final bool loading;
  final bool saving;
  final String? error;
  final AddressFlowStep step;
  final AddressDraft draft;
  final String searchQuery;
  final List<PlaceSuggestion> suggestions;
  final bool suggesting;

  AddressState copyWith({
    List<DeliveryAddress>? addresses,
    DeliveryAddress? active,
    bool? loading,
    bool? saving,
    String? error,
    AddressFlowStep? step,
    AddressDraft? draft,
    String? searchQuery,
    List<PlaceSuggestion>? suggestions,
    bool? suggesting,
    bool clearError = false,
    bool clearActive = false,
    bool clearSuggestions = false,
  }) {
    return AddressState(
      addresses: addresses ?? this.addresses,
      active: clearActive ? null : (active ?? this.active),
      loading: loading ?? this.loading,
      saving: saving ?? this.saving,
      error: clearError ? null : (error ?? this.error),
      step: step ?? this.step,
      draft: draft ?? this.draft,
      searchQuery: searchQuery ?? this.searchQuery,
      suggestions:
          clearSuggestions ? const [] : (suggestions ?? this.suggestions),
      suggesting: suggesting ?? this.suggesting,
    );
  }
}

final addressProvider =
    NotifierProvider<AddressNotifier, AddressState>(AddressNotifier.new);

class AddressNotifier extends Notifier<AddressState> {
  Timer? _suggestDebounce;
  Timer? _pinGeocode;
  int _suggestToken = 0;

  @override
  AddressState build() {
    ref.onDispose(() {
      _suggestDebounce?.cancel();
      _pinGeocode?.cancel();
    });
    return const AddressState();
  }

  Future<void> load() async {
    state = state.copyWith(loading: true, clearError: true);
    try {
      final result = await ref.read(addressRepositoryProvider).list();
      state = state.copyWith(
        addresses: result.addresses,
        active: result.active,
        loading: false,
        clearActive: result.active == null,
      );
    } on ApiException catch (e) {
      state = state.copyWith(loading: false, error: e.message);
    } catch (_) {
      state = state.copyWith(loading: false, error: 'Something went wrong');
    }
  }

  void openWhere() {
    state = state.copyWith(
      step: AddressFlowStep.where,
      draft: const AddressDraft(),
      clearSuggestions: true,
      searchQuery: '',
      clearError: true,
    );
    load();
  }

  void goSearch() {
    state = state.copyWith(
      step: AddressFlowStep.search,
      clearSuggestions: true,
      searchQuery: '',
    );
  }

  void goConfirm() {
    state = state.copyWith(step: AddressFlowStep.confirm);
  }

  void goBuildingType() {
    state = state.copyWith(step: AddressFlowStep.buildingType);
  }

  void goDetails() {
    state = state.copyWith(step: AddressFlowStep.details);
  }

  void back() {
    switch (state.step) {
      case AddressFlowStep.where:
        break;
      case AddressFlowStep.search:
        state = state.copyWith(step: AddressFlowStep.where);
      case AddressFlowStep.confirm:
        state = state.copyWith(step: AddressFlowStep.search);
      case AddressFlowStep.buildingType:
        state = state.copyWith(step: AddressFlowStep.confirm);
      case AddressFlowStep.details:
        state = state.copyWith(step: AddressFlowStep.buildingType);
    }
  }

  void editExisting(DeliveryAddress address) {
    state = state.copyWith(
      draft: AddressDraft.fromAddress(address),
      step: AddressFlowStep.confirm,
      clearError: true,
    );
  }

  void startNewAddress() {
    state = state.copyWith(
      step: AddressFlowStep.search,
      draft: const AddressDraft(),
      clearSuggestions: true,
      searchQuery: '',
      clearError: true,
    );
  }

  Future<bool> deleteAddress(DeliveryAddress address) async {
    final id = address.id;
    if (id == null) return false;
    state = state.copyWith(saving: true, clearError: true);
    try {
      await ref.read(addressRepositoryProvider).delete(id);
      await load();
      return true;
    } on ApiException catch (e) {
      state = state.copyWith(saving: false, error: e.message);
      return false;
    } catch (_) {
      state = state.copyWith(saving: false, error: 'Something went wrong');
      return false;
    }
  }

  Future<bool> selectSaved(DeliveryAddress address) async {
    final id = address.id;
    if (id == null) return false;
    state = state.copyWith(saving: true, clearError: true);
    try {
      final active =
          await ref.read(addressRepositoryProvider).setActive(id);
      state = state.copyWith(
        active: active ?? address.copyWith(isActive: true),
        saving: false,
      );
      await load();
      return true;
    } on ApiException catch (e) {
      state = state.copyWith(saving: false, error: e.message);
      return false;
    } catch (_) {
      state = state.copyWith(saving: false, error: 'Something went wrong');
      return false;
    }
  }

  void setSearchQuery(String value) {
    state = state.copyWith(searchQuery: value);
    _suggestDebounce?.cancel();
    _suggestDebounce =
        Timer(const Duration(milliseconds: 280), _runSuggest);
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
      final rows = await ref.read(placesServiceProvider).autocomplete(q);
      if (token != _suggestToken) return;
      state = state.copyWith(suggestions: rows, suggesting: false);
    } catch (_) {
      if (token != _suggestToken) return;
      state = state.copyWith(clearSuggestions: true, suggesting: false);
    }
  }

  Future<void> pickSuggestion(PlaceSuggestion suggestion) async {
    final places = ref.read(placesServiceProvider);
    final resolved = await places.resolvePlace(suggestion);
    if (resolved == null) return;
    final lat = resolved.lat ?? MapsConfig.defaultLat;
    final lng = resolved.lng ?? MapsConfig.defaultLng;
    var street = resolved.primaryText;
    final reverse = await places.reverseGeocode(lat, lng);
    if (reverse != null && reverse.trim().isNotEmpty) {
      street = reverse.trim();
    }
    state = state.copyWith(
      draft: state.draft.copyWith(
        street: street,
        label: state.draft.label.isEmpty ? street : state.draft.label,
        lat: lat,
        lng: lng,
      ),
      step: AddressFlowStep.confirm,
      clearSuggestions: true,
    );
  }

  Future<void> useCurrentLocation() async {
    state = state.copyWith(saving: true, clearError: true);
    try {
      final enabled = await Geolocator.isLocationServiceEnabled();
      if (!enabled) {
        state = state.copyWith(
          saving: false,
          error: 'Location services disabled',
        );
        return;
      }
      var permission = await Geolocator.checkPermission();
      if (permission == LocationPermission.denied) {
        permission = await Geolocator.requestPermission();
      }
      if (permission == LocationPermission.denied ||
          permission == LocationPermission.deniedForever) {
        state = state.copyWith(saving: false, error: 'Location permission denied');
        return;
      }
      final pos = await Geolocator.getCurrentPosition(
        locationSettings: const LocationSettings(
          accuracy: LocationAccuracy.high,
        ),
      );
      final reverse = await ref
          .read(placesServiceProvider)
          .reverseGeocode(pos.latitude, pos.longitude);
      final street = reverse?.trim().isNotEmpty == true
          ? reverse!.trim()
          : 'Current location';
      state = state.copyWith(
        saving: false,
        draft: state.draft.copyWith(
          street: street,
          label: street,
          lat: pos.latitude,
          lng: pos.longitude,
        ),
        step: AddressFlowStep.confirm,
      );
    } catch (_) {
      state = state.copyWith(saving: false, error: 'Could not get location');
    }
  }

  void updatePin(double lat, double lng) {
    state = state.copyWith(draft: state.draft.copyWith(lat: lat, lng: lng));
    _pinGeocode?.cancel();
    _pinGeocode = Timer(const Duration(milliseconds: 450), () {
      refreshStreetFromPin();
    });
  }

  Future<void> refreshStreetFromPin() async {
    final reverse = await ref
        .read(placesServiceProvider)
        .reverseGeocode(state.draft.lat, state.draft.lng);
    if (reverse == null || reverse.trim().isEmpty) return;
    state = state.copyWith(
      draft: state.draft.copyWith(street: reverse.trim()),
    );
  }

  void setBuildingType(BuildingType type) {
    state = state.copyWith(draft: state.draft.copyWith(buildingType: type));
  }

  void setFloor(String value) {
    state = state.copyWith(draft: state.draft.copyWith(floor: value));
  }

  void setDoor(String value) {
    state = state.copyWith(draft: state.draft.copyWith(door: value));
  }

  void setComment(String value) {
    state = state.copyWith(draft: state.draft.copyWith(comment: value));
  }

  void setLabel(String value) {
    state = state.copyWith(draft: state.draft.copyWith(label: value));
  }

  void setAdjustingEntrance(bool value) {
    state = state.copyWith(
      draft: state.draft.copyWith(adjustingEntrance: value),
    );
  }

  void updateEntrancePin(double lat, double lng) {
    state = state.copyWith(
      draft: state.draft.copyWith(entranceLat: lat, entranceLng: lng),
    );
  }

  Future<bool> saveDraft() async {
    state = state.copyWith(saving: true, clearError: true);
    try {
      final saved =
          await ref.read(addressRepositoryProvider).save(state.draft.toAddress());
      state = state.copyWith(
        active: saved,
        saving: false,
        step: AddressFlowStep.where,
      );
      await load();
      return true;
    } on ApiException catch (e) {
      state = state.copyWith(saving: false, error: e.message);
      return false;
    } catch (_) {
      state = state.copyWith(saving: false, error: 'Something went wrong');
      return false;
    }
  }
}

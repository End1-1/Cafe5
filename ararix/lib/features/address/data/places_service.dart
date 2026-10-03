import 'dart:ui';

import 'package:dio/dio.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import 'address_models.dart';
import 'maps_config.dart';

final placesServiceProvider = Provider<PlacesService>((ref) {
  return PlacesService();
});

/// Google Places Autocomplete + Geocoding via REST.
/// Without [MapsConfig.isConfigured] falls back to manual typed address.
class PlacesService {
  PlacesService({Dio? dio}) : _dio = dio ?? Dio();

  final Dio _dio;

  String get _language {
    final code = PlatformDispatcher.instance.locale.languageCode;
    if (code == 'hy' || code == 'ru' || code == 'en') return code;
    return 'hy';
  }

  Future<List<PlaceSuggestion>> autocomplete(String query) async {
    final q = query.trim();
    if (q.isEmpty) return const [];

    if (!MapsConfig.isConfigured) {
      return [
        PlaceSuggestion(
          placeId: 'manual:$q',
          primaryText: q,
          secondaryText: '',
          lat: MapsConfig.defaultLat,
          lng: MapsConfig.defaultLng,
        ),
      ];
    }

    try {
      final response = await _dio.get<Map<String, dynamic>>(
        'https://maps.googleapis.com/maps/api/place/autocomplete/json',
        queryParameters: {
          'input': q,
          'key': MapsConfig.apiKey,
          'language': _language,
          'components': 'country:am',
        },
      );
      final status = response.data?['status']?.toString();
      if (status != 'OK' && status != 'ZERO_RESULTS') {
        return const [];
      }
      final preds = response.data?['predictions'] as List<dynamic>? ?? [];
      return preds.map((raw) {
        final m = Map<String, dynamic>.from(raw as Map);
        final structured = m['structured_formatting'] as Map?;
        return PlaceSuggestion(
          placeId: m['place_id']?.toString() ?? '',
          primaryText: structured?['main_text']?.toString() ??
              m['description']?.toString() ??
              '',
          secondaryText: structured?['secondary_text']?.toString() ?? '',
        );
      }).where((p) => p.placeId.isNotEmpty).toList();
    } catch (_) {
      return const [];
    }
  }

  Future<PlaceSuggestion?> resolvePlace(PlaceSuggestion suggestion) async {
    if (suggestion.placeId.startsWith('manual:')) {
      return suggestion;
    }
    if (!MapsConfig.isConfigured || suggestion.placeId.isEmpty) {
      return suggestion.copyWithCoords(
        MapsConfig.defaultLat,
        MapsConfig.defaultLng,
      );
    }

    try {
      final response = await _dio.get<Map<String, dynamic>>(
        'https://maps.googleapis.com/maps/api/place/details/json',
        queryParameters: {
          'place_id': suggestion.placeId,
          'fields': 'geometry,formatted_address,name',
          'key': MapsConfig.apiKey,
          'language': _language,
        },
      );
      final result = response.data?['result'] as Map?;
      final loc = (result?['geometry'] as Map?)?['location'] as Map?;
      final lat = (loc?['lat'] as num?)?.toDouble();
      final lng = (loc?['lng'] as num?)?.toDouble();
      final formatted = result?['formatted_address']?.toString();
      return PlaceSuggestion(
        placeId: suggestion.placeId,
        primaryText: formatted ?? suggestion.primaryText,
        secondaryText: suggestion.secondaryText,
        lat: lat ?? MapsConfig.defaultLat,
        lng: lng ?? MapsConfig.defaultLng,
      );
    } catch (_) {
      return null;
    }
  }

  Future<String?> reverseGeocode(double lat, double lng) async {
    if (!MapsConfig.isConfigured) {
      return null;
    }
    try {
      final response = await _dio.get<Map<String, dynamic>>(
        'https://maps.googleapis.com/maps/api/geocode/json',
        queryParameters: {
          'latlng': '$lat,$lng',
          'key': MapsConfig.apiKey,
          'language': _language,
        },
      );
      final results = response.data?['results'] as List<dynamic>? ?? [];
      if (results.isEmpty) return null;
      final first = Map<String, dynamic>.from(results.first as Map);
      return first['formatted_address']?.toString();
    } catch (_) {
      return null;
    }
  }
}

extension on PlaceSuggestion {
  PlaceSuggestion copyWithCoords(double lat, double lng) {
    return PlaceSuggestion(
      placeId: placeId,
      primaryText: primaryText,
      secondaryText: secondaryText,
      lat: lat,
      lng: lng,
    );
  }
}

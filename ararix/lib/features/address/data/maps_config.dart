import 'maps_key.dart';

/// Google Maps / Places key.
///
/// The value lives in gitignored `maps_key.dart`, generated from
/// `secrets.properties` by `tool/sync_maps_secrets.ps1`.
/// Android reads `secrets.properties`; iOS reads `MapsSecrets.xcconfig`.
class MapsConfig {
  static const apiKey = mapsApiKey;

  static const _placeholder = 'YOUR_GOOGLE_MAPS_API_KEY';

  /// True when a usable Maps/Places key is available for HTTP + native maps.
  static bool get isConfigured {
    final key = apiKey.trim();
    return key.isNotEmpty && key != _placeholder;
  }

  /// Yerevan center — default when no address / location yet.
  static const defaultLat = 40.1872;
  static const defaultLng = 44.5121;
}

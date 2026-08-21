class BackendHost {
  const BackendHost({
    required this.id,
    required this.label,
    required this.baseUrl,
    this.enabled = true,
  });

  final String id;
  final String label;
  final String baseUrl;
  final bool enabled;
}

class BackendHosts {
  static const local = BackendHost(
    id: 'local',
    label: 'ararix.local',
    baseUrl: 'http://ararix.local/engine/v2',
  );

  static const development = BackendHost(
    id: 'development',
    label: 'development.picassocloud.com',
    baseUrl: 'https://development.picassocloud.com/engine/v2',
  );

  /// Production URL is unknown for now — keep as disabled placeholder.
  static const production = BackendHost(
    id: 'production',
    label: 'Production (TBD)',
    baseUrl: '',
    enabled: false,
  );

  static const List<BackendHost> all = [local, development, production];

  static const defaultBaseUrl = String.fromEnvironment(
    'API_BASE',
    defaultValue: 'http://ararix.local/engine/v2',
  );

  static BackendHost? byBaseUrl(String? url) {
    if (url == null || url.isEmpty) return null;
    for (final host in all) {
      if (host.baseUrl == url) return host;
    }
    return null;
  }
}

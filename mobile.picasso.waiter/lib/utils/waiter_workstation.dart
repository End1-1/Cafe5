import 'dart:convert';
import 'dart:io' show Platform;

import 'package:picassowaiter/utils/http_query.dart';
import 'package:picassowaiter/utils/prefs.dart';

/// Workstation config (cashbox, hall, printer) — same as desktop Waiter [DlgScreen].
class WaiterWorkstation {
  static const routeGetConfig = 'engine/v2/common/workstation/get-config';
  static const int typeWaiter = 1;

  static String workstationName() {
    final stored = prefs.string('workstation_name');
    if (stored.isNotEmpty) {
      return stored;
    }
    try {
      return Platform.localHostname;
    } catch (_) {
      return 'mobile-waiter';
    }
  }

  static String stationAccount() {
    final stored = prefs.string('station_account');
    if (stored.isNotEmpty) {
      return stored;
    }
    try {
      final user =
          Platform.environment['USERNAME'] ?? Platform.environment['USER'];
      if (user != null && user.isNotEmpty) {
        return user;
      }
    } catch (_) {}
    return 'mobile';
  }

  /// Returns [f_cashbox_id] from workstation row, or null if missing / request failed.
  static Future<int?> loadCashboxFromWorkstation() async {
    final r = await HttpQuery(routeGetConfig).request({
      'type': typeWaiter,
      'station_account': stationAccount(),
      'workstation': workstationName(),
    });
    if (r['status'] == 0) {
      return null;
    }

    final cashboxId = cashboxIdFromConfigField(r['f_config']);
    mergeWorkstationConfig(r);
    return cashboxId;
  }

  static void mergeWorkstationConfig(Map<String, dynamic> r) {
    Map<String, dynamic> wsCfg = {};
    final raw = r['f_config'];
    if (raw is String && raw.isNotEmpty) {
      try {
        final decoded = jsonDecode(raw);
        if (decoded is Map) {
          wsCfg = Map<String, dynamic>.from(decoded);
        }
      } catch (_) {}
    } else if (raw is Map) {
      wsCfg = Map<String, dynamic>.from(raw);
    }

    Map<String, dynamic> cfg = {};
    final existing = prefs.string('config');
    if (existing.isNotEmpty) {
      try {
        final decoded = jsonDecode(existing);
        if (decoded is Map) {
          cfg = Map<String, dynamic>.from(decoded);
        }
      } catch (_) {}
    }
    cfg.addAll(wsCfg);
    prefs.setString('config', jsonEncode(cfg));
    prefs.init();
  }

  static int? cashboxIdFromConfigField(dynamic fConfig) {
    Map<String, dynamic>? cfg;
    if (fConfig is String && fConfig.isNotEmpty) {
      try {
        final decoded = jsonDecode(fConfig);
        if (decoded is Map) {
          cfg = Map<String, dynamic>.from(decoded);
        }
      } catch (_) {}
    } else if (fConfig is Map) {
      cfg = Map<String, dynamic>.from(fConfig);
    }
    if (cfg == null) {
      return null;
    }
    final id = cfg['f_cashbox_id'] ?? cfg['cashbox_id'];
    if (id == null) {
      return null;
    }
    final n = int.tryParse('$id');
    return (n != null && n > 0) ? n : null;
  }
}

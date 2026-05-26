import 'package:picassowaiter/utils/waiter_order_v2.dart';

/// Parses engine v2 waiter hall/menu responses for the mobile dashboard.
class WaiterInitV2 {
  static const routeHallGet = 'engine/v2/waiter/hall/get';
  static const routeMenuGet = 'engine/v2/waiter/menu/get';

  static Map<String, dynamic> unwrapPayload(dynamic raw) {
    if (raw is! Map) return {};
    final d = Map<String, dynamic>.from(raw);
    if (d['halls'] != null || d['tables'] != null) return d;
    if (d['groups'] != null || d['dishes'] != null) return d;
    final nested = d['data'];
    if (nested is Map) {
      return Map<String, dynamic>.from(nested);
    }
    return d;
  }

  static void applyHall(
    dynamic raw, {
    required List<dynamic> halls,
    required List<dynamic> tables,
    required Map openTables,
    required Map filteredTables,
  }) {
    final payload = unwrapPayload(raw);
    final hallList = (payload['halls'] as List?) ?? [];
    final tableList = (payload['tables'] as List?) ?? [];

    halls.clear();
    tables.clear();
    openTables.clear();
    filteredTables.clear();

    for (final h in hallList) {
      if (h is! Map) continue;
      final hall = Map<String, dynamic>.from(h);
      halls.add(hall);
      final id = _asInt(hall['f_id']);
      if (id != null) {
        filteredTables[id] = [];
      }
    }

    for (final t in tableList) {
      if (t is! Map) continue;
      final table = Map<String, dynamic>.from(t);
      tables.add(table);
      final hallId = _asInt(table['f_hall']);
      if (hallId != null) {
        filteredTables.putIfAbsent(hallId, () => []).add(table);
      }
      final tableId = _asInt(table['f_id']);
      final state = _asInt(table['f_table_state']) ?? 1;
      if (tableId != null && state >= 2) {
        openTables[tableId] = _openOrderFromTable(table, state);
      }
    }
  }

  static Map<String, dynamic> _openOrderFromTable(
    Map<String, dynamic> table,
    int tableState,
  ) {
    final data = WaiterOrderV2.parseJsonMap(table['f_data']);
    var precheck = 0;
    if (tableState >= 3) {
      precheck = 1;
    } else {
      final pc = data['f_precheck'];
      if (pc is num) {
        precheck = pc.toInt();
      } else if (pc is String) {
        precheck = int.tryParse(pc) ?? 0;
      } else if (pc == true) {
        precheck = 1;
      }
    }
    return {
      'f_table': table['f_id'],
      'f_staff': table['f_staff'],
      'f_staffname': table['f_current_staff_name'] ?? '',
      'f_amounttotal': table['f_amount'] ?? 0,
      'f_precheck': precheck,
      'f_prefix': table['f_prefix'] ?? '',
    };
  }

  static int? _asInt(dynamic v) {
    if (v == null) return null;
    if (v is int) return v;
    if (v is num) return v.toInt();
    return int.tryParse(v.toString());
  }
}

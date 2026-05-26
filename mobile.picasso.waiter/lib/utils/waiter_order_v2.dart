import 'dart:convert';

/// Maps engine v2 waiter order API responses to the shapes used by the mobile UI.
class WaiterOrderV2 {
  static const routeOpenTable = 'engine/v2/waiter/order/open-table';
  static const routeAddDish = 'engine/v2/waiter/order/add-dish';
  static const routeSetDishQty = 'engine/v2/waiter/order/set-dish-qty';
  static const routePrintServiceCheck = 'engine/v2/waiter/order/print-service-check';
  static const routePrintServicePreorder =
      'engine/v2/waiter/order/print-service-of-preorder';
  static const routeUnlockTable = 'engine/v2/waiter/order/unlock-table';
  static const routeRemovePackage =
      'engine/v2/waiter/order/remove-package-with-children';
  static const routeGetRemoveReason = 'engine/v2/waiter/menu/get-remove-reason';

  static const dishStateNone = 0;
  static const dishStateOk = 1;
  static const dishStateMistake = 2;
  static const dishStateVoid = 3;
  static const dishStateSet = 4;

  static const goodsTypeGoods = 1;
  static const goodsTypeDish = 2;
  static const goodsTypeModificator = 4;
  static const goodsTypePackage = 5;

  static const orderStatePreorder = 5;

  static Map<String, dynamic> parseJsonMap(dynamic v) {
    if (v == null) return {};
    if (v is Map<String, dynamic>) return Map<String, dynamic>.from(v);
    if (v is Map) return Map<String, dynamic>.from(v);
    if (v is String && v.isNotEmpty) {
      try {
        final d = jsonDecode(v);
        if (d is Map) return Map<String, dynamic>.from(d);
      } catch (_) {}
    }
    return {};
  }

  /// API may return { status, data: { order } } or { order } inside data.
  static Map<String, dynamic>? orderFromResponse(dynamic result) {
    if (result == null) return null;
    if (result is! Map) return null;
    final m = Map<String, dynamic>.from(result);
    final data = m['data'];
    if (data is Map && data['order'] != null) {
      final o = data['order'];
      if (o is Map) return Map<String, dynamic>.from(o);
    }
    if (m['order'] != null) {
      final o = m['order'];
      if (o is Map) return Map<String, dynamic>.from(o);
    }
    return null;
  }

  static Map<String, dynamic> normalizeHeader(Map<String, dynamic> raw) {
    final data = parseJsonMap(raw['f_data']);
    final merged = Map<String, dynamic>.from(raw);
    merged['f_servicefactor'] = data['f_service_factor'] ?? raw['f_servicefactor'] ?? 0;
    merged['f_discountfactor'] = data['f_discount_factor'] ?? raw['f_discountfactor'] ?? 0;
    merged['f_precheck'] = data['f_precheck'] ?? raw['f_precheck'] ?? 0;
    merged['_f_data'] = data;
    return merged;
  }

  /// One o_goods row -> legacy UI dish map (f_name, f_qty1, f_qty2 printed flag, etc.)
  static Map<String, dynamic> dishRowToUi(Map<String, dynamic> row) {
    final fd = parseJsonMap(row['f_data']);
    final printed = fd['f_printed'] == true || fd['f_printed'] == 1;
    final qty = _toDouble(row['f_qty']);
    final state = row['f_state'] is int ? row['f_state'] as int : int.tryParse('${row['f_state']}') ?? 0;
    return {
      'f_id': row['f_id']?.toString() ?? '',
      'f_name': row['f_dish_name'] ?? '',
      'f_dish': row['f_dish'],
      'f_type': row['f_type'] is int
          ? row['f_type'] as int
          : int.tryParse('${row['f_type']}') ?? 0,
      'f_qty1': qty,
      'f_qty2': printed ? 1.0 : 0.0,
      'f_state': state,
      'f_emarks': row['f_emarks'],
      'f_comment': fd['f_comment'] ?? '',
      'f_price': row['f_price'],
      'f_store': row['f_store'],
      '_f_data': fd,
    };
  }

  static bool isPrintedDishRow(Map<String, dynamic> row) {
    if (((row['f_qty2'] as num?) ?? 0) > 0) return true;
    final fd = parseJsonMap(row['_f_data']);
    return fd['f_printed'] == true || fd['f_printed'] == 1;
  }

  /// Same visibility rules as [DlgOrder::updateRemovedDishesVisibility] with mShowRemoved=false.
  static bool isVisibleDishRow(Map<String, dynamic> row, {bool showRemoved = false}) {
    final state = row['f_state'] is int
        ? row['f_state'] as int
        : int.tryParse('${row['f_state']}') ?? 0;
    if (state == dishStateNone) return false;
    if (state == dishStateOk || state == dishStateSet) return true;
    return showRemoved;
  }

  static bool canRemoveDishRow(Map<String, dynamic> row) {
    final state = row['f_state'] is int
        ? row['f_state'] as int
        : int.tryParse('${row['f_state']}') ?? 0;
    if (state != dishStateOk) return false;
    final type = row['f_type'] is int
        ? row['f_type'] as int
        : int.tryParse('${row['f_type']}') ?? 0;
    if (type == goodsTypePackage) return true;
    if (type == goodsTypeGoods || type == goodsTypeDish) return true;
    if (type == goodsTypeModificator) return true;
    return false;
  }

  static double _toDouble(dynamic v) {
    if (v == null) return 0;
    if (v is double) return v;
    if (v is int) return v.toDouble();
    return double.tryParse(v.toString()) ?? 0;
  }

  static List<Map<String, dynamic>> dishesFromOrder(
    Map<String, dynamic> order, {
    bool showRemoved = false,
  }) {
    final list = order['dishes'];
    if (list is! List) return [];
    return list
        .whereType<Map>()
        .map((e) => dishRowToUi(Map<String, dynamic>.from(e)))
        .where((row) => isVisibleDishRow(row, showRemoved: showRemoved))
        .toList();
  }

  static Map<String, dynamic> menuItemToFData(Map<String, dynamic> e) {
    final fd = <String, dynamic>{};
    if (e['f_data'] != null) {
      fd.addAll(parseJsonMap(e['f_data']));
    }
    void copy(String key) {
      if (e[key] != null) fd[key] = e[key];
    }

    copy('f_adgcode');
    copy('f_hourly_payment');
    copy('f_hourly_rule');
    copy('f_emarks');
    return fd;
  }

  static int as01(dynamic v) {
    if (v == null) return 0;
    if (v is bool) return v ? 1 : 0;
    if (v is num) return v != 0 ? 1 : 0;
    final s = v.toString().toLowerCase();
    if (s == 'true' || s == '1' || s == 'yes') return 1;
    return 0;
  }
}

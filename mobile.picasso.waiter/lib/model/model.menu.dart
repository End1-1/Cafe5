part of 'model.dart';

class Menu {
  static final Menu _menu = Menu._internal();

  factory Menu() {
    return _menu;
  }

  Menu._internal();

  final menu = {};

  /// Menu + dishes from `engine/v2/waiter/menu/get` (same tree as desktop Waiter).
  void buildFromV2(dynamic raw) {
    menu.clear();
    final payload = WaiterInitV2.unwrapPayload(raw);
    final groupsList = (payload['groups'] as List?) ?? [];
    final dishesList = (payload['dishes'] as List?) ?? [];

    final groupById = <int, Map<String, dynamic>>{};
    for (final g in groupsList) {
      if (g is! Map) continue;
      final gm = Map<String, dynamic>.from(g);
      final id = _id(gm['f_id']);
      if (id != null) groupById[id] = gm;
    }

    for (final rawDish in dishesList) {
      if (rawDish is! Map) continue;
      final dish = Map<String, dynamic>.from(rawDish);
      final menuName = '${dish['f_name'] ?? 'Menu'}';
      final groupId = _id(dish['f_group']) ?? 0;
      final part1 = _part1Name(groupId, groupById);
      final part2 = _part2Name(groupId, groupById);
      final row = _menuRowFromV2Dish(dish, menuName, part1, part2);

      menu.putIfAbsent(menuName, () => {});
      final m = menu[menuName] as Map;
      m.putIfAbsent(part1, () => {});
      final p1 = m[part1] as Map;
      p1.putIfAbsent(part2, () => <Map<String, dynamic>>[]);
      (p1[part2] as List).add(row);
    }
    if (kDebugMode) {
      print(menu);
    }
  }

  void build(dynamic d) {
    menu.clear();
    for (final k in d['d_menu'].keys) {
      final o = d['d_menu'][k];
      if (!menu.containsKey(o['f_menuname'])) {
        menu[o['f_menuname']] = {};
      }
      final m = menu[o['f_menuname']];
      if (!m.containsKey(o['f_part1name'])) {
        m[o['f_part1name']] = {};
      }
      final p1 = m[o['f_part1name']];
      if (!p1.containsKey(o['f_part2name'])) {
        p1[o['f_part2name']] = []..add(o);
      }
      final p2 = p1[o['f_part2name']];
      p2.add(o);
    }
    if (kDebugMode) {
      print(menu);
    }
  }

  List<dynamic> part1() {
    final l = [];
    for (final k in menu.keys) {
      l.addAll(menu[k].keys);
    }
    return l.toSet().toList();
  }

  List<dynamic> part2(String menufilter, String part1filter) {
    final l = <dynamic>[];
    final m = menu[menufilter];
    if (m is! Map) return l;
    if (part1filter.isEmpty) {
      for (final k in m.keys) {
        final p1 = m[k];
        if (p1 is Map) {
          l.addAll(p1.keys);
        }
      }
    } else if (m[part1filter] is Map) {
      l.addAll((m[part1filter] as Map).keys);
    }
    return l;
  }

  List<dynamic> dishes(
    String menufilter,
    String part2filter, {
    String part1filter = '',
  }) {
    final m = menu[menufilter];
    if (m is! Map) return [];

    if (part1filter.isNotEmpty && m[part1filter] is Map) {
      final p1 = m[part1filter] as Map;
      if (part2filter.isNotEmpty && p1.containsKey(part2filter)) {
        return List<dynamic>.from(p1[part2filter] as List);
      }
      if (part2filter.isEmpty) {
        final all = <dynamic>[];
        for (final sub in p1.values) {
          if (sub is List) {
            all.addAll(sub);
          }
        }
        return all;
      }
      if (p1.containsKey('')) {
        return List<dynamic>.from(p1[''] as List);
      }
    }

    for (final k in m.keys) {
      final p1 = m[k];
      if (p1 is Map && p1.containsKey(part2filter)) {
        return List<dynamic>.from(p1[part2filter] as List);
      }
    }
    return [];
  }

  dynamic firstDishOfBarcode(String menuFilter, String barcode) {
    final m = menu[menuFilter];
    for (final l1 in m.keys) {
      final l1m = m[l1];
      for (final l2 in l1m.keys) {
        final l2m = m[l1][l2];
        for (final e in l2m) {
          if (_barcodeMatches(e['f_barcode'], barcode)) {
            final result = <String, dynamic>{};
            result.addAll(Map<String, dynamic>.from(e as Map));
            return result;
          }
        }
      }
    }
    return null;
  }

  static int? _id(dynamic v) {
    if (v == null) return null;
    if (v is int) return v;
    if (v is num) return v.toInt();
    return int.tryParse(v.toString());
  }

  static String _part1Name(int groupId, Map<int, Map<String, dynamic>> groupById) {
    var g = groupById[groupId];
    if (g == null) return '';
    var parent = _id(g['f_parent']) ?? 0;
    if (parent == 0) {
      return '${g['f_name'] ?? ''}';
    }
    while (parent != 0) {
      final p = groupById[parent];
      if (p == null) break;
      final pp = _id(p['f_parent']) ?? 0;
      if (pp == 0) {
        return '${p['f_name'] ?? ''}';
      }
      parent = pp;
      g = p;
    }
    return '${g?['f_name'] ?? ''}';
  }

  static String _part2Name(int groupId, Map<int, Map<String, dynamic>> groupById) {
    final g = groupById[groupId];
    if (g == null) return '';
    final parent = _id(g['f_parent']) ?? 0;
    if (parent == 0) {
      return '${g['f_name'] ?? ''}';
    }
    return '${g['f_name'] ?? ''}';
  }

  static Map<String, dynamic> _menuRowFromV2Dish(
    Map<String, dynamic> dish,
    String menuName,
    String part1,
    String part2,
  ) {
    final fd = WaiterOrderV2.parseJsonMap(dish['f_data']);
    return {
      'f_dish': dish['f_dish'],
      'f_name': dish['f_goods_name'],
      'f_price': dish['f_price'],
      'f_menu': dish['f_menu'],
      'f_menuname': menuName,
      'f_part1name': part1,
      'f_part2name': part2,
      'f_barcode': dish['f_scancode'],
      'f_store': dish['f_store'],
      'f_print1': dish['f_print1'],
      'f_print2': dish['f_print2'],
      'f_data': dish['f_data'],
      'f_service': fd['f_count_service'],
      'f_discount': fd['f_count_discount'],
      'f_group': dish['f_group'],
      'f_group_name': dish['f_group_name'],
    };
  }

  static bool _barcodeMatches(dynamic field, String barcode) {
    if (field == null || barcode.isEmpty) return false;
    final s = field.toString();
    if (s == barcode) return true;
    for (final part in s.split(',')) {
      if (part.trim() == barcode) return true;
    }
    return false;
  }
}

import 'dart:convert';

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:intl/intl.dart';
import 'package:shared_preferences/shared_preferences.dart';

extension Prefs on SharedPreferences {
  static GlobalKey<NavigatorState> navigatorKey = GlobalKey<NavigatorState>();
  static final regex = RegExp(r"([.]*0+)(?!.*\d)");
  static Map<String, dynamic> config = {};

  static const int defaultCashboxId = 1;

  int cashboxId() => getInt('cashbox_id') ?? defaultCashboxId;

  void applyCashboxId(int cashboxId) {
    if (cashboxId <= 0) {
      return;
    }
    setInt('cashbox_id', cashboxId);
    Map<String, dynamic> cfg = {};
    final raw = string('config');
    if (raw.isNotEmpty) {
      try {
        final decoded = jsonDecode(raw);
        if (decoded is Map) {
          cfg = Map<String, dynamic>.from(decoded);
        }
      } catch (_) {}
    }
    cfg['cashbox_id'] = cashboxId;
    cfg['f_cashbox_id'] = cashboxId;
    setString('config', jsonEncode(cfg));
    init();
  }

  void applyDefaultCashbox({int cashboxId = defaultCashboxId}) {
    setInt('cashbox_id', cashboxId);
    Map<String, dynamic> cfg = {};
    final raw = string('config');
    if (raw.isNotEmpty) {
      try {
        final decoded = jsonDecode(raw);
        if (decoded is Map) {
          cfg = Map<String, dynamic>.from(decoded);
        }
      } catch (_) {}
    }
    cfg['cashbox_id'] = cashboxId;
    cfg['f_cashbox_id'] = cashboxId;
    setString('config', jsonEncode(cfg));
  }

  BuildContext? get maybeContext => navigatorKey.currentContext;

  BuildContext context() {
    final ctx = maybeContext;
    assert(ctx != null, 'Navigator context is not ready');
    return ctx!;
  }

  String string(String key) {
    return getString(key) ?? '';
  }

  /// Active API bearer: waiter if logged in, otherwise program session.
  String bearerToken() {
    final waiter = string('waiter_token');
    if (waiter.isNotEmpty) return waiter;
    return string('program_token');
  }

  String df(String v) {
    return v.replaceAll(RegExp('r(?!\d[\.\,][1-9]+)0+\$'), '').replaceAll('[\.\,]\$', '');
  }

  String currentDateText() {
    DateTime dt = DateTime.now();
    return DateFormat('dd/MM/yyyy').format(dt);
  }

  DateTime strDate(String s) {
    return DateFormat('yyyy-MM-dd').tryParse(s) ?? DateTime.now();
  }

  String dateText(DateTime dt) {
    return DateFormat('dd/MM/yyyy').format(dt);
  }

  String dateMySqlText(DateTime dt) {
    return DateFormat('yyyy-MM-dd').format(dt);
  }

  DateTime workingDay() {
    return strDate(string('workingday'));
  }

  String number(num v) {
    var nf = NumberFormat.decimalPatternDigits(locale: 'en_us',
      decimalDigits:0);
    return nf.format(v);
  }

  void init() {
    config.clear();

    var configString = string('config');
    print(configString);
    if (configString.isEmpty) {
      configString = '{}';
    }
    try {
      final decoded = jsonDecode(configString);
      config = decoded is Map ? Map<String, dynamic>.from(decoded) : <String, dynamic>{};
    } catch (e) {
      setString('config', '{}');
      config = {'dashboard': ''};
    }
    if (!config.containsKey('cashbox_id') && getInt('cashbox_id') == null) {
      applyDefaultCashbox();
    } else if (getInt('cashbox_id') != null) {
      config['cashbox_id'] = cashboxId();
      config['f_cashbox_id'] = cashboxId();
    }
  }
}

late final SharedPreferences prefs;

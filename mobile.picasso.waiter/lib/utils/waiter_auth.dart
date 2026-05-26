import 'dart:convert';

import 'package:picassowaiter/utils/http_query.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/waiter_cashbox.dart';
import 'package:picassowaiter/utils/waiter_workstation.dart';

/// Two-stage auth like desktop Waiter:
/// 1. Program account ([DlgSplashScreen]) — client settings, workstation, cashbox.
/// 2. Waiter PIN ([DlgScreen]) — user session for orders.
class WaiterAuth {
  static const int permissionEditOrder = 501;
  static const int permissionRemovePrintedGoods = 506;

  static const routePinLogin = 'engine/v2/worker/user-login/pin-login';
  static const routeProgramLogin = 'engine/v2/worker/user-login/login';
  static const routeLegacyLogin = 'engine/login.php';

  static bool get isProgramReady => prefs.getBool('program_ready') ?? false;

  static bool get isWaiterLoggedIn => prefs.string('waiter_token').isNotEmpty;

  /// Server + service login/password from Configuration (stage 1).
  static bool isConnectionConfigured() {
    return prefs.string('serveraddress').trim().isNotEmpty &&
        prefs.string('server_user').trim().isNotEmpty &&
        prefs.string('server_password').isNotEmpty;
  }

  /// Stage 1 at app start: program login + workstation (after client-config).
  static Future<String?> startupProgramSession() async {
    clearWaiterSession(keepProgram: false);

    final username = prefs.string('server_user');
    final password = prefs.string('server_password');
    if (username.isEmpty || password.isEmpty) {
      prefs.setBool('program_ready', false);
      return 'Set program user and password in Configuration';
    }

    final r = await HttpQuery(routeProgramLogin).request({
      'username': username,
      'password': password,
      'nootp': true,
    });
    if (r['status'] == 0) {
      prefs.setBool('program_ready', false);
      final d = r['data'];
      return d is String && d.isNotEmpty ? d : 'Program login failed';
    }

    final token = '${r['token'] ?? ''}';
    if (token.isEmpty) {
      prefs.setBool('program_ready', false);
      return 'Program login: empty token';
    }

    prefs.setString('program_token', token);
    useProgramToken();
    _saveProgramSettings(r['settings']);

    final wsCashbox = await WaiterWorkstation.loadCashboxFromWorkstation();
    if (wsCashbox != null && wsCashbox > 0) {
      prefs.applyCashboxId(wsCashbox);
    }

    prefs.setBool('program_ready', true);
    return null;
  }

  /// Stage 2: waiter PIN — replaces active bearer, does not touch workstation [config].
  static Future<String?> pinLogin(String pin) async {
    if (!isProgramReady) {
      return 'Program session is not ready';
    }
    if (pin.isEmpty) {
      return 'PIN is empty';
    }

    useProgramToken();

    final r = await HttpQuery(routePinLogin).request({
      'pin': pin,
      'nootp': true,
    });
    if (r['status'] == 0) {
      final d = r['data'];
      if (d is String && d.isNotEmpty) return d;
      return 'Access denied';
    }

    final err = applyWaiterLoginPayload(r);
    if (err != null) return err;

    if (!hasEditOrderPermission()) {
      clearWaiterSession();
      return 'You have not permission';
    }

    return null;
  }

  /// Restore waiter after app restart (stage 1 must already be done).
  static Future<String?> restoreWaiterSession() async {
    if (!isProgramReady) {
      return await startupProgramSession();
    }

    final waiterToken = prefs.string('waiter_token');
    if (waiterToken.isEmpty) {
      return 'Not logged in';
    }

    useWaiterToken();

    final r = await HttpQuery(routeLegacyLogin).request({'method': 3});
    if (r['status'] == 0) {
      clearWaiterSession();
      return 'Session expired';
    }

    final data = r['data'];
    if (data is Map) {
      final legacy = Map<String, dynamic>.from(data);
      if (legacy['sessionkey'] != null) {
        prefs.setString('waiter_token', '${legacy['sessionkey']}');
        useWaiterToken();
      }
      final err = applyWaiterLoginPayload({
        'userdata': legacy['user'],
        'permissions': legacy['permissions'],
      });
      if (err != null) return err;
    }

    if (!hasEditOrderPermission()) {
      clearWaiterSession();
      return 'You have not permission';
    }
    return null;
  }

  static Future<String?> finishWaiterLogin() async {
    useWaiterToken();
    return WaiterCashbox.ensureSession();
  }

  static void useProgramToken() {
    final t = prefs.string('program_token');
    if (t.isNotEmpty) {
      prefs.setString('token', t);
    }
  }

  static void useWaiterToken() {
    final t = prefs.string('waiter_token');
    if (t.isNotEmpty) {
      prefs.setString('token', t);
    }
  }

  /// Log out waiter only; program session stays for next PIN.
  static void clearWaiterSession({bool keepProgram = true}) {
    prefs.remove('waiter_token');
    prefs.setInt('userid', 0);
    prefs.remove('permissions');
    prefs.remove('staff_name');
    prefs.remove('user_group');
    prefs.setBool('stayloggedin', false);
    if (keepProgram && isProgramReady) {
      useProgramToken();
    } else {
      prefs.setString('token', '');
    }
  }

  static String? applyWaiterLoginPayload(Map<String, dynamic> d) {
    try {
      final token = '${d['token'] ?? ''}';
      if (token.isNotEmpty) {
        prefs.setString('waiter_token', token);
        useWaiterToken();
      }

      final userdata = d['user'] ?? d['userdata'];
      if (userdata is Map) {
        final uid = userdata['f_id'] ?? userdata['f_user'];
        if (uid != null) {
          prefs.setInt('userid', int.tryParse('$uid') ?? 0);
        }
        final last = '${userdata['f_last'] ?? ''}'.trim();
        final first = '${userdata['f_first'] ?? ''}'.trim();
        if (last.isNotEmpty) {
          final initial = first.isNotEmpty ? '${first[0]}.' : '';
          prefs.setString('staff_name', '$last $initial'.trim());
        }
        final grp = userdata['f_group'];
        if (grp != null) {
          prefs.setInt('user_group', int.tryParse('$grp') ?? 0);
        }
      }

      _savePermissions(d['permissions']);
    } catch (e) {
      return e.toString();
    }
    return null;
  }

  static void _saveProgramSettings(dynamic settings) {
    if (settings is List) {
      prefs.setString('program_settings', jsonEncode(settings));
    }
  }

  static void _savePermissions(dynamic permissions) {
    if (permissions is! List) {
      prefs.remove('permissions');
      return;
    }
    final ids = <int>[];
    for (final p in permissions) {
      final n = int.tryParse('$p');
      if (n != null) ids.add(n);
    }
    prefs.setString('permissions', jsonEncode(ids));
  }

  static bool hasPermission(int permissionId) {
    final group = prefs.getInt('user_group');
    if (group == 1) return true;

    final raw = prefs.string('permissions');
    if (raw.isEmpty) return false;
    try {
      final list = jsonDecode(raw);
      if (list is! List) return false;
      for (final p in list) {
        if (int.tryParse('$p') == permissionId) return true;
      }
    } catch (_) {}
    return false;
  }

  static bool hasEditOrderPermission() => hasPermission(permissionEditOrder);

  static bool hasRemovePrintedGoodsPermission() =>
      hasPermission(permissionRemovePrintedGoods);

  static Map<String, dynamic> _waiterSessionSnapshot() => {
        'waiter_token': prefs.string('waiter_token'),
        'token': prefs.string('token'),
        'userid': prefs.getInt('userid'),
        'permissions': prefs.string('permissions'),
        'staff_name': prefs.string('staff_name'),
        'user_group': prefs.getInt('user_group'),
      };

  static void _restoreWaiterSession(Map<String, dynamic> snap) {
    prefs.setString('waiter_token', '${snap['waiter_token'] ?? ''}');
    prefs.setInt('userid', snap['userid'] as int? ?? 0);
    prefs.setString('permissions', '${snap['permissions'] ?? ''}');
    prefs.setString('staff_name', '${snap['staff_name'] ?? ''}');
    final grp = snap['user_group'];
    if (grp is int) {
      prefs.setInt('user_group', grp);
    }
    final t = prefs.string('waiter_token');
    if (t.isNotEmpty) {
      prefs.setString('token', t);
    } else if (isProgramReady) {
      useProgramToken();
    }
  }

  /// PIN of a user with [permissionRemovePrintedGoods], then run [action] as that user.
  static Future<String?> withRemovePrintedElevation(
    String pin,
    Future<String?> Function() action,
  ) async {
    final snap = _waiterSessionSnapshot();
    try {
      final err = await pinLogin(pin);
      if (err != null && err.isNotEmpty) return err;
      if (!hasRemovePrintedGoodsPermission()) {
        return 'Permission denied';
      }
      return await action();
    } finally {
      _restoreWaiterSession(snap);
    }
  }

  static void saveProgramCredentials(String user, String password) {
    prefs.setString('server_user', user);
    prefs.setString('server_password', password);
    prefs.setBool('program_ready', false);
  }
}

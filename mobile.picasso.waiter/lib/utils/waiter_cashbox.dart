import 'package:picassowaiter/utils/http_query.dart';
import 'package:picassowaiter/utils/prefs.dart';

/// Cash session must be open before creating orders (o_daily_counter).
class WaiterCashbox {
  static const routeCheckStatus = 'engine/v2/waiter/cashbox/check-status';
  static const routeOpen = 'engine/v2/waiter/cashbox/open';

  /// Opens shift on server if missing. Returns null on success, error text on failure.
  static Future<String?> ensureSession() async {
    var cashboxId = prefs.cashboxId();

    final check = await HttpQuery(routeCheckStatus).request({
      if (cashboxId > 0) 'cashbox_id': cashboxId,
    });
    if (check['status'] == 0) {
      return _errorText(check);
    }

    final resolved = int.tryParse('${check['cashbox_id'] ?? ''}') ?? 0;
    if (resolved > 0 && resolved != cashboxId) {
      prefs.applyCashboxId(resolved);
      cashboxId = resolved;
    }
    if (cashboxId <= 0) {
      return 'cashbox_id not set';
    }

    if (_sessionIdFromCheck(check) > 0) {
      return null;
    }

    final open = await HttpQuery(routeOpen).request({
      'cashbox_id': cashboxId,
      'amount_open': 0,
    });
    if (open['status'] == 0) {
      return _errorText(open);
    }
    if (_sessionIdFromOpen(open) > 0) {
      return null;
    }

    return 'Cashbox session is empty';
  }

  static int _sessionIdFromCheck(Map<String, dynamic> r) {
    final id = r['cashbox_session_id'];
    if (id != null) {
      final n = int.tryParse('$id');
      if (n != null && n > 0) {
        return n;
      }
    }
    final sess = r['cashbox_session'];
    if (sess is Map) {
      final n = int.tryParse('${sess['f_id']}');
      if (n != null && n > 0) {
        return n;
      }
    }
    return 0;
  }

  static int _sessionIdFromOpen(Map<String, dynamic> r) {
    final sess = r['cashbox_session'];
    if (sess is Map) {
      final n = int.tryParse('${sess['f_id']}');
      if (n != null && n > 0) {
        return n;
      }
    }
    return _sessionIdFromCheck(r);
  }

  static String _errorText(Map<String, dynamic> r) {
    final d = r['data'];
    if (d == null) {
      return 'Request failed';
    }
    return d is String ? d : d.toString();
  }
}

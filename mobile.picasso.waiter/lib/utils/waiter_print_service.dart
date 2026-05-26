import 'package:intl/intl.dart';
import 'package:picassowaiter/utils/pos_print.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/waiter_print_client.dart';

/// Service check printing — mirrors [DlgOrder::printService].
class WaiterPrintService {
  static const orderStatePreorder = 5;

  static String printServerUrl() {
    final v = Prefs.config['print_server'];
    if (v == null) return '';
    return v.toString().trim();
  }

  static String staffName() {
    final s = prefs.string('staff_name');
    if (s.isNotEmpty) return s;
    return prefs.string('username');
  }

  /// After API [print-service-check]: send tickets to print server if configured.
  /// Returns error text or null on success / skip (no print server).
  static Future<String?> dispatchServiceCheck(Map<String, dynamic> jdoc) async {
    final printServer = printServerUrl();
    if (printServer.isEmpty) {
      return null;
    }

    final printData = _printDataMap(jdoc['print_data']);
    if (printData.isEmpty) {
      return null;
    }

    final header = _map(jdoc['header']);
    final reprint = jdoc['reprint'] == true || jdoc['reprint'] == 1;
    final orderState = int.tryParse('${jdoc['order']?['f_state'] ?? header['f_state'] ?? ''}') ?? 0;
    final isPreorder = orderState == orderStatePreorder;

    for (final entry in printData.entries) {
      final printerName = entry.key;
      final block = entry.value;

      final cmds = _buildServiceTicket(
        printerName: printerName,
        block: block,
        header: header,
        reprint: reprint,
        isPreorder: isPreorder,
      );
      if (cmds.isEmpty) continue;

      final err = await WaiterPrintClient.post(
        printServer: printServer,
        printerName: printerName,
        printData: cmds,
      );
      if (err != null && err.isNotEmpty) {
        return err;
      }
    }
    return null;
  }

  static Map<String, Map<String, dynamic>> _printDataMap(dynamic raw) {
    if (raw is! Map) return {};
    final out = <String, Map<String, dynamic>>{};
    raw.forEach((key, value) {
      if (value is Map) {
        out['$key'] = Map<String, dynamic>.from(value);
      }
    });
    return out;
  }

  static Map<String, dynamic> _map(dynamic v) {
    if (v is Map) return Map<String, dynamic>.from(v);
    return {};
  }

  static List<Map<String, dynamic>> _buildServiceTicket({
    required String printerName,
    required Map<String, dynamic> block,
    required Map<String, dynamic> header,
    required bool reprint,
    required bool isPreorder,
  }) {
    final dishes = block['dishes'];
    if (dishes is! List || dishes.isEmpty) {
      return [];
    }

    const bs = 22;
    final now = DateTime.now();
    final dateStr = DateFormat('dd/MM/yyyy').format(now);
    final timeStr = DateFormat('HH:mm:ss').format(now);

    final p = PosPrint();
    p.setFontSize(bs);
    p.setFontBold(true);

    if (reprint) {
      p.ctext('REPRINT');
      p.br();
      p.br();
    }
    if (isPreorder) {
      p.ctext('PREORDER');
      p.br();
      p.br();
    }

    p.ctext('NEW ORDER');
    p.br();
    p.br();
    p.setFontBold(false);
    p.ltext('Table', x: 0);
    p.rtext('${header['f_table_name'] ?? ''}');
    p.br();
    p.ltext('Order no', x: 0);
    p.rtext('${header['f_prefix'] ?? ''}');
    p.br();
    p.ltext('Date', x: 0);
    p.rtext(dateStr);
    p.br();
    p.ltext('Time', x: 0);
    p.rtext(timeStr);
    p.br();
    p.ltext('Staff', x: 0);
    p.rtext(staffName());
    p.br();
    p.line();
    p.br(2);

    for (final item in dishes) {
      if (item is! Map) continue;
      final jd = Map<String, dynamic>.from(item);
      final fromPackage = jd['f_is_package_component'] == true ||
          jd['f_is_package_component'] == 1;
      var dishName = '${jd['f_dish_name'] ?? ''}';
      if (fromPackage) {
        dishName = '>>> $dishName';
      }

      p.setFontSize(bs + 2);
      p.setFontBold(false);
      p.ltext(dishName, x: 0, textWidth: 65);
      p.setFontBold(true);

      final qtyLine = '${jd['f_qty_line'] ?? ''}'.trim();
      final qtyOut = qtyLine.isNotEmpty
          ? qtyLine
          : _formatQty(jd['f_qty']);
      p.rtext(qtyOut);

      final comment = '${jd['f_comment'] ?? ''}'.trim();
      if (comment.isNotEmpty) {
        p.br();
        p.setFontSize(bs - 4);
        p.setFontBold(true);
        p.ltext(comment, x: 0, textWidth: 650);
        p.br();
        p.setFontSize(bs + 2);
      }

      p.br();
      p.line();
      p.br(1);
    }

    p.line();
    p.br(1);
    p.setFontSize(bs - 6);
    p.ltext('Printer: $printerName');
    p.setFontBold(true);
    p.rtext('${block['side'] ?? ''}');
    p.br();

    return p.jsonData();
  }

  /// Kitchen ticket for removed dish — mirrors [DlgOrder::printRemovedDish].
  static Future<String?> dispatchRemovedDish(
    Map<String, dynamic> jdoc, {
    required Map<String, dynamic> header,
  }) async {
    final printServer = printServerUrl();
    if (printServer.isEmpty) return null;

    for (final printerName in [
      '${jdoc['print1'] ?? ''}',
      '${jdoc['print2'] ?? ''}',
    ]) {
      if (printerName.isEmpty) continue;
      final cmds = _buildRemovedTicket(
        printerName: printerName,
        jdoc: jdoc,
        header: header,
      );
      if (cmds.isEmpty) continue;
      final err = await WaiterPrintClient.post(
        printServer: printServer,
        printerName: printerName,
        printData: cmds,
      );
      if (err != null && err.isNotEmpty) return err;
    }
    return null;
  }

  static List<Map<String, dynamic>> _buildRemovedTicket({
    required String printerName,
    required Map<String, dynamic> jdoc,
    required Map<String, dynamic> header,
  }) {
    const bs = 22;
    final now = DateTime.now();
    final dateStr = DateFormat('dd/MM/yyyy').format(now);
    final timeStr = DateFormat('HH:mm:ss').format(now);
    final dishName = '${jdoc['f_removed_dish_name'] ?? ''}';
    if (dishName.isEmpty) return [];

    final p = PosPrint();
    p.setFontSize(bs);
    p.setFontBold(true);
    p.ctext('REMOVED');
    p.br();
    p.br();
    p.setFontBold(false);
    p.ltext('Table', x: 0);
    p.rtext('${header['f_table_name'] ?? ''}');
    p.br();
    p.ltext('Order no', x: 0);
    p.rtext('${header['f_prefix'] ?? ''}');
    p.br();
    p.ltext('Date', x: 0);
    p.rtext(dateStr);
    p.br();
    p.ltext('Time', x: 0);
    p.rtext(timeStr);
    p.br();
    p.ltext('Staff', x: 0);
    p.rtext(staffName());
    p.br();
    p.line();
    p.setFontSize(bs + 2);
    p.ltext(dishName, x: 0);
    p.setFontBold(true);
    p.rtext(_formatQty(jdoc['f_removed_qty']));
    p.setFontBold(false);
    final comment = '${jdoc['f_removed_comment'] ?? ''}'.trim();
    if (comment.isNotEmpty) {
      p.br();
      p.setFontSize(bs - 2);
      p.setFontBold(true);
      p.ltext(comment, x: 0);
      p.br();
      p.setFontSize(bs);
      p.setFontBold(false);
    }
    p.br();
    p.line();
    p.setFontSize(bs - 6);
    p.ltext('Printer: $printerName');
    p.br();
    return p.jsonData();
  }

  static String _formatQty(dynamic v) {
    final n = (v is num) ? v.toDouble() : double.tryParse('$v') ?? 0;
    if ((n - n.roundToDouble()).abs() < 0.00001) {
      return '${n.round()}';
    }
    var s = n.toStringAsFixed(2);
    if (s.contains('.')) {
      s = s.replaceAll(RegExp(r'0+$'), '').replaceAll(RegExp(r'\.$'), '');
    }
    return s;
  }
}

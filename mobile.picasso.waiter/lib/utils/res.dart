import 'dart:convert';
import 'dart:typed_data';

class Res {
  static Map<String, Uint8List> images = {};
  static Map<String, String> tr = {};

  static void initFrom(String s) {
    List<dynamic> sd = jsonDecode(s);
    for (final e in sd) {
      for (final k in e.keys) {
        images[k] = base64Decode(e[k]);
      }
    }
  }

  static void initTr(String s) {
    tr.clear();
    List<String> trlist = s.split('\r\n');
    for (final s in trlist) {
      List<String> k = s.split('=');
      if (k.length > 1) {
        tr[k[0]] = k[1];
      }
    }
    print(tr);
  }
}
import 'package:flutter/foundation.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'api.dart';

/// Demo-development server.
const demoServer = 'https://development.picassocloud.com';

final appModel = AppModel();

class AppModel extends ChangeNotifier {
  String server = demoServer;
  int artNumber = 0;
  int storeOut = 0;
  String storeOutName = '';
  int storeIn = 0;
  String storeInName = '';
  String lang = 'ru';
  String token = '';
  String userName = '';

  bool get configured => server.isNotEmpty && artNumber > 0;
  bool get storesReady => storeOut > 0 && storeIn > 0;
  bool get signedIn => token.isNotEmpty;

  Api get api => Api(base: server, token: token, locale: lang);

  Future<void> load() async {
    final prefs = await SharedPreferences.getInstance();
    final savedServer = prefs.getString('server') ?? '';
    server = savedServer.isEmpty ? demoServer : savedServer;
    artNumber = prefs.getInt('artnumber') ?? 0;
    storeOut = prefs.getInt('store_out') ?? 0;
    storeOutName = prefs.getString('store_out_name') ?? '';
    storeIn = prefs.getInt('store_in') ?? 0;
    storeInName = prefs.getString('store_in_name') ?? '';
    lang = prefs.getString('lang') ?? 'ru';
    if (lang != 'hy' && lang != 'ru') {
      lang = 'ru';
    }
    notifyListeners();
  }

  Future<void> saveSettings({
    required String server,
    required int artNumber,
    required int storeOut,
    required String storeOutName,
    required int storeIn,
    required String storeInName,
    required String lang,
  }) async {
    this.server = server;
    this.artNumber = artNumber;
    this.storeOut = storeOut;
    this.storeOutName = storeOutName;
    this.storeIn = storeIn;
    this.storeInName = storeInName;
    this.lang = lang;
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('server', server);
    await prefs.setInt('artnumber', artNumber);
    await prefs.setInt('store_out', storeOut);
    await prefs.setString('store_out_name', storeOutName);
    await prefs.setInt('store_in', storeIn);
    await prefs.setString('store_in_name', storeInName);
    await prefs.setString('lang', lang);
    notifyListeners();
  }

  void signIn(String token, String name) {
    this.token = token;
    userName = name;
    notifyListeners();
  }

  void signOut() {
    token = '';
    userName = '';
    notifyListeners();
  }
}

String normalizeServer(String raw) {
  var value = raw.trim();
  if (value.isEmpty) {
    return '';
  }
  if (!value.contains('://')) {
    value = 'https://$value';
  }
  return value.replaceAll(RegExp(r'/+$'), '');
}

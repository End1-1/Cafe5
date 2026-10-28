import 'package:flutter/material.dart';

import 'api.dart';
import 'app_model.dart';
import 'drafts_page.dart';
import 'i18n.dart';
import 'qty_dialog.dart';
import 'settings_page.dart';

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  List<Map<String, dynamic>> _goods = [];
  bool _loading = true;
  String? _error;

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    try {
      final goods = await appModel.api.post('goods', {'artnumber': appModel.artNumber});
      if (!mounted) {
        return;
      }
      setState(() {
        _goods = (goods['goods'] as List? ?? []).whereType<Map>().map((row) => Map<String, dynamic>.from(row)).toList();
      });
    } on ApiException catch (e) {
      if (mounted) {
        setState(() => _error = e.message);
      }
    } catch (e) {
      debugPrint('$e');
      if (mounted) {
        setState(() => _error = S.t(appModel.lang, 'loadFail'));
      }
    } finally {
      if (mounted) {
        setState(() => _loading = false);
      }
    }
  }

  Future<void> _create(Map<String, dynamic> good) async {
    if (!appModel.storesReady) {
      _showError(S.t(appModel.lang, 'needStores'));
      return;
    }
    final qty = await showQtyDialog(
      context,
      title: good['f_name']?.toString() ?? '',
      unit: good['f_unit_name']?.toString() ?? '',
    );
    if (qty == null) {
      return;
    }
    try {
      await appModel.api.post('create', {
        'goods_id': good['f_id'],
        'qty': qty,
        'store_out': appModel.storeOut,
        'store_in': appModel.storeIn,
        'artnumber': appModel.artNumber,
      });
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(S.t(appModel.lang, 'saved'))));
      }
      await _load();
    } on ApiException catch (e) {
      _showError(e.message);
    } catch (e) {
      debugPrint('$e');
      _showError(S.t(appModel.lang, 'loadFail'));
    }
  }

  void _showError(String message) {
    if (!mounted) {
      return;
    }
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(message)));
  }

  @override
  Widget build(BuildContext context) {
    final lang = appModel.lang;
    return Scaffold(
      appBar: AppBar(
        title: Text(appModel.userName.isEmpty ? S.t(lang, 'title') : appModel.userName),
        actions: [
          IconButton(
            tooltip: S.t(lang, 'refresh'),
            onPressed: _loading ? null : _load,
            icon: const Icon(Icons.refresh),
          ),
          IconButton(
            tooltip: S.t(lang, 'drafts'),
            onPressed: () {
              Navigator.of(context).push(
                MaterialPageRoute(builder: (_) => const DraftsPage()),
              );
            },
            icon: const Icon(Icons.drafts_outlined),
          ),
          IconButton(
            tooltip: S.t(lang, 'settings'),
            onPressed: () async {
              await Navigator.of(context).push(
                MaterialPageRoute(builder: (_) => const SettingsPage()),
              );
              _load();
            },
            icon: const Icon(Icons.settings),
          ),
          IconButton(
            tooltip: S.t(lang, 'logout'),
            onPressed: appModel.signOut,
            icon: const Icon(Icons.logout),
          ),
        ],
      ),
      body: _loading
          ? const Center(child: CircularProgressIndicator())
          : RefreshIndicator(
              onRefresh: _load,
              child: ListView(
                padding: const EdgeInsets.fromLTRB(12, 8, 12, 24),
                children: [
                  if (_error != null)
                    Padding(
                      padding: const EdgeInsets.only(bottom: 8),
                      child: Text(_error!, style: TextStyle(color: Theme.of(context).colorScheme.error)),
                    ),
                  Text(S.t(lang, 'goods'), style: const TextStyle(fontSize: 18, fontWeight: FontWeight.w600)),
                  const SizedBox(height: 8),
                  if (_goods.isEmpty)
                    Text(S.t(lang, 'emptyGoods'))
                  else
                    ..._goods.map((good) {
                      final unit = good['f_unit_name']?.toString() ?? '';
                      return Padding(
                        padding: const EdgeInsets.only(bottom: 8),
                        child: SizedBox(
                          height: 64,
                          child: FilledButton.tonal(
                            onPressed: () => _create(good),
                            child: Align(
                              alignment: Alignment.centerLeft,
                              child: Text(
                                unit.isEmpty ? '${good['f_name']}' : '${good['f_name']} · $unit',
                                style: const TextStyle(fontSize: 20),
                              ),
                            ),
                          ),
                        ),
                      );
                    }),
                ],
              ),
            ),
    );
  }
}

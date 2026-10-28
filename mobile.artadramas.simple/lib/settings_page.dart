import 'package:flutter/material.dart';

import 'api.dart';
import 'app_model.dart';
import 'i18n.dart';

class SettingsPage extends StatefulWidget {
  const SettingsPage({super.key, this.requiredSetup = false});

  final bool requiredSetup;

  @override
  State<SettingsPage> createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  late final TextEditingController _server;
  late final TextEditingController _art;
  late String _lang;
  int _storeOut = 0;
  int _storeIn = 0;
  List<Map<String, dynamic>> _storages = [];
  String? _error;
  bool _loadingStores = false;

  @override
  void initState() {
    super.initState();
    _server = TextEditingController(text: appModel.server);
    _art = TextEditingController(text: appModel.artNumber > 0 ? '${appModel.artNumber}' : '');
    _lang = appModel.lang;
    _storeOut = appModel.storeOut;
    _storeIn = appModel.storeIn;
    _loadStores();
  }

  @override
  void dispose() {
    _server.dispose();
    _art.dispose();
    super.dispose();
  }

  Future<void> _loadStores() async {
    if (!appModel.signedIn) {
      return;
    }
    setState(() {
      _loadingStores = true;
      _error = null;
    });
    try {
      final data = await appModel.api.post('storages', {});
      final rows = (data['storages'] as List? ?? [])
          .whereType<Map>()
          .map((row) {
            final item = Map<String, dynamic>.from(row);
            item['f_id'] = _asInt(item['f_id']);
            return item;
          })
          .where((row) => row['f_id'] != 0)
          .toList();
      if (!mounted) {
        return;
      }
      setState(() {
        _storages = rows;
        if (rows.isEmpty) {
          _error = S.t(_lang, 'emptyStores');
        }
      });
    } on ApiException catch (e) {
      if (mounted) {
        setState(() => _error = e.message);
      }
    } catch (e) {
      debugPrint('$e');
      if (mounted) {
        setState(() => _error = S.t(_lang, 'loadFail'));
      }
    } finally {
      if (mounted) {
        setState(() => _loadingStores = false);
      }
    }
  }

  String _storeName(int id) {
    for (final row in _storages) {
      if (row['f_id'] == id) {
        return row['f_name']?.toString() ?? '';
      }
    }
    if (id == appModel.storeOut) {
      return appModel.storeOutName;
    }
    if (id == appModel.storeIn) {
      return appModel.storeInName;
    }
    return '';
  }

  int _asInt(dynamic value) {
    if (value is int) {
      return value;
    }
    if (value is num) {
      return value.toInt();
    }
    return int.tryParse('$value') ?? 0;
  }

  Future<void> _save() async {
    final server = normalizeServer(_server.text);
    final art = int.tryParse(_art.text.trim()) ?? 0;
    if (server.isEmpty || art <= 0) {
      _fail(S.t(_lang, 'fillSettings'));
      return;
    }
    if (appModel.signedIn && widget.requiredSetup && (_storeOut <= 0 || _storeIn <= 0)) {
      _fail(S.t(_lang, 'needStores'));
      return;
    }
    await appModel.saveSettings(
      server: server,
      artNumber: art,
      storeOut: _storeOut,
      storeOutName: _storeName(_storeOut),
      storeIn: _storeIn,
      storeInName: _storeName(_storeIn),
      lang: _lang,
    );
    if (!mounted) {
      return;
    }
    if (!widget.requiredSetup && Navigator.of(context).canPop()) {
      Navigator.of(context).pop();
    }
  }

  void _fail(String message) {
    setState(() => _error = message);
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(message)));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(S.t(_lang, 'settings')),
        automaticallyImplyLeading: !widget.requiredSetup,
        actions: [
          if (widget.requiredSetup && appModel.signedIn)
            IconButton(
              tooltip: S.t(_lang, 'logout'),
              onPressed: appModel.signOut,
              icon: const Icon(Icons.logout),
            ),
        ],
      ),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: [
          TextField(
            controller: _server,
            decoration: InputDecoration(
              labelText: S.t(_lang, 'server'),
              hintText: 'https://development.picassocloud.com',
              border: const OutlineInputBorder(),
            ),
          ),
          const SizedBox(height: 12),
          TextField(
            controller: _art,
            keyboardType: TextInputType.number,
            decoration: InputDecoration(
              labelText: S.t(_lang, 'art'),
              border: const OutlineInputBorder(),
            ),
          ),
          const SizedBox(height: 12),
          DropdownButtonFormField<String>(
            initialValue: _lang,
            decoration: InputDecoration(
              labelText: S.t(_lang, 'language'),
              border: const OutlineInputBorder(),
            ),
            items: const [
              DropdownMenuItem(value: 'ru', child: Text('Русский')),
              DropdownMenuItem(value: 'hy', child: Text('Հայերեն')),
            ],
            onChanged: (value) {
              if (value == null) {
                return;
              }
              setState(() => _lang = value);
            },
          ),
          const SizedBox(height: 12),
          if (!appModel.signedIn)
            Text(S.t(_lang, 'signInForStores'))
          else ...[
            _storeField(S.t(_lang, 'storeOut'), _storeOut, (id) => setState(() => _storeOut = id)),
            const SizedBox(height: 12),
            _storeField(S.t(_lang, 'storeIn'), _storeIn, (id) => setState(() => _storeIn = id)),
            if (_loadingStores)
              const Padding(padding: EdgeInsets.only(top: 12), child: Center(child: CircularProgressIndicator())),
          ],
          if (_error != null)
            Padding(
              padding: const EdgeInsets.only(top: 12),
              child: Text(_error!, style: TextStyle(color: Theme.of(context).colorScheme.error)),
            ),
          const SizedBox(height: 20),
          SizedBox(
            height: 52,
            child: FilledButton(
              onPressed: _loadingStores ? null : _save,
              child: Text(
                S.t(_lang, appModel.signedIn ? 'save' : 'next'),
                style: const TextStyle(fontSize: 18),
              ),
            ),
          ),
        ],
      ),
    );
  }

  Widget _storeField(String label, int value, ValueChanged<int> onChanged) {
    final ids = _storages.map((row) => _asInt(row['f_id'])).toSet();
    final current = ids.contains(value) ? value : 0;
    return DropdownButtonFormField<int>(
      key: ValueKey('$label-${_storages.length}-$current'),
      initialValue: current,
      decoration: InputDecoration(labelText: label, border: const OutlineInputBorder()),
      items: [
        const DropdownMenuItem(value: 0, child: Text('—')),
        ..._storages.map(
          (row) => DropdownMenuItem(
            value: _asInt(row['f_id']),
            child: Text(row['f_name']?.toString() ?? ''),
          ),
        ),
      ],
      onChanged: (id) => onChanged(id ?? 0),
    );
  }
}

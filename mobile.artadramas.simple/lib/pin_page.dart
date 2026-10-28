import 'package:flutter/material.dart';

import 'api.dart';
import 'app_model.dart';
import 'i18n.dart';
import 'settings_page.dart';

class PinPage extends StatefulWidget {
  const PinPage({super.key});

  @override
  State<PinPage> createState() => _PinPageState();
}

class _PinPageState extends State<PinPage> {
  String _pin = '';
  bool _busy = false;
  String? _error;

  Future<void> _submit() async {
    if (_pin.isEmpty || _busy) {
      return;
    }
    setState(() {
      _busy = true;
      _error = null;
    });
    try {
      final data = await appModel.api.post('pin-login', {'pin': _pin});
      final user = data['user'];
      final name = user is Map ? (user['f_name']?.toString() ?? '') : '';
      appModel.signIn(data['token']?.toString() ?? '', name);
    } on ApiException catch (e) {
      setState(() => _error = e.message);
    } catch (e) {
      debugPrint('$e');
      setState(() => _error = S.t(appModel.lang, 'networkFail'));
    } finally {
      if (mounted) {
        setState(() => _busy = false);
      }
    }
  }

  void _key(String key) {
    if (_busy) {
      return;
    }
    setState(() {
      _error = null;
      if (key == 'back') {
        if (_pin.isNotEmpty) {
          _pin = _pin.substring(0, _pin.length - 1);
        }
      } else if (_pin.length < 12) {
        _pin += key;
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    final lang = appModel.lang;
    return Scaffold(
      appBar: AppBar(
        title: Text(S.t(lang, 'title')),
        actions: [
          IconButton(
            tooltip: S.t(lang, 'settings'),
            onPressed: () {
              Navigator.of(context).push(
                MaterialPageRoute(builder: (_) => const SettingsPage()),
              );
            },
            icon: const Icon(Icons.settings),
          ),
        ],
      ),
      body: SafeArea(
        child: Column(
          children: [
            const SizedBox(height: 16),
            ClipRRect(
              borderRadius: BorderRadius.circular(20),
              child: Image.asset('assets/logo.png', width: 96, height: 96),
            ),
            const SizedBox(height: 16),
            Text(S.t(lang, 'pin'), style: const TextStyle(fontSize: 22)),
            const SizedBox(height: 12),
            Text(
              _pin.isEmpty ? '—' : List.filled(_pin.length, '●').join(' '),
              style: const TextStyle(fontSize: 32, letterSpacing: 4),
            ),
            if (_error != null)
              Padding(
                padding: const EdgeInsets.all(16),
                child: Text(_error!, style: TextStyle(color: Theme.of(context).colorScheme.error, fontSize: 16)),
              ),
            const Spacer(),
            if (_busy) const Padding(padding: EdgeInsets.all(16), child: CircularProgressIndicator()),
            _pad(),
            Padding(
              padding: const EdgeInsets.fromLTRB(16, 8, 16, 16),
              child: SizedBox(
                width: double.infinity,
                height: 56,
                child: FilledButton(
                  onPressed: _pin.isEmpty || _busy ? null : _submit,
                  child: Text(S.t(lang, 'done'), style: const TextStyle(fontSize: 20)),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _pad() {
    const keys = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '', '0', 'back'];
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 24),
      child: GridView.count(
        crossAxisCount: 3,
        shrinkWrap: true,
        childAspectRatio: 1.6,
        mainAxisSpacing: 8,
        crossAxisSpacing: 8,
        physics: const NeverScrollableScrollPhysics(),
        children: keys.map((key) {
          if (key.isEmpty) {
            return const SizedBox.shrink();
          }
          return OutlinedButton(
            onPressed: () => _key(key),
            child: key == 'back'
                ? const Icon(Icons.backspace_outlined, size: 28)
                : Text(key, style: const TextStyle(fontSize: 28)),
          );
        }).toList(),
      ),
    );
  }
}

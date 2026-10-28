import 'package:flutter/material.dart';

import 'api.dart';
import 'app_model.dart';
import 'i18n.dart';
import 'qty_dialog.dart';

class DraftsPage extends StatefulWidget {
  const DraftsPage({super.key});

  @override
  State<DraftsPage> createState() => _DraftsPageState();
}

class _DraftsPageState extends State<DraftsPage> {
  List<Map<String, dynamic>> _drafts = [];
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
      final data = await appModel.api.post('drafts', {});
      if (!mounted) {
        return;
      }
      setState(() {
        _drafts = (data['drafts'] as List? ?? []).whereType<Map>().map((row) => Map<String, dynamic>.from(row)).toList();
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

  Future<void> _edit(Map<String, dynamic> draft) async {
    final current = (draft['qty'] as num?)?.toDouble() ?? 0;
    final qty = await showQtyDialog(
      context,
      title: draft['goods_name']?.toString() ?? '',
      unit: '',
      initial: formatQty(current),
    );
    if (qty == null) {
      return;
    }
    try {
      await appModel.api.post('update-qty', {
        'id': draft['id'],
        'qty': qty,
        'version': draft['version'],
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

  Future<bool> _confirmDelete(Map<String, dynamic> draft) async {
    final lang = appModel.lang;
    final name = draft['goods_name']?.toString() ?? '';
    final accepted = await showDialog<bool>(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(S.t(lang, 'deleteDraft')),
        content: Text(name),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context, false),
            child: Text(S.t(lang, 'cancel')),
          ),
          FilledButton(
            onPressed: () => Navigator.pop(context, true),
            child: Text(S.t(lang, 'delete')),
          ),
        ],
      ),
    );
    if (accepted != true) {
      return false;
    }
    try {
      await appModel.api.post('delete', {'id': draft['id']});
      return true;
    } on ApiException catch (e) {
      _showError(e.message);
      return false;
    } catch (e) {
      debugPrint('$e');
      _showError(S.t(lang, 'loadFail'));
      return false;
    }
  }

  void _showError(String message) {
    if (!mounted) {
      return;
    }
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(message)));
  }

  String _when(String raw) {
    final parsed = DateTime.tryParse(raw.replaceFirst(' ', 'T'));
    if (parsed == null) {
      return raw;
    }
    final day = parsed.day.toString().padLeft(2, '0');
    final month = parsed.month.toString().padLeft(2, '0');
    final hour = parsed.hour.toString().padLeft(2, '0');
    final minute = parsed.minute.toString().padLeft(2, '0');
    return '$day.$month $hour:$minute';
  }

  @override
  Widget build(BuildContext context) {
    final lang = appModel.lang;
    return Scaffold(
      appBar: AppBar(
        title: Text(S.t(lang, 'drafts')),
        actions: [
          IconButton(
            tooltip: S.t(lang, 'refresh'),
            onPressed: _loading ? null : _load,
            icon: const Icon(Icons.refresh),
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
                  if (_drafts.isEmpty)
                    Text(S.t(lang, 'emptyDrafts'))
                  else
                    ..._drafts.map((draft) {
                      final qty = (draft['qty'] as num?)?.toDouble() ?? 0;
                      final docNum = draft['doc_num']?.toString() ?? '';
                      final id = draft['id']?.toString() ?? '';
                      return Dismissible(
                        key: ValueKey(id),
                        direction: DismissDirection.startToEnd,
                        confirmDismiss: (_) => _confirmDelete(draft),
                        onDismissed: (_) {
                          setState(() => _drafts.removeWhere((row) => row['id']?.toString() == id));
                        },
                        background: Container(
                          alignment: Alignment.centerLeft,
                          padding: const EdgeInsets.only(left: 20),
                          color: Colors.red,
                          child: const Icon(Icons.delete, color: Colors.white),
                        ),
                        child: Card(
                          child: ListTile(
                            title: Text(draft['goods_name']?.toString() ?? '', style: const TextStyle(fontSize: 18)),
                            subtitle: Text('${_when(draft['doc_date']?.toString() ?? '')}  №$docNum'),
                            trailing: Text(formatQty(qty), style: const TextStyle(fontSize: 22, fontWeight: FontWeight.w600)),
                            onTap: () => _edit(draft),
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

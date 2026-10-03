import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/network/api_client.dart';
import '../../../core/network/api_exception.dart';
import '../../../l10n/app_localizations.dart';

class GuestOrderStatusScreen extends ConsumerStatefulWidget {
  const GuestOrderStatusScreen({super.key, required this.token});

  final String token;

  @override
  ConsumerState<GuestOrderStatusScreen> createState() =>
      _GuestOrderStatusScreenState();
}

class _GuestOrderStatusScreenState
    extends ConsumerState<GuestOrderStatusScreen> {
  bool _loading = true;
  String? _error;
  Map<String, dynamic>? _order;

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
      final data = await ref.read(apiClientProvider).post(
        '/ararix/order-status/by-token',
        {'token': widget.token},
      );
      final order = data['order'];
      if (order is! Map) {
        throw ApiException('Invalid response');
      }
      if (!mounted) return;
      setState(() {
        _order = Map<String, dynamic>.from(order);
        _loading = false;
      });
    } on ApiException catch (e) {
      if (!mounted) return;
      setState(() {
        _error = e.message;
        _loading = false;
      });
    } catch (e) {
      if (!mounted) return;
      setState(() {
        _error = e.toString();
        _loading = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    return Scaffold(
      appBar: AppBar(
        title: Text(l10n.orderStatusTitle),
      ),
      body: _loading
          ? const Center(child: CircularProgressIndicator())
          : _error != null
              ? Center(
                  child: Padding(
                    padding: const EdgeInsets.all(24),
                    child: Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        Text(
                          _error!,
                          textAlign: TextAlign.center,
                        ),
                        const SizedBox(height: 16),
                        FilledButton(
                          onPressed: _load,
                          child: Text(l10n.retry),
                        ),
                      ],
                    ),
                  ),
                )
              : RefreshIndicator(
                  onRefresh: _load,
                  child: ListView(
                    padding: const EdgeInsets.all(20),
                    children: [
                      Text(
                        '${l10n.orderNumber}: ${_order?['number'] ?? ''}',
                        style: Theme.of(context).textTheme.headlineSmall,
                      ),
                      const SizedBox(height: 8),
                      Text(
                        '${l10n.status}: ${_order?['kitchen_status_name'] ?? _order?['state_name'] ?? ''}',
                        style: Theme.of(context).textTheme.titleMedium,
                      ),
                      const SizedBox(height: 4),
                      Text(
                        '${l10n.total}: ${_order?['amount_total'] ?? ''}',
                      ),
                      const SizedBox(height: 20),
                      Text(
                        l10n.items,
                        style: Theme.of(context).textTheme.titleMedium,
                      ),
                      const SizedBox(height: 8),
                      ..._items(),
                    ],
                  ),
                ),
    );
  }

  List<Widget> _items() {
    final raw = _order?['items'];
    if (raw is! List) return const [];
    return raw.whereType<Map>().map((e) {
      final name = (e['name'] ?? '').toString();
      final qty = e['qty'];
      final status = (e['kitchen_status_name'] ?? '').toString();
      return ListTile(
        contentPadding: EdgeInsets.zero,
        title: Text(name),
        subtitle: status.isEmpty ? null : Text(status),
        trailing: Text('× $qty'),
      );
    }).toList();
  }
}

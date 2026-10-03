import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';
import '../../address/application/address_notifier.dart';
import '../../address/data/address_models.dart';
import '../../address/presentation/address_flow.dart';

class MyAddressesScreen extends ConsumerStatefulWidget {
  const MyAddressesScreen({super.key});

  @override
  ConsumerState<MyAddressesScreen> createState() => _MyAddressesScreenState();
}

class _MyAddressesScreenState extends ConsumerState<MyAddressesScreen> {
  @override
  void initState() {
    super.initState();
    Future.microtask(() => ref.read(addressProvider.notifier).load());
  }

  Future<void> _edit(DeliveryAddress? address) async {
    await showAddressEditor(context, ref, existing: address);
  }

  Future<void> _delete(DeliveryAddress address) async {
    final l10n = AppLocalizations.of(context);
    final ok = await showDialog<bool>(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(l10n.deleteAddress),
        content: Text(l10n.deleteAddressQuestion),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context, false),
            child: Text(l10n.back),
          ),
          TextButton(
            onPressed: () => Navigator.pop(context, true),
            child: Text(l10n.deleteAddress),
          ),
        ],
      ),
    );
    if (ok != true || !mounted) return;
    await ref.read(addressProvider.notifier).deleteAddress(address);
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final state = ref.watch(addressProvider);

    return Scaffold(
      backgroundColor: Colors.white,
      appBar: ScreenAppBar(title: l10n.myAddresses),
      body: state.loading && state.addresses.isEmpty
          ? const Center(child: CircularProgressIndicator())
          : state.error != null && state.addresses.isEmpty
              ? Center(
                  child: Padding(
                    padding: const EdgeInsets.all(24),
                    child: Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        Text(state.error!, textAlign: TextAlign.center),
                        const SizedBox(height: 12),
                        FilledButton(
                          onPressed: () =>
                              ref.read(addressProvider.notifier).load(),
                          child: Text(l10n.retry),
                        ),
                      ],
                    ),
                  ),
                )
              : Column(
                  children: [
                    Expanded(
                      child: state.addresses.isEmpty
                          ? Center(
                              child: Text(
                                l10n.noSavedAddresses,
                                style: const TextStyle(color: AppColors.muted),
                              ),
                            )
                          : ListView.separated(
                              itemCount: state.addresses.length,
                              separatorBuilder: (_, _) => const Divider(
                                height: 1,
                                color: AppColors.border,
                              ),
                              itemBuilder: (context, index) {
                                final address = state.addresses[index];
                                final subtitle = address.street.trim();
                                return ListTile(
                                  contentPadding: const EdgeInsets.only(
                                    left: 20,
                                    right: 8,
                                  ),
                                  leading: Icon(
                                    address.isActive
                                        ? Icons.radio_button_checked
                                        : Icons.location_on_outlined,
                                    color: address.isActive
                                        ? const Color(0xFF2EAE57)
                                        : AppColors.text,
                                  ),
                                  title: Text(
                                    address.displayLabel,
                                    maxLines: 1,
                                    overflow: TextOverflow.ellipsis,
                                  ),
                                  subtitle: subtitle.isNotEmpty &&
                                          subtitle != address.displayLabel
                                      ? Text(
                                          subtitle,
                                          maxLines: 2,
                                          overflow: TextOverflow.ellipsis,
                                        )
                                      : null,
                                  onTap: () => _edit(address),
                                  trailing: Row(
                                    mainAxisSize: MainAxisSize.min,
                                    children: [
                                      IconButton(
                                        icon: const Icon(Icons.edit_outlined),
                                        onPressed: () => _edit(address),
                                      ),
                                      IconButton(
                                        icon: const Icon(Icons.delete_outline),
                                        onPressed: state.saving
                                            ? null
                                            : () => _delete(address),
                                      ),
                                    ],
                                  ),
                                );
                              },
                            ),
                    ),
                    SafeArea(
                      child: Padding(
                        padding: const EdgeInsets.fromLTRB(16, 8, 16, 16),
                        child: SizedBox(
                          width: double.infinity,
                          height: 48,
                          child: FilledButton.icon(
                            style: FilledButton.styleFrom(
                              backgroundColor: const Color(0xFF2EAE57),
                            ),
                            onPressed: () => _edit(null),
                            icon: const Icon(Icons.add),
                            label: Text(l10n.addNewAddress),
                          ),
                        ),
                      ),
                    ),
                  ],
                ),
    );
  }
}

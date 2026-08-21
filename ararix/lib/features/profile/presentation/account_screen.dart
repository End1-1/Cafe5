import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../core/network/api_exception.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';
import '../../auth/application/session_notifier.dart';
import '../data/profile_repository.dart';

class AccountScreen extends ConsumerWidget {
  const AccountScreen({super.key});

  Future<void> _editName(BuildContext context, WidgetRef ref) async {
    final l10n = AppLocalizations.of(context);
    final current = ref.read(sessionProvider).client?.name ?? '';
    final controller = TextEditingController(text: current);
    final value = await showDialog<String>(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(l10n.name),
        content: TextField(controller: controller, autofocus: true),
        actions: [
          TextButton(onPressed: () => Navigator.pop(context), child: const Text('Cancel')),
          TextButton(
            onPressed: () => Navigator.pop(context, controller.text.trim()),
            child: Text(l10n.save),
          ),
        ],
      ),
    );
    if (value == null) return;
    try {
      final client = await ref.read(profileRepositoryProvider).update(name: value);
      await ref.read(sessionProvider.notifier).updateClient(client);
    } on ApiException catch (e) {
      if (context.mounted) {
        ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(e.message)));
      }
    }
  }

  Future<void> _editEmail(BuildContext context, WidgetRef ref) async {
    final l10n = AppLocalizations.of(context);
    final current = ref.read(sessionProvider).client?.email ?? '';
    final controller = TextEditingController(text: current);
    final value = await showDialog<String>(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(l10n.email),
        content: TextField(
          controller: controller,
          autofocus: true,
          keyboardType: TextInputType.emailAddress,
        ),
        actions: [
          TextButton(onPressed: () => Navigator.pop(context), child: const Text('Cancel')),
          TextButton(
            onPressed: () => Navigator.pop(context, controller.text.trim()),
            child: Text(l10n.save),
          ),
        ],
      ),
    );
    if (value == null) return;
    try {
      final client = await ref.read(profileRepositoryProvider).update(email: value);
      await ref.read(sessionProvider.notifier).updateClient(client);
    } on ApiException catch (e) {
      if (context.mounted) {
        ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(e.message)));
      }
    }
  }

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final client = ref.watch(sessionProvider).client;
    final phone = client == null
        ? ''
        : '${client.countryCode} ${client.phone}';

    return Scaffold(
      backgroundColor: Colors.white,
      appBar: ScreenAppBar(title: l10n.account),
      body: ListView(
        children: [
          SettingsTile(
            icon: Icons.person_outline,
            title: (client?.name?.isNotEmpty ?? false)
                ? client!.name!
                : l10n.enterYourName,
            onTap: () => _editName(context, ref),
          ),
          SettingsTile(
            icon: Icons.mail_outline,
            title: (client?.email?.isNotEmpty ?? false)
                ? client!.email!
                : l10n.enterYourEmail,
            onTap: () => _editEmail(context, ref),
          ),
          SettingsTile(
            icon: Icons.lock_outline,
            title: l10n.changePassword,
            onTap: () => ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text(l10n.comingSoon)),
            ),
          ),
          SettingsTile(
            icon: Icons.phone_outlined,
            title: phone.isEmpty ? l10n.phoneNumber : phone,
            onTap: () => context.push('/profile/change-phone'),
          ),
          SettingsTile(
            icon: Icons.location_on_outlined,
            title: l10n.addAddresses,
            onTap: () => ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text(l10n.comingSoon)),
            ),
          ),
          SettingsTile(
            icon: Icons.credit_card,
            title: l10n.managePayments,
            onTap: () => context.push('/profile/payments'),
          ),
        ],
      ),
    );
  }
}

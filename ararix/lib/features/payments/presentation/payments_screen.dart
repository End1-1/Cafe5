import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';
import '../data/payments_repository.dart';

final paymentsProvider = FutureProvider.autoDispose((ref) {
  return ref.watch(paymentsRepositoryProvider).list();
});

class PaymentsScreen extends ConsumerWidget {
  const PaymentsScreen({super.key});

  IconData _iconFor(String brand) {
    switch (brand.toLowerCase()) {
      case 'visa':
        return Icons.credit_card;
      case 'mastercard':
        return Icons.credit_card;
      default:
        return Icons.credit_card_outlined;
    }
  }

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final async = ref.watch(paymentsProvider);

    return Scaffold(
      backgroundColor: Colors.white,
      appBar: ScreenAppBar(title: l10n.payments),
      body: async.when(
        loading: () => const Center(child: CircularProgressIndicator()),
        error: (e, _) => Center(child: Text(e.toString())),
        data: (cards) {
          return ListView(
            children: [
              ...cards.map(
                (card) => SettingsTile(
                  icon: _iconFor(card.brand),
                  title: '**** **** **** ${card.last4}',
                  onTap: () {},
                ),
              ),
              SettingsTile(
                icon: Icons.add_card,
                title: l10n.addNewCard,
                onTap: () => ScaffoldMessenger.of(context).showSnackBar(
                  SnackBar(content: Text(l10n.comingSoon)),
                ),
              ),
              if (cards.isEmpty)
                const Padding(
                  padding: EdgeInsets.all(20),
                  child: Text(
                    'No cards yet',
                    style: TextStyle(color: AppColors.muted),
                  ),
                ),
            ],
          );
        },
      ),
    );
  }
}

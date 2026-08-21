import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';
import '../../auth/application/session_notifier.dart';

class ProfileHubScreen extends ConsumerWidget {
  const ProfileHubScreen({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final client = ref.watch(sessionProvider).client;
    final name = (client?.name?.isNotEmpty ?? false) ? client!.name! : 'Guest';

    return Scaffold(
      backgroundColor: Colors.white,
      body: CustomScrollView(
        slivers: [
          SliverToBoxAdapter(
            child: Stack(
              clipBehavior: Clip.none,
              children: [
                Container(
                  height: 180,
                  decoration: const BoxDecoration(
                    gradient: LinearGradient(
                      colors: [Color(0xFF5C4033), Color(0xFFC9A57A)],
                    ),
                  ),
                ),
                Positioned(
                  left: 0,
                  right: 0,
                  bottom: -40,
                  child: Container(
                    margin: const EdgeInsets.symmetric(horizontal: 0),
                    padding: const EdgeInsets.fromLTRB(20, 48, 20, 16),
                    decoration: const BoxDecoration(
                      color: Colors.white,
                      borderRadius: BorderRadius.vertical(top: Radius.circular(24)),
                    ),
                    child: Row(
                      children: [
                        CircleAvatar(
                          radius: 36,
                          backgroundColor: AppColors.background,
                          child: Text(
                            name.isNotEmpty ? name[0].toUpperCase() : '?',
                            style: const TextStyle(
                              fontSize: 28,
                              fontWeight: FontWeight.w700,
                            ),
                          ),
                        ),
                        const SizedBox(width: 14),
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Text(
                              name,
                              style: const TextStyle(
                                fontSize: 22,
                                fontWeight: FontWeight.w800,
                              ),
                            ),
                            Text(
                              l10n.silverStatus,
                              style: const TextStyle(color: AppColors.muted),
                            ),
                          ],
                        ),
                      ],
                    ),
                  ),
                ),
              ],
            ),
          ),
          const SliverToBoxAdapter(child: SizedBox(height: 56)),
          SliverToBoxAdapter(
            child: Padding(
              padding: const EdgeInsets.fromLTRB(20, 0, 20, 8),
              child: Text(
                l10n.profile,
                style: const TextStyle(fontSize: 18, fontWeight: FontWeight.w700),
              ),
            ),
          ),
          SliverList(
            delegate: SliverChildListDelegate([
              SettingsTile(
                icon: Icons.person_outline,
                title: l10n.account,
                onTap: () => context.push('/profile/account'),
              ),
              SettingsTile(
                icon: Icons.history,
                title: l10n.orderHistory,
                onTap: () => ScaffoldMessenger.of(context).showSnackBar(
                  SnackBar(content: Text(l10n.comingSoon)),
                ),
              ),
              SettingsTile(
                icon: Icons.credit_card,
                title: l10n.paymentMethods,
                onTap: () => context.push('/profile/payments'),
              ),
              SettingsTile(
                icon: Icons.translate,
                title: l10n.language,
                onTap: () => context.push('/profile/language'),
              ),
              SettingsTile(
                icon: Icons.help_outline,
                title: l10n.faq,
                onTap: () => context.push('/profile/faq'),
              ),
              const Padding(
                padding: EdgeInsets.symmetric(horizontal: 20, vertical: 8),
                child: Divider(thickness: 2, color: Color(0xFF90CAF9)),
              ),
              SettingsTile(
                icon: Icons.notifications_none,
                title: l10n.notifications,
                onTap: () => ScaffoldMessenger.of(context).showSnackBar(
                  SnackBar(content: Text(l10n.comingSoon)),
                ),
              ),
              SettingsTile(
                icon: Icons.delete_outline,
                title: l10n.deleteAccount,
                onTap: () async {
                  // Temporary: act as logout until real account deletion exists.
                  await ref.read(sessionProvider.notifier).logout();
                  if (context.mounted) {
                    context.go('/welcome');
                  }
                },
              ),
              const SizedBox(height: 24),
            ]),
          ),
        ],
      ),
    );
  }
}

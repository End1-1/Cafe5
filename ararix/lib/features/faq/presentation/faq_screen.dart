import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';
import '../../auth/application/session_notifier.dart';
import '../data/faq_repository.dart';

final faqProvider = FutureProvider.autoDispose((ref) {
  final locale = ref.watch(sessionProvider).locale;
  return ref.watch(faqRepositoryProvider).list(locale: locale);
});

class FaqScreen extends ConsumerStatefulWidget {
  const FaqScreen({super.key});

  @override
  ConsumerState<FaqScreen> createState() => _FaqScreenState();
}

class _FaqScreenState extends ConsumerState<FaqScreen> {
  int? _expandedId;

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final async = ref.watch(faqProvider);

    return Scaffold(
      backgroundColor: Colors.white,
      appBar: ScreenAppBar(title: l10n.faq),
      body: async.when(
        loading: () => const Center(child: CircularProgressIndicator()),
        error: (e, _) => Center(child: Text(e.toString())),
        data: (items) {
          return ListView(
            children: [
              Padding(
                padding: const EdgeInsets.fromLTRB(20, 16, 20, 8),
                child: Text(
                  l10n.helpAndSupport,
                  style: const TextStyle(
                    fontSize: 18,
                    fontWeight: FontWeight.w700,
                  ),
                ),
              ),
              ...items.map((item) {
                final open = _expandedId == item.id;
                return Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    ListTile(
                      contentPadding: const EdgeInsets.symmetric(horizontal: 20),
                      title: Text(item.question),
                      trailing: Icon(open ? Icons.remove : Icons.add),
                      onTap: () {
                        setState(() {
                          _expandedId = open ? null : item.id;
                        });
                      },
                    ),
                    if (open)
                      Padding(
                        padding: const EdgeInsets.fromLTRB(20, 0, 20, 12),
                        child: Text(
                          item.answer,
                          style: const TextStyle(
                            color: AppColors.muted,
                            height: 1.4,
                          ),
                        ),
                      ),
                    const Divider(height: 1, color: AppColors.border),
                  ],
                );
              }),
            ],
          );
        },
      ),
    );
  }
}

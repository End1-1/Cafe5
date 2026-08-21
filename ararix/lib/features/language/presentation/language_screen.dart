import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';
import '../../auth/application/session_notifier.dart';
import '../../profile/data/profile_repository.dart';

class LanguageScreen extends ConsumerWidget {
  const LanguageScreen({super.key});

  Future<void> _select(
    BuildContext context,
    WidgetRef ref,
    String code,
  ) async {
    await ref.read(sessionProvider.notifier).setLocale(code);
    try {
      final client = await ref.read(profileRepositoryProvider).update(locale: code);
      await ref.read(sessionProvider.notifier).updateClient(client);
    } catch (_) {
      // Locale is still applied locally even if API fails.
    }
  }

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final selected = ref.watch(sessionProvider).locale;

    final languages = [
      (code: 'en', title: l10n.english, native: l10n.englishNative, flag: '🇬🇧'),
      (code: 'ru', title: l10n.russian, native: l10n.russianNative, flag: '🇷🇺'),
      (code: 'hy', title: l10n.armenian, native: l10n.armenianNative, flag: '🇦🇲'),
    ];

    return Scaffold(
      backgroundColor: Colors.white,
      appBar: ScreenAppBar(title: l10n.language),
      body: ListView(
        children: [
          Padding(
            padding: const EdgeInsets.fromLTRB(20, 16, 20, 8),
            child: Text(
              l10n.chooseLanguage,
              style: const TextStyle(fontSize: 18, fontWeight: FontWeight.w700),
            ),
          ),
          ...languages.map((lang) {
            final isSelected = selected == lang.code;
            return Column(
              children: [
                ListTile(
                  contentPadding: const EdgeInsets.symmetric(horizontal: 20),
                  leading: Text(lang.flag, style: const TextStyle(fontSize: 22)),
                  title: Text(
                    lang.title,
                    style: const TextStyle(fontWeight: FontWeight.w700),
                  ),
                  subtitle: Text(
                    lang.native,
                    style: const TextStyle(color: AppColors.muted),
                  ),
                  trailing: isSelected
                      ? const Icon(Icons.check, color: Colors.black)
                      : null,
                  onTap: () => _select(context, ref, lang.code),
                ),
                const Divider(height: 1, color: AppColors.border),
              ],
            );
          }),
        ],
      ),
    );
  }
}

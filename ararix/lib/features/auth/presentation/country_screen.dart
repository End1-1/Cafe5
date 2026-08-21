import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';

import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';

class CountryScreen extends StatefulWidget {
  const CountryScreen({super.key});

  @override
  State<CountryScreen> createState() => _CountryScreenState();
}

class _CountryScreenState extends State<CountryScreen> {
  final _query = TextEditingController();

  @override
  void dispose() {
    _query.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final countries = [
      (name: l10n.armenia, code: '+374'),
    ];
    final filtered = countries
        .where((c) =>
            c.name.toLowerCase().contains(_query.text.toLowerCase()) ||
            c.code.contains(_query.text))
        .toList();

    return Scaffold(
      backgroundColor: Colors.white,
      appBar: ScreenAppBar(title: l10n.selectCountry),
      body: Column(
        children: [
          Padding(
            padding: const EdgeInsets.all(16),
            child: TextField(
              controller: _query,
              onChanged: (_) => setState(() {}),
              decoration: InputDecoration(
                hintText: l10n.searchCountry,
                prefixIcon: const Icon(Icons.search, color: AppColors.muted),
                filled: true,
                fillColor: AppColors.background,
                border: OutlineInputBorder(
                  borderRadius: BorderRadius.circular(12),
                  borderSide: BorderSide.none,
                ),
              ),
            ),
          ),
          Expanded(
            child: ListView.separated(
              itemCount: filtered.length,
              separatorBuilder: (context, index) =>
                  const Divider(height: 1, color: AppColors.border),
              itemBuilder: (context, index) {
                final item = filtered[index];
                return ListTile(
                  leading: const Text('🇦🇲', style: TextStyle(fontSize: 22)),
                  title: Text('${item.name} (${item.code})'),
                  onTap: () => context.pop(item.code),
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}

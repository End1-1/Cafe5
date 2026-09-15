import 'package:flutter/material.dart';

import '../../data/menu_models.dart';
import '../../../../l10n/app_localizations.dart';

class BjuRow extends StatelessWidget {
  const BjuRow({super.key, required this.bju, this.compact = false});

  final BjuInfo bju;
  final bool compact;

  @override
  Widget build(BuildContext context) {
    if (!bju.hasAny) return const SizedBox.shrink();
    final l10n = AppLocalizations.of(context);

    Widget box(String value, String label) {
      return Container(
        padding: EdgeInsets.symmetric(
          horizontal: compact ? 8 : 10,
          vertical: compact ? 6 : 8,
        ),
        decoration: BoxDecoration(
          color: const Color(0xFFF0F0F0),
          borderRadius: BorderRadius.circular(10),
        ),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(
              value,
              style: TextStyle(
                fontWeight: FontWeight.w700,
                fontSize: compact ? 11 : 12,
              ),
            ),
            const SizedBox(height: 2),
            Text(
              label,
              style: TextStyle(
                color: Colors.black54,
                fontSize: compact ? 9 : 10,
              ),
            ),
          ],
        ),
      );
    }

    String fmt(double v, {bool kcal = false}) {
      if (kcal) {
        return v == v.roundToDouble()
            ? v.toStringAsFixed(0)
            : v.toStringAsFixed(0);
      }
      final n = v == v.roundToDouble()
          ? v.toStringAsFixed(0)
          : v.toStringAsFixed(0);
      return '$n gr';
    }

    return Wrap(
      spacing: 6,
      runSpacing: 6,
      children: [
        if (bju.fat > 0) box(fmt(bju.fat), l10n.fats),
        if (bju.carbs > 0) box(fmt(bju.carbs), l10n.carbs),
        if (bju.protein > 0) box(fmt(bju.protein), l10n.protein),
        if (bju.kcal > 0) box(fmt(bju.kcal, kcal: true), l10n.calories),
      ],
    );
  }
}

class DietaryIconRow extends StatelessWidget {
  const DietaryIconRow({super.key, required this.dietary});

  final DietaryFlags dietary;

  @override
  Widget build(BuildContext context) {
    if (!dietary.hasAny) return const SizedBox.shrink();

    final items = <({IconData icon, Color color, String tip})>[];
    if (dietary.noLactose) {
      items.add((icon: Icons.local_drink_outlined, color: const Color(0xFFBDBDBD), tip: 'LF'));
    }
    if (dietary.containsNuts) {
      items.add((icon: Icons.spa_outlined, color: const Color(0xFFE57373), tip: 'Nuts'));
    }
    if (dietary.glutenFree) {
      items.add((icon: Icons.grain, color: const Color(0xFFFFB74D), tip: 'GF'));
    }
    if (dietary.vegan || dietary.vegetarian) {
      items.add((icon: Icons.eco_outlined, color: const Color(0xFF81C784), tip: 'Veg'));
    }
    if (dietary.halalKosher) {
      items.add((icon: Icons.verified_outlined, color: const Color(0xFF64B5F6), tip: 'H/K'));
    }

    if (items.isEmpty) return const SizedBox.shrink();

    return Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        for (final it in items)
          Padding(
            padding: const EdgeInsets.only(left: 4),
            child: Tooltip(
              message: it.tip,
              child: CircleAvatar(
                radius: 12,
                backgroundColor: it.color.withValues(alpha: 0.35),
                child: Icon(it.icon, size: 14, color: Colors.black87),
              ),
            ),
          ),
      ],
    );
  }
}

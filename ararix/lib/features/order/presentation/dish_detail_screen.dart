import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../core/theme/app_theme.dart';
import '../../../l10n/app_localizations.dart';
import '../application/cart_notifier.dart';
import '../application/restaurant_notifier.dart';
import '../data/menu_models.dart';
import 'widgets/bju_row.dart';
import 'widgets/menu_image.dart';
import 'widgets/qty_control.dart';

class DishDetailScreen extends ConsumerStatefulWidget {
  const DishDetailScreen({
    super.key,
    required this.restaurantId,
    required this.dishId,
  });

  final int restaurantId;
  final int dishId;

  @override
  ConsumerState<DishDetailScreen> createState() => _DishDetailScreenState();
}

class _DishDetailScreenState extends ConsumerState<DishDetailScreen> {
  int _qty = 1;
  final Set<int> _selectedAddonIds = {};

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final order = ref.watch(restaurantOrderProvider(widget.restaurantId));
    MenuDish? dish;
    for (final d in order.menu?.dishes ?? const <MenuDish>[]) {
      if (d.id == widget.dishId) {
        dish = d;
        break;
      }
    }

    if (order.loading && dish == null) {
      return const Scaffold(
        body: Center(child: CircularProgressIndicator()),
      );
    }
    if (dish == null) {
      return Scaffold(
        appBar: AppBar(),
        body: Center(child: Text(l10n.errorGeneric)),
      );
    }

    final d = dish;
    final selectedAddons = d.modificators
        .where((m) => _selectedAddonIds.contains(m.id))
        .map((m) => SelectedAddon(id: m.id, name: m.name, price: m.price))
        .toList();
    final unit = d.price + selectedAddons.fold<double>(0, (s, a) => s + a.price);
    final total = unit * _qty;

    return Scaffold(
      backgroundColor: Colors.white,
      body: Column(
        children: [
          Expanded(
            child: ListView(
              padding: EdgeInsets.zero,
              children: [
                SizedBox(
                  height: 260,
                  child: Stack(
                    children: [
                      Positioned.fill(child: MenuImage(source: d.image)),
                      Positioned(
                        top: MediaQuery.paddingOf(context).top + 8,
                        right: 12,
                        child: Material(
                          color: Colors.black87,
                          shape: const CircleBorder(),
                          child: InkWell(
                            customBorder: const CircleBorder(),
                            onTap: () => context.pop(),
                            child: const SizedBox(
                              width: 40,
                              height: 40,
                              child: Icon(Icons.close, color: Colors.white),
                            ),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
                Transform.translate(
                  offset: const Offset(0, -20),
                  child: Container(
                    width: double.infinity,
                    padding: const EdgeInsets.fromLTRB(20, 24, 20, 8),
                    decoration: const BoxDecoration(
                      color: Colors.white,
                      borderRadius:
                          BorderRadius.vertical(top: Radius.circular(24)),
                    ),
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          d.name,
                          style: const TextStyle(
                            fontSize: 28,
                            fontWeight: FontWeight.w800,
                          ),
                        ),
                        if (d.sizeLabel.isNotEmpty) ...[
                          const SizedBox(height: 4),
                          Text(
                            '(${d.sizeLabel})',
                            style: const TextStyle(
                              color: Colors.black54,
                              fontSize: 16,
                            ),
                          ),
                        ],
                        if (d.description.isNotEmpty) ...[
                          const SizedBox(height: 10),
                          Text(
                            d.description,
                            style: const TextStyle(
                              fontSize: 15,
                              height: 1.35,
                            ),
                          ),
                        ],
                        const SizedBox(height: 16),
                        Row(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Expanded(child: BjuRow(bju: d.bju)),
                            DietaryIconRow(dietary: d.dietary),
                          ],
                        ),
                        if (d.modificators.isNotEmpty) ...[
                          const SizedBox(height: 22),
                          Text(
                            l10n.addons,
                            style: const TextStyle(
                              fontSize: 20,
                              fontWeight: FontWeight.w800,
                            ),
                          ),
                          const SizedBox(height: 8),
                          for (final m in d.modificators)
                            CheckboxListTile(
                              contentPadding: EdgeInsets.zero,
                              controlAffinity: ListTileControlAffinity.leading,
                              value: _selectedAddonIds.contains(m.id),
                              onChanged: (v) {
                                setState(() {
                                  if (v == true) {
                                    _selectedAddonIds.add(m.id);
                                  } else {
                                    _selectedAddonIds.remove(m.id);
                                  }
                                });
                              },
                              title: Text(m.name),
                              secondary: Text(
                                formatMoney(m.price),
                                style: const TextStyle(
                                  fontWeight: FontWeight.w700,
                                ),
                              ),
                            ),
                        ],
                      ],
                    ),
                  ),
                ),
              ],
            ),
          ),
          SafeArea(
            top: false,
            child: Padding(
              padding: const EdgeInsets.fromLTRB(16, 8, 16, 12),
              child: Row(
                children: [
                  QtyPill(
                    qty: _qty,
                    onPlus: () => setState(() => _qty++),
                    onMinusOrRemove: () {
                      if (_qty <= 1) {
                        context.pop();
                        return;
                      }
                      setState(() => _qty--);
                    },
                  ),
                  const SizedBox(width: 12),
                  Expanded(
                    child: FilledButton(
                      onPressed: () {
                        ref.read(cartProvider.notifier).ensureRestaurant(
                              widget.restaurantId,
                            );
                        ref.read(cartProvider.notifier).addDish(
                              d,
                              qty: _qty,
                              addons: selectedAddons,
                            );
                        context.pop();
                      },
                      style: FilledButton.styleFrom(
                        backgroundColor: AppColors.accent,
                        foregroundColor: Colors.black,
                        padding: const EdgeInsets.symmetric(vertical: 16),
                        shape: RoundedRectangleBorder(
                          borderRadius: BorderRadius.circular(28),
                        ),
                      ),
                      child: Text(
                        l10n.addNFor(_qty, formatMoney(total)),
                        style: const TextStyle(fontWeight: FontWeight.w800),
                      ),
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}

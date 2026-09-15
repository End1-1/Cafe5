import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../core/theme/app_theme.dart';
import '../../../l10n/app_localizations.dart';
import '../application/cart_notifier.dart';
import '../application/restaurant_notifier.dart';
import '../data/menu_models.dart';
import '../data/package_helpers.dart';
import 'widgets/bju_row.dart';
import 'widgets/menu_image.dart';
import 'widgets/qty_control.dart';

enum _PkgStep { personalize, drink, extra }

class PackagePickScreen extends ConsumerStatefulWidget {
  const PackagePickScreen({
    super.key,
    required this.restaurantId,
    required this.dishId,
  });

  final int restaurantId;
  final int dishId;

  @override
  ConsumerState<PackagePickScreen> createState() => _PackagePickScreenState();
}

class _PackagePickScreenState extends ConsumerState<PackagePickScreen> {
  int _qty = 1;
  _PkgStep _step = _PkgStep.personalize;
  final Set<int> _selectedAddonIds = {};
  Map<String, String> _selections = {};
  bool _attrsReady = false;
  final Map<int, int> _drinkQty = {};
  final Map<int, int> _extraQty = {};

  void _ensureAttrs(MenuDish pkg) {
    if (_attrsReady) return;
    _attrsReady = true;
    if (pkg.packageComponents.isNotEmpty) {
      _selections = defaultAttributeSelections(pkg.packageComponents);
    }
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final order = ref.watch(restaurantOrderProvider(widget.restaurantId));
    final menu = order.menu;
    final pkg = menu?.dishById(widget.dishId);

    if (order.loading && pkg == null) {
      return const Scaffold(body: Center(child: CircularProgressIndicator()));
    }
    if (pkg == null || menu == null) {
      return Scaffold(
        appBar: AppBar(),
        body: Center(child: Text(l10n.errorGeneric)),
      );
    }

    _ensureAttrs(pkg);

    final hasAttrs = packageNeedsAttributePicker(pkg.packageComponents) ||
        pkg.packageComponents.isNotEmpty;
    final hasMods = pkg.modificators.isNotEmpty;
    final drinkDishes = _resolveRelated(menu, pkg.relatedDrinks);
    final extraDishes = _resolveRelated(menu, pkg.relatedOther);
    final hasDrink = drinkDishes.isNotEmpty;
    final hasExtra = extraDishes.isNotEmpty;
    final emptyPackage = pkg.packageComponents.isEmpty &&
        !hasMods &&
        !hasDrink &&
        !hasExtra;

    final component = hasAttrs && pkg.packageComponents.isNotEmpty
        ? findPackageComponent(pkg.packageComponents, _selections)
        : (pkg.packageComponents.isNotEmpty ? pkg.packageComponents.first : null);

    final selectedAddons = pkg.modificators
        .where((m) => _selectedAddonIds.contains(m.id))
        .map((m) => SelectedAddon(id: m.id, name: m.name, price: m.price))
        .toList();

    final baseUnit = component != null
        ? packageLinePrice(pkg, component)
        : pkg.price;
    final modsSum = selectedAddons.fold<double>(0, (s, a) => s + a.price);
    final relatedSum = _relatedTotal(drinkDishes, _drinkQty) +
        _relatedTotal(extraDishes, _extraQty);
    final unitOk = !hasAttrs ||
        pkg.packageComponents.isEmpty ||
        component != null;
    final lineTotal = unitOk
        ? (baseUnit + modsSum) * _qty + relatedSum
        : 0.0;

    final steps = <_PkgStep>[
      _PkgStep.personalize,
      if (hasDrink) _PkgStep.drink,
      if (hasExtra) _PkgStep.extra,
    ];
    final stepIndex = steps.indexOf(_step).clamp(0, steps.length - 1);
    final isLast = stepIndex >= steps.length - 1;

    return Scaffold(
      backgroundColor: Colors.white,
      body: Column(
        children: [
          Expanded(
            child: ListView(
              padding: EdgeInsets.zero,
              children: [
                SizedBox(
                  height: 220,
                  child: Stack(
                    children: [
                      Positioned.fill(
                        child: MenuImage(
                          source: component?.image ?? pkg.image,
                        ),
                      ),
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
                  offset: const Offset(0, -16),
                  child: Container(
                    padding: const EdgeInsets.fromLTRB(20, 20, 20, 8),
                    decoration: const BoxDecoration(
                      color: Colors.white,
                      borderRadius:
                          BorderRadius.vertical(top: Radius.circular(24)),
                    ),
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          pkg.name,
                          style: const TextStyle(
                            fontSize: 26,
                            fontWeight: FontWeight.w800,
                          ),
                        ),
                        const SizedBox(height: 8),
                        _StepTabs(
                          steps: steps,
                          current: _step,
                          labels: {
                            _PkgStep.personalize: l10n.personalize,
                            _PkgStep.drink: l10n.drinkStep,
                            _PkgStep.extra: l10n.extraStep,
                          },
                          onSelect: (s) => setState(() => _step = s),
                        ),
                        const SizedBox(height: 12),
                        Row(
                          children: [
                            Expanded(child: BjuRow(bju: pkg.bju, compact: true)),
                            DietaryIconRow(dietary: pkg.dietary),
                          ],
                        ),
                        const SizedBox(height: 16),
                        if (_step == _PkgStep.personalize)
                          _PersonalizeBody(
                            pkg: pkg,
                            hasAttrs: hasAttrs,
                            hasMods: hasMods,
                            selections: _selections,
                            selectedAddonIds: _selectedAddonIds,
                            unitOk: unitOk,
                            onSelectAttr: (key, value) {
                              setState(() => _selections[key] = value);
                            },
                            onToggleMod: (id, on) {
                              setState(() {
                                if (on) {
                                  _selectedAddonIds.add(id);
                                } else {
                                  _selectedAddonIds.remove(id);
                                }
                              });
                            },
                          )
                        else if (_step == _PkgStep.drink)
                          _RelatedBody(
                            title: l10n.chooseDrink,
                            dishes: drinkDishes,
                            qtyMap: _drinkQty,
                            onChanged: (id, q) {
                              setState(() {
                                if (q <= 0) {
                                  _drinkQty.remove(id);
                                } else {
                                  _drinkQty[id] = q;
                                }
                              });
                            },
                          )
                        else
                          _RelatedBody(
                            title: l10n.addExtra,
                            dishes: extraDishes,
                            qtyMap: _extraQty,
                            onChanged: (id, q) {
                              setState(() {
                                if (q <= 0) {
                                  _extraQty.remove(id);
                                } else {
                                  _extraQty[id] = q;
                                }
                              });
                            },
                          ),
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
              child: Column(
                children: [
                  if (_step == _PkgStep.personalize) ...[
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
                    const SizedBox(height: 10),
                  ],
                  Row(
                    children: [
                      if (stepIndex > 0)
                        OutlinedButton(
                          onPressed: () {
                            setState(() => _step = steps[stepIndex - 1]);
                          },
                          child: Text(l10n.back),
                        ),
                      if (stepIndex > 0) const SizedBox(width: 10),
                      Expanded(
                        child: FilledButton(
                          onPressed: emptyPackage ||
                                  (!unitOk && _step == _PkgStep.personalize)
                              ? null
                              : () {
                                  if (!isLast) {
                                    setState(
                                      () => _step = steps[stepIndex + 1],
                                    );
                                    return;
                                  }
                                  _commit(
                                    pkg: pkg,
                                    component: component,
                                    addons: selectedAddons,
                                    drinkDishes: drinkDishes,
                                    extraDishes: extraDishes,
                                  );
                                },
                          style: FilledButton.styleFrom(
                            backgroundColor: AppColors.accent,
                            foregroundColor: Colors.black,
                            disabledBackgroundColor: const Color(0xFFE0E0E0),
                            padding: const EdgeInsets.symmetric(vertical: 16),
                            shape: RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(28),
                            ),
                          ),
                          child: Text(
                            isLast
                                ? l10n.addNFor(_qty, formatMoney(lineTotal))
                                : l10n.next,
                            style: const TextStyle(fontWeight: FontWeight.w800),
                          ),
                        ),
                      ),
                    ],
                  ),
                  if (!unitOk && _step == _PkgStep.personalize) ...[
                    const SizedBox(height: 8),
                    Text(
                      l10n.comboUnavailable,
                      style: const TextStyle(color: Colors.redAccent, fontSize: 12),
                      textAlign: TextAlign.center,
                    ),
                  ],
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }

  List<MenuDish> _resolveRelated(
    RestaurantMenu menu,
    List<MenuRelatedRef> refs,
  ) {
    final out = <MenuDish>[];
    for (final ref in refs) {
      final d = menu.dishById(ref.id);
      if (d != null) {
        out.add(d);
      } else if (ref.name.isNotEmpty) {
        // Related id may point to goods not on active menu — skip silently.
      }
    }
    return out;
  }

  double _relatedTotal(List<MenuDish> dishes, Map<int, int> qty) {
    var sum = 0.0;
    for (final d in dishes) {
      final q = qty[d.id] ?? 0;
      if (q > 0) sum += d.price * q;
    }
    return sum;
  }

  void _commit({
    required MenuDish pkg,
    required PackageComponent? component,
    required List<SelectedAddon> addons,
    required List<MenuDish> drinkDishes,
    required List<MenuDish> extraDishes,
  }) {
    final cart = ref.read(cartProvider.notifier);
    cart.ensureRestaurant(widget.restaurantId);

    final lineDish = component != null
        ? resolvePackageCartLine(pkg, component)
        : pkg.copyWith(needsConfig: false);

    cart.addDish(lineDish, qty: _qty, addons: addons);

    for (final d in drinkDishes) {
      final q = _drinkQty[d.id] ?? 0;
      if (q > 0) cart.addDish(d, qty: q);
    }
    for (final d in extraDishes) {
      final q = _extraQty[d.id] ?? 0;
      if (q > 0) cart.addDish(d, qty: q);
    }
    context.pop();
  }
}

class _StepTabs extends StatelessWidget {
  const _StepTabs({
    required this.steps,
    required this.current,
    required this.labels,
    required this.onSelect,
  });

  final List<_PkgStep> steps;
  final _PkgStep current;
  final Map<_PkgStep, String> labels;
  final ValueChanged<_PkgStep> onSelect;

  @override
  Widget build(BuildContext context) {
    if (steps.length <= 1) return const SizedBox.shrink();
    return Row(
      children: [
        for (var i = 0; i < steps.length; i++) ...[
          if (i > 0) const SizedBox(width: 8),
          Expanded(
            child: InkWell(
              onTap: () => onSelect(steps[i]),
              borderRadius: BorderRadius.circular(14),
              child: Container(
                height: 40,
                alignment: Alignment.center,
                decoration: BoxDecoration(
                  color: current == steps[i]
                      ? AppColors.accent
                      : const Color(0xFFF2F2F2),
                  borderRadius: BorderRadius.circular(14),
                ),
                child: Text(
                  labels[steps[i]] ?? '',
                  style: const TextStyle(fontWeight: FontWeight.w700, fontSize: 12),
                  textAlign: TextAlign.center,
                ),
              ),
            ),
          ),
        ],
      ],
    );
  }
}

class _PersonalizeBody extends StatelessWidget {
  const _PersonalizeBody({
    required this.pkg,
    required this.hasAttrs,
    required this.hasMods,
    required this.selections,
    required this.selectedAddonIds,
    required this.unitOk,
    required this.onSelectAttr,
    required this.onToggleMod,
  });

  final MenuDish pkg;
  final bool hasAttrs;
  final bool hasMods;
  final Map<String, String> selections;
  final Set<int> selectedAddonIds;
  final bool unitOk;
  final void Function(String key, String value) onSelectAttr;
  final void Function(int id, bool on) onToggleMod;

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final options = uniqueAttributeOptions(pkg.packageComponents);

    if (!hasAttrs && !hasMods) {
      return Text(
        l10n.packageNoOptions,
        style: const TextStyle(color: Colors.black54),
      );
    }

    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          l10n.customizePackage(pkg.name),
          style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w700),
        ),
        const SizedBox(height: 12),
        for (final key in attributeKeys) ...[
          if ((options[key] ?? const []).length > 1) ...[
            Text(
              key == 'Type' ? l10n.attrType : l10n.attrSize,
              style: const TextStyle(fontWeight: FontWeight.w800),
            ),
            const SizedBox(height: 8),
            for (final value in options[key]!)
              ListTile(
                contentPadding: EdgeInsets.zero,
                dense: true,
                onTap: () => onSelectAttr(key, value),
                leading: Icon(
                  selections[key] == value
                      ? Icons.radio_button_checked
                      : Icons.radio_button_off,
                  color: selections[key] == value
                      ? AppColors.accent
                      : Colors.black45,
                ),
                title: Text(value),
                trailing: Text(
                  formatMoney(
                    fixedAttributeOptionPrice(
                      pkg.packageComponents,
                      key,
                      value,
                    ),
                  ),
                  style: const TextStyle(fontWeight: FontWeight.w600),
                ),
              ),
            const SizedBox(height: 8),
          ],
        ],
        if (hasMods) ...[
          Text(
            l10n.addons,
            style: const TextStyle(fontWeight: FontWeight.w800, fontSize: 16),
          ),
          for (final m in pkg.modificators)
            CheckboxListTile(
              contentPadding: EdgeInsets.zero,
              controlAffinity: ListTileControlAffinity.leading,
              value: selectedAddonIds.contains(m.id),
              onChanged: (v) => onToggleMod(m.id, v == true),
              title: Text(m.name),
              secondary: Text(
                formatMoney(m.price),
                style: const TextStyle(fontWeight: FontWeight.w700),
              ),
            ),
        ],
      ],
    );
  }
}

class _RelatedBody extends StatelessWidget {
  const _RelatedBody({
    required this.title,
    required this.dishes,
    required this.qtyMap,
    required this.onChanged,
  });

  final String title;
  final List<MenuDish> dishes;
  final Map<int, int> qtyMap;
  final void Function(int id, int qty) onChanged;

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          title,
          style: const TextStyle(fontSize: 18, fontWeight: FontWeight.w800),
        ),
        const SizedBox(height: 12),
        for (final d in dishes) ...[
          Row(
            children: [
              ClipRRect(
                borderRadius: BorderRadius.circular(12),
                child: SizedBox(
                  width: 56,
                  height: 56,
                  child: MenuImage(source: d.image),
                ),
              ),
              const SizedBox(width: 12),
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      d.name,
                      style: const TextStyle(fontWeight: FontWeight.w700),
                    ),
                    Text(
                      formatMoney(d.price),
                      style: const TextStyle(color: Colors.black54),
                    ),
                  ],
                ),
              ),
              QtyControl(
                qty: qtyMap[d.id] ?? 0,
                compact: true,
                onPlus: () => onChanged(d.id, (qtyMap[d.id] ?? 0) + 1),
                onMinusOrRemove: () =>
                    onChanged(d.id, (qtyMap[d.id] ?? 0) - 1),
              ),
            ],
          ),
          const SizedBox(height: 12),
        ],
      ],
    );
  }
}

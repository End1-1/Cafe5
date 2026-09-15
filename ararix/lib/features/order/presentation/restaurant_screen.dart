import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../core/theme/app_theme.dart';
import '../../../l10n/app_localizations.dart';
import '../../home/application/home_notifier.dart';
import '../application/cart_notifier.dart';
import '../application/restaurant_notifier.dart';
import '../data/menu_models.dart';
import 'widgets/bju_row.dart';
import 'widgets/menu_image.dart';
import 'widgets/qty_control.dart';

class RestaurantScreen extends ConsumerWidget {
  const RestaurantScreen({super.key, required this.restaurantId});

  final int restaurantId;

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final order = ref.watch(restaurantOrderProvider(restaurantId));
    final cart = ref.watch(cartProvider);
    final serviceMode = ref.watch(homeProvider.select((s) => s.serviceMode));
    final restaurant = order.menu?.restaurant;

    return Scaffold(
      backgroundColor: Colors.white,
      body: order.loading && order.menu == null
          ? const Center(child: CircularProgressIndicator())
          : order.error != null && order.menu == null
              ? Center(
                  child: Padding(
                    padding: const EdgeInsets.all(24),
                    child: Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        Text(order.error!, textAlign: TextAlign.center),
                        const SizedBox(height: 12),
                        FilledButton(
                          onPressed: () => ref
                              .read(restaurantOrderProvider(restaurantId)
                                  .notifier)
                              .load(),
                          child: Text(l10n.retry),
                        ),
                      ],
                    ),
                  ),
                )
              : Column(
                  children: [
                    Expanded(
                      child: CustomScrollView(
                        slivers: [
                          SliverToBoxAdapter(
                            child: _HeroHeader(
                              restaurant: restaurant,
                              addressLabel: 'Komitas Avenue, 8',
                              onBack: () => context.pop(),
                            ),
                          ),
                          SliverToBoxAdapter(
                            child: _InfoCard(
                              restaurant: restaurant,
                              serviceMode: serviceMode,
                              delivery: l10n.delivery,
                              takeaway: l10n.takeaway,
                              dineIn: l10n.dineIn,
                              onServiceMode: (m) => ref
                                  .read(restaurantOrderProvider(restaurantId)
                                      .notifier)
                                  .setServiceMode(m),
                            ),
                          ),
                          if (order.menu != null &&
                              order.menu!.groups.isNotEmpty)
                            SliverPersistentHeader(
                              pinned: true,
                              delegate: _CategoryChipsDelegate(
                                groups: order.menu!.groups,
                                selectedId: order.selectedGroupId,
                                onSelect: (id) => ref
                                    .read(restaurantOrderProvider(restaurantId)
                                        .notifier)
                                    .selectGroup(id),
                              ),
                            ),
                          if (order.selectedGroupId != null)
                            ..._buildGroupSections(
                              context,
                              ref,
                              order,
                              cart,
                              l10n,
                            ),
                          const SliverToBoxAdapter(
                            child: SizedBox(height: 24),
                          ),
                        ],
                      ),
                    ),
                    _OrderFooter(
                      etaMin: restaurant?.etaMin ?? 55,
                      total: cart.totalAmount,
                      enabled: !cart.isEmpty,
                      onNext: () => context.push('/cart'),
                    ),
                  ],
                ),
    );
  }

  List<Widget> _buildGroupSections(
    BuildContext context,
    WidgetRef ref,
    RestaurantOrderState order,
    CartState cart,
    AppLocalizations l10n,
  ) {
    final group = order.menu!.groups.firstWhere(
      (g) => g.id == order.selectedGroupId,
      orElse: () => order.menu!.groups.first,
    );
    final dishes = order.dishesInSelectedGroup;
    return [
      SliverToBoxAdapter(
        child: Padding(
          padding: const EdgeInsets.fromLTRB(16, 16, 16, 8),
          child: Text(
            group.name,
            style: const TextStyle(fontSize: 22, fontWeight: FontWeight.w800),
          ),
        ),
      ),
      if (dishes.isEmpty)
        SliverToBoxAdapter(
          child: Padding(
            padding: const EdgeInsets.all(24),
            child: Text(l10n.noDishesInGroup, textAlign: TextAlign.center),
          ),
        )
      else
        SliverList(
          delegate: SliverChildBuilderDelegate(
            (context, i) {
              final dish = dishes[i];
              final qty = cart.qtyForDish(dish.id);
              return _DishRow(
                dish: dish,
                qty: qty,
                onOpen: () {
                  final path = dish.isPackage
                      ? '/restaurant/$restaurantId/package/${dish.id}'
                      : '/restaurant/$restaurantId/dish/${dish.id}';
                  context.push(path);
                },
                onPlus: () {
                  if (dish.isPackage || dish.needsConfig) {
                    final path = dish.isPackage
                        ? '/restaurant/$restaurantId/package/${dish.id}'
                        : '/restaurant/$restaurantId/dish/${dish.id}';
                    context.push(path);
                  } else {
                    ref
                        .read(cartProvider.notifier)
                        .incrementSimpleDish(dish);
                  }
                },
                onMinusOrRemove: () {
                  ref
                      .read(cartProvider.notifier)
                      .decrementDishAggregate(dish.id);
                },
              );
            },
            childCount: dishes.length,
          ),
        ),
    ];
  }
}

class _HeroHeader extends StatelessWidget {
  const _HeroHeader({
    required this.restaurant,
    required this.addressLabel,
    required this.onBack,
  });

  final RestaurantInfo? restaurant;
  final String addressLabel;
  final VoidCallback onBack;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: 220,
      child: Stack(
        clipBehavior: Clip.none,
        children: [
          Positioned.fill(
            child: MenuImage(source: restaurant?.imageUrl),
          ),
          Positioned(
            top: MediaQuery.paddingOf(context).top + 8,
            left: 12,
            right: 12,
            child: Row(
              children: [
                _IconCircle(icon: Icons.arrow_back, onTap: onBack),
                const SizedBox(width: 8),
                Expanded(
                  child: Container(
                    height: 40,
                    padding: const EdgeInsets.symmetric(horizontal: 12),
                    decoration: BoxDecoration(
                      color: Colors.white.withValues(alpha: 0.92),
                      borderRadius: BorderRadius.circular(20),
                    ),
                    child: Row(
                      children: [
                        const Icon(Icons.location_on_outlined, size: 18),
                        const SizedBox(width: 6),
                        Expanded(
                          child: Text(
                            addressLabel,
                            overflow: TextOverflow.ellipsis,
                            style: const TextStyle(fontWeight: FontWeight.w600),
                          ),
                        ),
                        const Icon(Icons.expand_more, size: 18),
                      ],
                    ),
                  ),
                ),
                const SizedBox(width: 8),
                const _IconCircle(icon: Icons.favorite_border),
                const SizedBox(width: 8),
                const _IconCircle(icon: Icons.search),
              ],
            ),
          ),
          Positioned(
            left: 20,
            bottom: -28,
            child: Container(
              width: 64,
              height: 64,
              decoration: BoxDecoration(
                color: Colors.white,
                borderRadius: BorderRadius.circular(14),
                border: Border.all(color: Colors.white, width: 3),
                boxShadow: const [
                  BoxShadow(
                    color: Colors.black12,
                    blurRadius: 8,
                    offset: Offset(0, 2),
                  ),
                ],
              ),
              clipBehavior: Clip.antiAlias,
              child: MenuImage(
                source: restaurant?.logoUrl ?? restaurant?.imageUrl,
                placeholder: Center(
                  child: Text(
                    (restaurant?.name.isNotEmpty == true)
                        ? restaurant!.name.characters.first
                        : '?',
                    style: const TextStyle(
                      fontWeight: FontWeight.w800,
                      fontSize: 22,
                    ),
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _IconCircle extends StatelessWidget {
  const _IconCircle({required this.icon, this.onTap});
  final IconData icon;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return Material(
      color: Colors.white.withValues(alpha: 0.92),
      shape: const CircleBorder(),
      child: InkWell(
        customBorder: const CircleBorder(),
        onTap: onTap,
        child: SizedBox(
          width: 40,
          height: 40,
          child: Icon(icon, size: 20),
        ),
      ),
    );
  }
}

class _InfoCard extends StatelessWidget {
  const _InfoCard({
    required this.restaurant,
    required this.serviceMode,
    required this.delivery,
    required this.takeaway,
    required this.dineIn,
    required this.onServiceMode,
  });

  final RestaurantInfo? restaurant;
  final ServiceMode serviceMode;
  final String delivery;
  final String takeaway;
  final String dineIn;
  final ValueChanged<ServiceMode> onServiceMode;

  @override
  Widget build(BuildContext context) {
    final r = restaurant;
    final dist = r?.distanceM;
    final distLabel = dist == null
        ? null
        : dist >= 1000
            ? '${(dist / 1000).toStringAsFixed(1)} km'
            : '${dist}m';

    return Container(
      width: double.infinity,
      margin: const EdgeInsets.only(top: 36),
      padding: const EdgeInsets.fromLTRB(16, 8, 16, 12),
      decoration: const BoxDecoration(
        color: Colors.white,
        borderRadius: BorderRadius.vertical(top: Radius.circular(24)),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            r?.name ?? '',
            style: const TextStyle(fontSize: 26, fontWeight: FontWeight.w800),
          ),
          const SizedBox(height: 6),
          Row(
            children: [
              if (distLabel != null) ...[
                Text(distLabel, style: const TextStyle(color: Colors.black54)),
                const Text('  ·  ', style: TextStyle(color: Colors.black38)),
              ],
              if ((r?.category ?? '').isNotEmpty)
                Expanded(
                  child: Text(
                    r!.category!,
                    style: const TextStyle(color: Colors.black54),
                    overflow: TextOverflow.ellipsis,
                  ),
                ),
              const Icon(Icons.star, size: 16, color: Colors.amber),
              const SizedBox(width: 4),
              Text(
                (r?.rating ?? 0).toStringAsFixed(1),
                style: const TextStyle(fontWeight: FontWeight.w700),
              ),
            ],
          ),
          const SizedBox(height: 14),
          Row(
            children: [
              _modeChip(delivery, ServiceMode.delivery, const Color(0xFFEAF7D4)),
              const SizedBox(width: 8),
              _modeChip(takeaway, ServiceMode.takeaway, const Color(0xFFE8F1FF)),
              const SizedBox(width: 8),
              _modeChip(dineIn, ServiceMode.dineIn, const Color(0xFFFFF0E0)),
            ],
          ),
        ],
      ),
    );
  }

  Widget _modeChip(String label, ServiceMode mode, Color idle) {
    final on = serviceMode == mode;
    return Expanded(
      child: InkWell(
        onTap: () => onServiceMode(mode),
        borderRadius: BorderRadius.circular(18),
        child: Container(
          height: 44,
          alignment: Alignment.center,
          decoration: BoxDecoration(
            color: on ? AppColors.accent : idle,
            borderRadius: BorderRadius.circular(18),
          ),
          child: Text(label, style: const TextStyle(fontWeight: FontWeight.w700)),
        ),
      ),
    );
  }
}

class _CategoryChipsDelegate extends SliverPersistentHeaderDelegate {
  _CategoryChipsDelegate({
    required this.groups,
    required this.selectedId,
    required this.onSelect,
  });

  final List<MenuGroup> groups;
  final int? selectedId;
  final ValueChanged<int> onSelect;

  @override
  double get minExtent => 52;
  @override
  double get maxExtent => 52;

  @override
  Widget build(
    BuildContext context,
    double shrinkOffset,
    bool overlapsContent,
  ) {
    return Container(
      color: Colors.white,
      alignment: Alignment.centerLeft,
      child: ListView.separated(
        padding: const EdgeInsets.symmetric(horizontal: 16),
        scrollDirection: Axis.horizontal,
        itemCount: groups.length,
        separatorBuilder: (_, __) => const SizedBox(width: 18),
        itemBuilder: (context, i) {
          final g = groups[i];
          final on = g.id == selectedId;
          return InkWell(
            onTap: () => onSelect(g.id),
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Text(
                  g.name,
                  style: TextStyle(
                    fontWeight: on ? FontWeight.w800 : FontWeight.w500,
                    color: on ? Colors.black : Colors.black54,
                  ),
                ),
                const SizedBox(height: 6),
                Container(
                  height: 3,
                  width: 28,
                  color: on ? Colors.black : Colors.transparent,
                ),
              ],
            ),
          );
        },
      ),
    );
  }

  @override
  bool shouldRebuild(covariant _CategoryChipsDelegate oldDelegate) {
    return oldDelegate.selectedId != selectedId ||
        oldDelegate.groups != groups;
  }
}

class _DishRow extends StatelessWidget {
  const _DishRow({
    required this.dish,
    required this.qty,
    required this.onOpen,
    required this.onPlus,
    required this.onMinusOrRemove,
  });

  final MenuDish dish;
  final int qty;
  final VoidCallback onOpen;
  final VoidCallback onPlus;
  final VoidCallback onMinusOrRemove;

  @override
  Widget build(BuildContext context) {
    final size = dish.sizeLabel;
    return InkWell(
      onTap: onOpen,
      child: Padding(
        padding: const EdgeInsets.fromLTRB(16, 12, 16, 12),
        child: Row(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            ClipOval(
              child: SizedBox(
                width: 72,
                height: 72,
                child: MenuImage(source: dish.image),
              ),
            ),
            const SizedBox(width: 12),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Row(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Expanded(
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Text(
                              dish.name,
                              style: const TextStyle(
                                fontWeight: FontWeight.w800,
                                fontSize: 16,
                              ),
                            ),
                            if (size.isNotEmpty)
                              Text(
                                size,
                                style: const TextStyle(color: Colors.black54),
                              ),
                          ],
                        ),
                      ),
                      Text(
                        formatMoney(dish.price),
                        style: const TextStyle(
                          fontWeight: FontWeight.w800,
                          fontSize: 16,
                        ),
                      ),
                    ],
                  ),
                  if (dish.description.isNotEmpty) ...[
                    const SizedBox(height: 4),
                    Text(
                      dish.description,
                      maxLines: 2,
                      overflow: TextOverflow.ellipsis,
                      style: const TextStyle(color: Colors.black54, fontSize: 13),
                    ),
                  ],
                  const SizedBox(height: 8),
                  Row(
                    crossAxisAlignment: CrossAxisAlignment.end,
                    children: [
                      Expanded(
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            BjuRow(bju: dish.bju, compact: true),
                            const SizedBox(height: 6),
                            DietaryIconRow(dietary: dish.dietary),
                          ],
                        ),
                      ),
                      QtyControl(
                        qty: qty,
                        onPlus: onPlus,
                        onMinusOrRemove: onMinusOrRemove,
                        compact: true,
                      ),
                    ],
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _OrderFooter extends StatelessWidget {
  const _OrderFooter({
    required this.etaMin,
    required this.total,
    required this.enabled,
    required this.onNext,
  });

  final int etaMin;
  final double total;
  final bool enabled;
  final VoidCallback onNext;

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    return SafeArea(
      top: false,
      child: Container(
        padding: const EdgeInsets.fromLTRB(16, 10, 16, 10),
        decoration: const BoxDecoration(
          color: Colors.white,
          boxShadow: [
            BoxShadow(
              color: Colors.black12,
              blurRadius: 10,
              offset: Offset(0, -2),
            ),
          ],
        ),
        child: Row(
          children: [
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              mainAxisSize: MainAxisSize.min,
              children: [
                Row(
                  children: [
                    const Icon(Icons.schedule, size: 16, color: Colors.black54),
                    const SizedBox(width: 4),
                    Text(
                      '$etaMin ${l10n.minShort}',
                      style: const TextStyle(fontWeight: FontWeight.w600),
                    ),
                  ],
                ),
                const SizedBox(height: 4),
                Row(
                  children: [
                    const Icon(Icons.local_offer_outlined,
                        size: 16, color: Colors.black54),
                    const SizedBox(width: 4),
                    Text(
                      formatMoney(total),
                      style: const TextStyle(fontWeight: FontWeight.w800),
                    ),
                  ],
                ),
              ],
            ),
            const Spacer(),
            FilledButton(
              onPressed: enabled ? onNext : null,
              style: FilledButton.styleFrom(
                backgroundColor: AppColors.accent,
                foregroundColor: Colors.black,
                disabledBackgroundColor: const Color(0xFFE0E0E0),
                padding:
                    const EdgeInsets.symmetric(horizontal: 28, vertical: 14),
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(28),
                ),
              ),
              child: Row(
                children: [
                  Text(l10n.next,
                      style: const TextStyle(fontWeight: FontWeight.w800)),
                  const SizedBox(width: 6),
                  const Icon(Icons.chevron_right),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}

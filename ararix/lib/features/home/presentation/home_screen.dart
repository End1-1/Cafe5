import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../../../core/theme/app_theme.dart';
import '../../../l10n/app_localizations.dart';
import '../application/home_notifier.dart';
import '../data/home_models.dart';

class HomeScreen extends ConsumerWidget {
  const HomeScreen({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final state = ref.watch(homeProvider);
    final feed = state.feed;

    return Scaffold(
      backgroundColor: Colors.white,
      body: SafeArea(
        child: RefreshIndicator(
          onRefresh: () => ref.read(homeProvider.notifier).load(),
          child: CustomScrollView(
            physics: const AlwaysScrollableScrollPhysics(),
            slivers: [
              SliverPad(
                child: _AddressRow(label: feed?.address.label ?? '…'),
              ),
              if (state.loading && feed == null)
                const SliverFillRemaining(
                  child: Center(child: CircularProgressIndicator()),
                )
              else if (state.error != null && feed == null)
                SliverFillRemaining(
                  child: Center(
                    child: Padding(
                      padding: const EdgeInsets.all(24),
                      child: Column(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Text(state.error!, textAlign: TextAlign.center),
                          const SizedBox(height: 12),
                          FilledButton(
                            onPressed: () => ref.read(homeProvider.notifier).load(),
                            child: Text(l10n.retry),
                          ),
                        ],
                      ),
                    ),
                  ),
                )
              else if (feed != null) ...[
                SliverPad(child: _PromoBanner(promo: feed.promo)),
                SliverPad(
                  top: 16,
                  child: _SearchField(
                    hint: l10n.searchRestaurant,
                    onChanged: (v) =>
                        ref.read(homeProvider.notifier).setSearchQuery(v),
                  ),
                ),
                SliverPad(
                  top: 16,
                  child: _ServiceModeRow(
                    selected: state.serviceMode,
                    delivery: l10n.delivery,
                    takeaway: l10n.takeaway,
                    dineIn: l10n.dineIn,
                    onSelect: (m) =>
                        ref.read(homeProvider.notifier).setServiceMode(m),
                  ),
                ),
                SliverPad(
                  top: 22,
                  bottom: 12,
                  child: _SectionTitle(l10n.topRestaurants),
                ),
                SliverBox(
                  child: SizedBox(
                    height: 96,
                    child: ListView.separated(
                      padding: const EdgeInsets.symmetric(horizontal: 16),
                      scrollDirection: Axis.horizontal,
                      itemCount: state.filteredRestaurants.length,
                      separatorBuilder: (_, __) => const SizedBox(width: 12),
                      itemBuilder: (context, i) {
                        return _RestaurantAvatar(
                          restaurant: state.filteredRestaurants[i],
                        );
                      },
                    ),
                  ),
                ),
                SliverPad(
                  top: 22,
                  bottom: 12,
                  child: _SectionTitle(l10n.dishes),
                ),
                SliverBox(
                  child: SizedBox(
                    height: 110,
                    child: ListView.separated(
                      padding: const EdgeInsets.symmetric(horizontal: 16),
                      scrollDirection: Axis.horizontal,
                      itemCount: feed.goodsGroups.length,
                      separatorBuilder: (_, __) => const SizedBox(width: 16),
                      itemBuilder: (context, i) {
                        return _DishGroupItem(group: feed.goodsGroups[i]);
                      },
                    ),
                  ),
                ),
                SliverPad(
                  top: 20,
                  bottom: 8,
                  child: _CuisineRow(
                    countries: feed.countries,
                    selectedId: state.selectedCountryId,
                    onSelect: (id) =>
                        ref.read(homeProvider.notifier).setCountry(id),
                  ),
                ),
                SliverPad(
                  top: 12,
                  bottom: 12,
                  child: _SectionTitle(l10n.topOffer),
                ),
                SliverPad(
                  bottom: 24,
                  child: _TopOfferCard(offer: feed.topOffer),
                ),
              ],
            ],
          ),
        ),
      ),
    );
  }
}

class SliverPad extends StatelessWidget {
  const SliverPad({
    super.key,
    required this.child,
    this.top = 12,
    this.bottom = 0,
  });

  final Widget child;
  final double top;
  final double bottom;

  @override
  Widget build(BuildContext context) {
    return SliverToBoxAdapter(
      child: Padding(
        padding: EdgeInsets.fromLTRB(16, top, 16, bottom),
        child: child,
      ),
    );
  }
}

class SliverBox extends StatelessWidget {
  const SliverBox({super.key, required this.child});
  final Widget child;

  @override
  Widget build(BuildContext context) => SliverToBoxAdapter(child: child);
}

class _AddressRow extends StatelessWidget {
  const _AddressRow({required this.label});
  final String label;

  @override
  Widget build(BuildContext context) {
    return InkWell(
      onTap: () {},
      borderRadius: BorderRadius.circular(12),
      child: Padding(
        padding: const EdgeInsets.symmetric(vertical: 8),
        child: Row(
          children: [
            const Icon(Icons.location_on_outlined, size: 22),
            const SizedBox(width: 8),
            Expanded(
              child: Text(
                label,
                style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w600),
                overflow: TextOverflow.ellipsis,
              ),
            ),
            const Icon(Icons.keyboard_arrow_down),
          ],
        ),
      ),
    );
  }
}

class _PromoBanner extends StatelessWidget {
  const _PromoBanner({required this.promo});
  final HomePromo promo;

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 120,
      width: double.infinity,
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: const Color(0xFF5B8FA8),
        borderRadius: BorderRadius.circular(18),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Text(
            promo.title,
            style: const TextStyle(
              color: Colors.white,
              fontSize: 22,
              fontWeight: FontWeight.w800,
            ),
          ),
          const SizedBox(height: 4),
          Text(
            promo.subtitle,
            style: const TextStyle(color: Colors.white70, fontSize: 14),
          ),
        ],
      ),
    );
  }
}

class _SearchField extends StatelessWidget {
  const _SearchField({required this.hint, required this.onChanged});
  final String hint;
  final ValueChanged<String> onChanged;

  @override
  Widget build(BuildContext context) {
    return TextField(
      onChanged: onChanged,
      decoration: InputDecoration(
        hintText: hint,
        prefixIcon: const Icon(Icons.search),
        filled: true,
        fillColor: const Color(0xFFF2F2F2),
        border: OutlineInputBorder(
          borderRadius: BorderRadius.circular(28),
          borderSide: BorderSide.none,
        ),
        contentPadding: const EdgeInsets.symmetric(horizontal: 16, vertical: 12),
      ),
    );
  }
}

class _ServiceModeRow extends StatelessWidget {
  const _ServiceModeRow({
    required this.selected,
    required this.delivery,
    required this.takeaway,
    required this.dineIn,
    required this.onSelect,
  });

  final ServiceMode selected;
  final String delivery;
  final String takeaway;
  final String dineIn;
  final ValueChanged<ServiceMode> onSelect;

  @override
  Widget build(BuildContext context) {
    Widget chip(String label, ServiceMode mode, Color bg) {
      final isOn = selected == mode;
      return Expanded(
        child: InkWell(
          onTap: () => onSelect(mode),
          borderRadius: BorderRadius.circular(18),
          child: Container(
            height: 52,
            alignment: Alignment.center,
            decoration: BoxDecoration(
              color: isOn ? AppColors.accent : bg,
              borderRadius: BorderRadius.circular(18),
            ),
            child: Text(
              label,
              style: const TextStyle(fontWeight: FontWeight.w700),
            ),
          ),
        ),
      );
    }

    return Row(
      children: [
        chip(delivery, ServiceMode.delivery, const Color(0xFFEAF7D4)),
        const SizedBox(width: 8),
        chip(takeaway, ServiceMode.takeaway, const Color(0xFFE8F1FF)),
        const SizedBox(width: 8),
        chip(dineIn, ServiceMode.dineIn, const Color(0xFFEAF7D4)),
      ],
    );
  }
}

class _SectionTitle extends StatelessWidget {
  const _SectionTitle(this.title);
  final String title;

  @override
  Widget build(BuildContext context) {
    return Text(
      title,
      style: const TextStyle(fontSize: 22, fontWeight: FontWeight.w800),
    );
  }
}

class _RestaurantAvatar extends StatelessWidget {
  const _RestaurantAvatar({required this.restaurant});
  final HomeRestaurant restaurant;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 78,
      child: Column(
        children: [
          Container(
            width: 68,
            height: 68,
            decoration: BoxDecoration(
              color: const Color(0xFFF3F3F3),
              borderRadius: BorderRadius.circular(16),
              border: Border.all(color: AppColors.border),
            ),
            clipBehavior: Clip.antiAlias,
            child: restaurant.imageUrl != null && restaurant.imageUrl!.isNotEmpty
                ? Image.network(restaurant.imageUrl!, fit: BoxFit.cover)
                : Center(
                    child: Text(
                      restaurant.name.isNotEmpty
                          ? restaurant.name[0].toUpperCase()
                          : '?',
                      style: const TextStyle(
                        fontSize: 24,
                        fontWeight: FontWeight.w800,
                      ),
                    ),
                  ),
          ),
          const SizedBox(height: 6),
          Text(
            restaurant.name,
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
            style: const TextStyle(fontSize: 11),
          ),
        ],
      ),
    );
  }
}

class _DishGroupItem extends StatelessWidget {
  const _DishGroupItem({required this.group});
  final HomeGoodsGroup group;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 88,
      child: Column(
        children: [
          Container(
            width: 72,
            height: 72,
            decoration: BoxDecoration(
              color: const Color(0xFFFFF3E0),
              borderRadius: BorderRadius.circular(18),
            ),
            child: group.imageUrl != null && group.imageUrl!.isNotEmpty
                ? ClipRRect(
                    borderRadius: BorderRadius.circular(18),
                    child: Image.network(group.imageUrl!, fit: BoxFit.cover),
                  )
                : Icon(
                    group.name.toLowerCase().contains('pizza')
                        ? Icons.local_pizza_outlined
                        : Icons.lunch_dining_outlined,
                    size: 36,
                    color: Colors.brown,
                  ),
          ),
          const SizedBox(height: 8),
          Text(
            group.name,
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
            style: const TextStyle(fontWeight: FontWeight.w600),
          ),
        ],
      ),
    );
  }
}

class _CuisineRow extends StatelessWidget {
  const _CuisineRow({
    required this.countries,
    required this.selectedId,
    required this.onSelect,
  });

  final List<HomeCountry> countries;
  final int? selectedId;
  final ValueChanged<int?> onSelect;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: 40,
      child: ListView.separated(
        scrollDirection: Axis.horizontal,
        itemCount: countries.length + 1,
        separatorBuilder: (_, __) => const SizedBox(width: 8),
        itemBuilder: (context, i) {
          if (i == 0) {
            return Container(
              width: 40,
              height: 40,
              decoration: BoxDecoration(
                color: const Color(0xFFF2F2F2),
                borderRadius: BorderRadius.circular(12),
              ),
              child: const Icon(Icons.tune, size: 20),
            );
          }
          final c = countries[i - 1];
          final selected = selectedId == c.id;
          return InkWell(
            onTap: () => onSelect(selected ? null : c.id),
            borderRadius: BorderRadius.circular(20),
            child: Container(
              padding: const EdgeInsets.symmetric(horizontal: 14),
              alignment: Alignment.center,
              decoration: BoxDecoration(
                color: selected ? AppColors.accent : const Color(0xFFF2F2F2),
                borderRadius: BorderRadius.circular(20),
              ),
              child: Text(
                c.name,
                style: TextStyle(
                  fontWeight: selected ? FontWeight.w700 : FontWeight.w500,
                ),
              ),
            ),
          );
        },
      ),
    );
  }
}

class _TopOfferCard extends StatelessWidget {
  const _TopOfferCard({required this.offer});
  final HomeTopOffer offer;

  @override
  Widget build(BuildContext context) {
    final distance = offer.distanceM == null
        ? ''
        : (offer.distanceM! >= 1000
            ? '${(offer.distanceM! / 1000).toStringAsFixed(1)} km'
            : '${offer.distanceM}m');

    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Stack(
          children: [
            Container(
              height: 180,
              width: double.infinity,
              decoration: BoxDecoration(
                color: const Color(0xFFE0E0E0),
                borderRadius: BorderRadius.circular(18),
              ),
              clipBehavior: Clip.antiAlias,
              child: offer.imageUrl != null && offer.imageUrl!.isNotEmpty
                  ? Image.network(offer.imageUrl!, fit: BoxFit.cover)
                  : const ColoredBox(
                      color: Color(0xFFBDBDBD),
                      child: Center(
                        child: Icon(Icons.image_outlined, size: 48, color: Colors.white),
                      ),
                    ),
            ),
            if (offer.title.isNotEmpty)
              Positioned(
                left: 12,
                top: 12,
                child: Container(
                  padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
                  decoration: BoxDecoration(
                    color: const Color(0xFFE53935),
                    borderRadius: BorderRadius.circular(8),
                  ),
                  child: Text(
                    offer.title,
                    style: const TextStyle(
                      color: Colors.white,
                      fontWeight: FontWeight.w700,
                    ),
                  ),
                ),
              ),
            Positioned(
              right: 12,
              top: 12,
              child: Material(
                color: Colors.white,
                shape: const CircleBorder(),
                child: IconButton(
                  onPressed: () {},
                  icon: const Icon(Icons.favorite_border, size: 20),
                ),
              ),
            ),
          ],
        ),
        const SizedBox(height: 12),
        Row(
          children: [
            Expanded(
              child: Text(
                offer.restaurantName,
                style: const TextStyle(fontSize: 22, fontWeight: FontWeight.w800),
              ),
            ),
            if (offer.rating != null) ...[
              Text(
                offer.rating!.toStringAsFixed(1),
                style: const TextStyle(fontWeight: FontWeight.w700),
              ),
              const SizedBox(width: 4),
              const Icon(Icons.star, size: 18),
            ],
          ],
        ),
        const SizedBox(height: 4),
        Row(
          children: [
            if (distance.isNotEmpty) ...[
              const Icon(Icons.directions_walk, size: 16, color: AppColors.muted),
              const SizedBox(width: 4),
              Text(distance, style: const TextStyle(color: AppColors.muted)),
              const SizedBox(width: 12),
            ],
            if ((offer.category ?? '').isNotEmpty)
              Flexible(
                child: Text(
                  offer.category!,
                  style: const TextStyle(color: AppColors.muted),
                  overflow: TextOverflow.ellipsis,
                ),
              ),
          ],
        ),
        if (offer.dishName.isNotEmpty) ...[
          const SizedBox(height: 4),
          Text(
            offer.dishName,
            style: const TextStyle(fontWeight: FontWeight.w600),
          ),
        ],
      ],
    );
  }
}

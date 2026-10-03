import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../l10n/app_localizations.dart';
import '../../home/data/home_models.dart';
import '../../order/presentation/widgets/menu_image.dart';
import '../application/menu_browse_notifier.dart';

class MenuScreen extends ConsumerWidget {
  const MenuScreen({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final l10n = AppLocalizations.of(context);
    final state = ref.watch(menuBrowseProvider);
    final showBack = state.level != MenuBrowseLevel.restaurants;
    final title = state.level == MenuBrowseLevel.restaurants
        ? l10n.menu
        : (state.title.isNotEmpty ? state.title : l10n.menu);

    return Scaffold(
      backgroundColor: Colors.white,
      appBar: AppBar(
        backgroundColor: Colors.white,
        elevation: 0,
        scrolledUnderElevation: 0,
        leading: showBack
            ? IconButton(
                icon: const Icon(Icons.arrow_back),
                tooltip: l10n.back,
                onPressed: () => ref.read(menuBrowseProvider.notifier).goBack(),
              )
            : null,
        title: Text(
          title,
          style: const TextStyle(
            color: Colors.black87,
            fontWeight: FontWeight.w700,
          ),
        ),
        centerTitle: false,
      ),
      body: _body(context, ref, state, l10n),
    );
  }

  Widget _body(
    BuildContext context,
    WidgetRef ref,
    MenuBrowseState state,
    AppLocalizations l10n,
  ) {
    switch (state.level) {
      case MenuBrowseLevel.restaurants:
        return _RestaurantsLevel(state: state, l10n: l10n);
      case MenuBrowseLevel.groups:
        return _GroupsLevel(state: state, l10n: l10n);
      case MenuBrowseLevel.dishes:
        return _DishesLevel(state: state, l10n: l10n);
    }
  }
}

class _RestaurantsLevel extends ConsumerWidget {
  const _RestaurantsLevel({required this.state, required this.l10n});

  final MenuBrowseState state;
  final AppLocalizations l10n;

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    if (state.loading && state.restaurants.isEmpty) {
      return const Center(child: CircularProgressIndicator());
    }
    if (state.error != null && state.restaurants.isEmpty) {
      return Center(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              Text(state.error!, textAlign: TextAlign.center),
              const SizedBox(height: 12),
              FilledButton(
                onPressed: () =>
                    ref.read(menuBrowseProvider.notifier).loadRestaurants(),
                child: Text(l10n.retry),
              ),
            ],
          ),
        ),
      );
    }

    return RefreshIndicator(
      onRefresh: () => ref.read(menuBrowseProvider.notifier).loadRestaurants(),
      child: ListView(
        padding: const EdgeInsets.fromLTRB(16, 8, 16, 24),
        children: [
          _MenuSearchField(
            hint: l10n.searchRestaurant,
            suggestions: state.suggestions,
            showingSuggestions: state.showSuggestions,
            onChanged: (v) =>
                ref.read(menuBrowseProvider.notifier).setSearchQuery(v),
            onSuggestionTap: (item) {
              ref.read(menuBrowseProvider.notifier).clearSuggestions();
              if (item.type == HomeSuggestType.restaurant) {
                final match = state.restaurants
                    .where((r) => r.id == item.restaurantId)
                    .toList();
                final restaurant = match.isNotEmpty
                    ? match.first
                    : HomeRestaurant(
                        id: item.restaurantId,
                        name: item.name,
                        score: 0,
                        imageUrl: item.imageUrl,
                        category: item.subtitle,
                      );
                ref
                    .read(menuBrowseProvider.notifier)
                    .openRestaurant(restaurant);
              } else if (item.restaurantId > 0 && item.id > 0) {
                context.push(
                  '/restaurant/${item.restaurantId}/dish/${item.id}',
                );
              } else if (item.restaurantId > 0) {
                context.push('/restaurant/${item.restaurantId}');
              }
            },
          ),
          const SizedBox(height: 16),
          ...state.restaurants.map(
            (r) => Padding(
              padding: const EdgeInsets.only(bottom: 10),
              child: _RestaurantTile(
                restaurant: r,
                onTap: () =>
                    ref.read(menuBrowseProvider.notifier).openRestaurant(r),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _GroupsLevel extends ConsumerWidget {
  const _GroupsLevel({required this.state, required this.l10n});

  final MenuBrowseState state;
  final AppLocalizations l10n;

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    if (state.menuLoading && state.menu == null) {
      return const Center(child: CircularProgressIndicator());
    }
    if (state.menuError != null && state.menu == null) {
      return Center(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              Text(state.menuError!, textAlign: TextAlign.center),
              const SizedBox(height: 12),
              FilledButton(
                onPressed: () {
                  final r = state.selectedRestaurant;
                  if (r != null) {
                    ref.read(menuBrowseProvider.notifier).openRestaurant(r);
                  }
                },
                child: Text(l10n.retry),
              ),
            ],
          ),
        ),
      );
    }

    final groups = state.groups;
    if (groups.isEmpty) {
      return Center(child: Text(l10n.noDishesInGroup));
    }

    return ListView.separated(
      padding: const EdgeInsets.fromLTRB(16, 8, 16, 24),
      itemCount: groups.length,
      separatorBuilder: (_, __) => const SizedBox(height: 10),
      itemBuilder: (context, i) {
        final g = groups[i];
        return ListTile(
          contentPadding:
              const EdgeInsets.symmetric(horizontal: 12, vertical: 4),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(14),
            side: const BorderSide(color: Color(0xFFEEEEEE)),
          ),
          leading: ClipRRect(
            borderRadius: BorderRadius.circular(12),
            child: SizedBox(
              width: 48,
              height: 48,
              child: MenuImage(source: g.image),
            ),
          ),
          title: Text(
            g.name,
            style: const TextStyle(fontWeight: FontWeight.w700),
          ),
          trailing: const Icon(Icons.chevron_right),
          onTap: () => ref.read(menuBrowseProvider.notifier).openGroup(g),
        );
      },
    );
  }
}

class _DishesLevel extends ConsumerWidget {
  const _DishesLevel({required this.state, required this.l10n});

  final MenuBrowseState state;
  final AppLocalizations l10n;

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final dishes = state.dishesInSelectedGroup;
    final restaurantId = state.selectedRestaurant?.id ?? 0;

    if (dishes.isEmpty) {
      return Center(child: Text(l10n.noDishesInGroup));
    }

    return ListView.separated(
      padding: const EdgeInsets.fromLTRB(16, 8, 16, 24),
      itemCount: dishes.length,
      separatorBuilder: (_, __) => const SizedBox(height: 10),
      itemBuilder: (context, i) {
        final d = dishes[i];
        return ListTile(
          contentPadding:
              const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(14),
            side: const BorderSide(color: Color(0xFFEEEEEE)),
          ),
          leading: ClipRRect(
            borderRadius: BorderRadius.circular(12),
            child: SizedBox(
              width: 56,
              height: 56,
              child: MenuImage(source: d.image),
            ),
          ),
          title: Text(
            d.name,
            maxLines: 2,
            overflow: TextOverflow.ellipsis,
            style: const TextStyle(fontWeight: FontWeight.w700),
          ),
          subtitle: Text(
            d.price.toStringAsFixed(d.price == d.price.roundToDouble() ? 0 : 2),
            style: const TextStyle(fontWeight: FontWeight.w600),
          ),
          onTap: restaurantId <= 0
              ? null
              : () {
                  final path = d.type == 5
                      ? '/restaurant/$restaurantId/package/${d.id}'
                      : '/restaurant/$restaurantId/dish/${d.id}';
                  context.push(path);
                },
        );
      },
    );
  }
}

class _RestaurantTile extends StatelessWidget {
  const _RestaurantTile({required this.restaurant, required this.onTap});

  final HomeRestaurant restaurant;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return ListTile(
      contentPadding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(14),
        side: const BorderSide(color: Color(0xFFEEEEEE)),
      ),
      leading: CircleAvatar(
        radius: 24,
        backgroundColor: const Color(0xFFF2F2F2),
        backgroundImage: restaurant.imageUrl != null &&
                restaurant.imageUrl!.isNotEmpty
            ? NetworkImage(restaurant.imageUrl!)
            : null,
        child: restaurant.imageUrl == null || restaurant.imageUrl!.isEmpty
            ? const Icon(Icons.storefront_outlined, color: Colors.black54)
            : null,
      ),
      title: Text(
        restaurant.name,
        style: const TextStyle(fontWeight: FontWeight.w700),
      ),
      subtitle: Text(
        [
          if (restaurant.nationality != null &&
              restaurant.nationality!.isNotEmpty)
            restaurant.nationality!,
          if (restaurant.category != null && restaurant.category!.isNotEmpty)
            restaurant.category!,
        ].join(' · '),
        maxLines: 1,
        overflow: TextOverflow.ellipsis,
      ),
      trailing: const Icon(Icons.chevron_right),
      onTap: onTap,
    );
  }
}

class _MenuSearchField extends StatelessWidget {
  const _MenuSearchField({
    required this.hint,
    required this.onChanged,
    required this.suggestions,
    required this.showingSuggestions,
    required this.onSuggestionTap,
  });

  final String hint;
  final ValueChanged<String> onChanged;
  final List<HomeSuggestItem> suggestions;
  final bool showingSuggestions;
  final ValueChanged<HomeSuggestItem> onSuggestionTap;

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        TextField(
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
            contentPadding:
                const EdgeInsets.symmetric(horizontal: 16, vertical: 12),
          ),
        ),
        if (showingSuggestions) ...[
          const SizedBox(height: 8),
          Material(
            color: Colors.white,
            elevation: 2,
            borderRadius: BorderRadius.circular(16),
            child: ConstrainedBox(
              constraints: const BoxConstraints(maxHeight: 280),
              child: ListView.separated(
                shrinkWrap: true,
                padding: const EdgeInsets.symmetric(vertical: 6),
                itemCount: suggestions.length,
                separatorBuilder: (_, __) => const Divider(height: 1),
                itemBuilder: (context, i) {
                  final item = suggestions[i];
                  final isRestaurant = item.type == HomeSuggestType.restaurant;
                  final subtitle = isRestaurant
                      ? (item.subtitle ?? '')
                      : (item.restaurantName ?? item.subtitle ?? '');
                  return ListTile(
                    dense: true,
                    leading: CircleAvatar(
                      radius: 18,
                      backgroundColor: const Color(0xFFF2F2F2),
                      backgroundImage: item.imageUrl != null &&
                              item.imageUrl!.isNotEmpty
                          ? NetworkImage(item.imageUrl!)
                          : null,
                      child: item.imageUrl == null || item.imageUrl!.isEmpty
                          ? Icon(
                              isRestaurant
                                  ? Icons.storefront_outlined
                                  : Icons.restaurant_menu_outlined,
                              size: 18,
                              color: Colors.black54,
                            )
                          : null,
                    ),
                    title: Text(
                      item.name,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                      style: const TextStyle(fontWeight: FontWeight.w600),
                    ),
                    subtitle: subtitle.isEmpty
                        ? null
                        : Text(
                            subtitle,
                            maxLines: 1,
                            overflow: TextOverflow.ellipsis,
                            style: const TextStyle(fontSize: 12),
                          ),
                    trailing: Icon(
                      isRestaurant
                          ? Icons.chevron_right
                          : Icons.lunch_dining_outlined,
                      size: 18,
                      color: Colors.black45,
                    ),
                    onTap: () => onSuggestionTap(item),
                  );
                },
              ),
            ),
          ),
        ],
      ],
    );
  }
}

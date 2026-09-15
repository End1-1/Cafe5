import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../data/menu_models.dart';

class CartState {
  const CartState({
    this.restaurantId,
    this.lines = const [],
  });

  final int? restaurantId;
  final List<CartLine> lines;

  int get totalQty => lines.fold(0, (s, l) => s + l.qty);

  double get totalAmount => lines.fold(0.0, (s, l) => s + l.lineTotal);

  bool get isEmpty => lines.isEmpty;

  int qtyForDish(int dishId) {
    return lines
        .where((l) => l.dish.id == dishId || l.dish.packageId == dishId)
        .fold(0, (s, l) => s + l.qty);
  }

  CartLine? lineByKey(String key) {
    for (final l in lines) {
      if (l.key == key) return l;
    }
    return null;
  }

  CartState copyWith({
    int? restaurantId,
    List<CartLine>? lines,
    bool clearRestaurant = false,
  }) {
    return CartState(
      restaurantId: clearRestaurant ? null : (restaurantId ?? this.restaurantId),
      lines: lines ?? this.lines,
    );
  }
}

final cartProvider = NotifierProvider<CartNotifier, CartState>(CartNotifier.new);

class CartNotifier extends Notifier<CartState> {
  @override
  CartState build() => const CartState();

  void ensureRestaurant(int restaurantId) {
    if (state.restaurantId == restaurantId) return;
    state = CartState(restaurantId: restaurantId);
  }

  void addDish(
    MenuDish dish, {
    int qty = 1,
    List<SelectedAddon> addons = const [],
  }) {
    if (qty <= 0) return;
    final key = CartLine.makeKey(dish, addons: addons);
    final existing = state.lineByKey(key);
    final lines = [...state.lines];
    if (existing != null) {
      final i = lines.indexWhere((l) => l.key == key);
      lines[i] = existing.copyWith(qty: existing.qty + qty);
    } else {
      lines.add(CartLine(key: key, dish: dish, qty: qty, addons: addons));
    }
    state = state.copyWith(lines: lines);
  }

  void setQtyByKey(String key, int qty) {
    if (qty <= 0) {
      removeByKey(key);
      return;
    }
    final lines = [...state.lines];
    final i = lines.indexWhere((l) => l.key == key);
    if (i < 0) return;
    lines[i] = lines[i].copyWith(qty: qty);
    state = state.copyWith(lines: lines);
  }

  void incrementByKey(String key) {
    final line = state.lineByKey(key);
    if (line == null) return;
    setQtyByKey(key, line.qty + 1);
  }

  void decrementByKey(String key) {
    final line = state.lineByKey(key);
    if (line == null) return;
    setQtyByKey(key, line.qty - 1);
  }

  void incrementSimpleDish(MenuDish dish) {
    if (dish.needsConfig) return;
    addDish(dish, qty: 1);
  }

  void decrementDishAggregate(int dishId) {
    final matching = state.lines
        .where((l) => l.dish.id == dishId || l.dish.packageId == dishId)
        .toList();
    if (matching.isEmpty) return;
    final plain = matching.where((l) => l.addons.isEmpty).toList();
    final target = plain.isNotEmpty ? plain.last : matching.last;
    setQtyByKey(target.key, target.qty - 1);
  }

  void removeByKey(String key) {
    state = state.copyWith(
      lines: state.lines.where((l) => l.key != key).toList(),
    );
  }

  void clear() {
    state = CartState(restaurantId: state.restaurantId);
  }
}

class DietaryFlags {
  const DietaryFlags({
    this.glutenFree = false,
    this.vegetarian = false,
    this.vegan = false,
    this.noGmo = false,
    this.noLactose = false,
    this.noSugar = false,
    this.containsNuts = false,
    this.halalKosher = false,
  });

  final bool glutenFree;
  final bool vegetarian;
  final bool vegan;
  final bool noGmo;
  final bool noLactose;
  final bool noSugar;
  final bool containsNuts;
  final bool halalKosher;

  bool get hasAny =>
      glutenFree ||
      vegetarian ||
      vegan ||
      noGmo ||
      noLactose ||
      noSugar ||
      containsNuts ||
      halalKosher;

  factory DietaryFlags.fromJson(Map<String, dynamic>? json) {
    if (json == null) return const DietaryFlags();
    bool b(String k) => json[k] == true || json[k] == 1 || json[k] == '1';
    return DietaryFlags(
      glutenFree: b('gluten_free'),
      vegetarian: b('vegetarian'),
      vegan: b('vegan'),
      noGmo: b('no_gmo'),
      noLactose: b('no_lactose'),
      noSugar: b('no_sugar'),
      containsNuts: b('contains_nuts'),
      halalKosher: b('halal_kosher'),
    );
  }
}

class BjuInfo {
  const BjuInfo({
    this.kcal = 0,
    this.protein = 0,
    this.fat = 0,
    this.carbs = 0,
  });

  final double kcal;
  final double protein;
  final double fat;
  final double carbs;

  bool get hasAny => kcal > 0 || protein > 0 || fat > 0 || carbs > 0;

  factory BjuInfo.fromJson(Map<String, dynamic>? json) {
    if (json == null) return const BjuInfo();
    double n(String k) => (json[k] as num?)?.toDouble() ?? 0;
    return BjuInfo(
      kcal: n('kcal'),
      protein: n('protein'),
      fat: n('fat'),
      carbs: n('carbs'),
    );
  }
}

class MenuModificator {
  const MenuModificator({
    required this.id,
    required this.name,
    required this.price,
    this.required = false,
  });

  final int id;
  final String name;
  final double price;
  final bool required;

  factory MenuModificator.fromJson(Map<String, dynamic> json) {
    return MenuModificator(
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
      price: (json['price'] as num?)?.toDouble() ?? 0,
      required: json['required'] == true || json['required'] == 1,
    );
  }
}

class MenuRelatedRef {
  const MenuRelatedRef({required this.id, required this.name});

  final int id;
  final String name;

  factory MenuRelatedRef.fromJson(Map<String, dynamic> json) {
    return MenuRelatedRef(
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
    );
  }
}

class PackageComponent {
  const PackageComponent({
    required this.goodsId,
    required this.name,
    required this.price,
    this.qty = 1,
    this.image,
    this.attrType = '',
    this.attrSize = '',
    this.attrMeasurement = '',
    this.attrTypePrice = 0,
    this.attrSizePrice = 0,
    this.bju = const BjuInfo(),
    this.dietary = const DietaryFlags(),
  });

  final int goodsId;
  final String name;
  final double price;
  final double qty;
  final String? image;
  final String attrType;
  final String attrSize;
  final String attrMeasurement;
  final double attrTypePrice;
  final double attrSizePrice;
  final BjuInfo bju;
  final DietaryFlags dietary;

  factory PackageComponent.fromJson(Map<String, dynamic> json) {
    return PackageComponent(
      goodsId: (json['goods_id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
      price: (json['price'] as num?)?.toDouble() ?? 0,
      qty: (json['qty'] as num?)?.toDouble() ?? 1,
      image: json['image']?.toString(),
      attrType: json['attr_type']?.toString() ?? '',
      attrSize: json['attr_size']?.toString() ?? '',
      attrMeasurement: json['attr_measurement']?.toString() ?? '',
      attrTypePrice: (json['attr_type_price'] as num?)?.toDouble() ?? 0,
      attrSizePrice: (json['attr_size_price'] as num?)?.toDouble() ?? 0,
      bju: BjuInfo.fromJson(
        json['bju'] is Map
            ? Map<String, dynamic>.from(json['bju'] as Map)
            : null,
      ),
      dietary: DietaryFlags.fromJson(
        json['dietary'] is Map
            ? Map<String, dynamic>.from(json['dietary'] as Map)
            : null,
      ),
    );
  }
}

class MenuGroup {
  const MenuGroup({
    required this.id,
    required this.name,
    this.image,
  });

  final int id;
  final String name;
  final String? image;

  factory MenuGroup.fromJson(Map<String, dynamic> json) {
    return MenuGroup(
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
      image: json['image']?.toString(),
    );
  }
}

class MenuDish {
  const MenuDish({
    required this.id,
    required this.menuId,
    required this.groupId,
    required this.groupName,
    required this.type,
    required this.name,
    required this.price,
    this.description = '',
    this.image,
    this.prepTime = '',
    this.popular = false,
    this.attrType = '',
    this.attrSize = '',
    this.attrMeasurement = '',
    this.bju = const BjuInfo(),
    this.dietary = const DietaryFlags(),
    this.modificators = const [],
    this.packageComponents = const [],
    this.relatedDrinks = const [],
    this.relatedOther = const [],
    this.needsAttributePicker = false,
    this.needsConfig = false,
    this.packageId = 0,
    this.packageName = '',
  });

  final int id;
  final int menuId;
  final int groupId;
  final String groupName;
  final int type;
  final String name;
  final double price;
  final String description;
  final String? image;
  final String prepTime;
  final bool popular;
  final String attrType;
  final String attrSize;
  final String attrMeasurement;
  final BjuInfo bju;
  final DietaryFlags dietary;
  final List<MenuModificator> modificators;
  final List<PackageComponent> packageComponents;
  final List<MenuRelatedRef> relatedDrinks;
  final List<MenuRelatedRef> relatedOther;
  final bool needsAttributePicker;
  final bool needsConfig;
  final int packageId;
  final String packageName;

  bool get isPackage => type == 5;

  String get sizeLabel {
    if (attrSize.isNotEmpty) return attrSize;
    if (attrMeasurement.isNotEmpty) return attrMeasurement;
    return '';
  }

  MenuDish copyWith({
    int? id,
    int? type,
    String? name,
    double? price,
    String? image,
    String? attrType,
    String? attrSize,
    String? attrMeasurement,
    List<PackageComponent>? packageComponents,
    int? packageId,
    String? packageName,
    bool? needsConfig,
  }) {
    return MenuDish(
      id: id ?? this.id,
      menuId: menuId,
      groupId: groupId,
      groupName: groupName,
      type: type ?? this.type,
      name: name ?? this.name,
      price: price ?? this.price,
      description: description,
      image: image ?? this.image,
      prepTime: prepTime,
      popular: popular,
      attrType: attrType ?? this.attrType,
      attrSize: attrSize ?? this.attrSize,
      attrMeasurement: attrMeasurement ?? this.attrMeasurement,
      bju: bju,
      dietary: dietary,
      modificators: modificators,
      packageComponents: packageComponents ?? this.packageComponents,
      relatedDrinks: relatedDrinks,
      relatedOther: relatedOther,
      needsAttributePicker: needsAttributePicker,
      needsConfig: needsConfig ?? this.needsConfig,
      packageId: packageId ?? this.packageId,
      packageName: packageName ?? this.packageName,
    );
  }

  factory MenuDish.fromJson(Map<String, dynamic> json) {
    final mods = (json['modificators'] is List)
        ? (json['modificators'] as List)
            .whereType<Map>()
            .map((e) => MenuModificator.fromJson(Map<String, dynamic>.from(e)))
            .toList()
        : <MenuModificator>[];
    final components = (json['package_components'] is List)
        ? (json['package_components'] as List)
            .whereType<Map>()
            .map((e) => PackageComponent.fromJson(Map<String, dynamic>.from(e)))
            .toList()
        : <PackageComponent>[];
    List<MenuRelatedRef> related(String key) {
      final raw = json[key];
      if (raw is! List) return const [];
      return raw
          .whereType<Map>()
          .map((e) => MenuRelatedRef.fromJson(Map<String, dynamic>.from(e)))
          .toList();
    }

    final type = (json['type'] as num?)?.toInt() ?? 1;
    final relatedDrinks = related('related_drinks');
    final relatedOther = related('related_other');
    final needsAttr = json['needs_attribute_picker'] == true ||
        json['needs_attribute_picker'] == 1;

    return MenuDish(
      id: (json['id'] as num?)?.toInt() ?? 0,
      menuId: (json['menu_id'] as num?)?.toInt() ?? 0,
      groupId: (json['group_id'] as num?)?.toInt() ?? 0,
      groupName: json['group_name']?.toString() ?? '',
      type: type,
      name: json['name']?.toString() ?? '',
      description: json['description']?.toString() ?? '',
      price: (json['price'] as num?)?.toDouble() ?? 0,
      image: json['image']?.toString(),
      prepTime: json['prep_time']?.toString() ?? '',
      popular: json['popular'] == true || json['popular'] == 1,
      attrType: json['attr_type']?.toString() ?? '',
      attrSize: json['attr_size']?.toString() ?? '',
      attrMeasurement: json['attr_measurement']?.toString() ?? '',
      bju: BjuInfo.fromJson(
        json['bju'] is Map
            ? Map<String, dynamic>.from(json['bju'] as Map)
            : null,
      ),
      dietary: DietaryFlags.fromJson(
        json['dietary'] is Map
            ? Map<String, dynamic>.from(json['dietary'] as Map)
            : null,
      ),
      modificators: mods,
      packageComponents: components,
      relatedDrinks: relatedDrinks,
      relatedOther: relatedOther,
      needsAttributePicker: needsAttr,
      needsConfig: json['needs_config'] == true ||
          json['needs_config'] == 1 ||
          type == 5 ||
          mods.isNotEmpty ||
          relatedDrinks.isNotEmpty ||
          relatedOther.isNotEmpty,
      packageId: (json['package_id'] as num?)?.toInt() ?? 0,
      packageName: json['package_name']?.toString() ?? '',
    );
  }
}

class RestaurantInfo {
  const RestaurantInfo({
    required this.id,
    required this.name,
    required this.score,
    this.imageUrl,
    this.logoUrl,
    this.category,
    this.etaMin = 55,
    this.distanceM,
    this.menuConfigured = true,
  });

  final int id;
  final String name;
  final int score;
  final String? imageUrl;
  final String? logoUrl;
  final String? category;
  final int etaMin;
  final int? distanceM;
  final bool menuConfigured;

  double get rating => (score.clamp(0, 100) / 20).clamp(0, 5).toDouble();

  factory RestaurantInfo.fromJson(Map<String, dynamic> json) {
    return RestaurantInfo(
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
      score: (json['score'] as num?)?.toInt() ?? 0,
      imageUrl: json['image_url']?.toString(),
      logoUrl: json['logo_url']?.toString(),
      category: json['category']?.toString(),
      etaMin: (json['eta_min'] as num?)?.toInt() ?? 55,
      distanceM: (json['distance_m'] as num?)?.toInt(),
      menuConfigured: json['menu_configured'] != false,
    );
  }
}

class RestaurantMenu {
  const RestaurantMenu({
    required this.restaurant,
    required this.groups,
    required this.dishes,
  });

  final RestaurantInfo restaurant;
  final List<MenuGroup> groups;
  final List<MenuDish> dishes;

  MenuDish? dishById(int id) {
    for (final d in dishes) {
      if (d.id == id) return d;
    }
    return null;
  }

  factory RestaurantMenu.fromJson(Map<String, dynamic> json) {
    List<T> mapList<T>(String key, T Function(Map<String, dynamic>) map) {
      final raw = json[key];
      if (raw is! List) return const [];
      return raw
          .whereType<Map>()
          .map((e) => map(Map<String, dynamic>.from(e)))
          .toList();
    }

    return RestaurantMenu(
      restaurant: RestaurantInfo.fromJson(
        Map<String, dynamic>.from(json['restaurant'] as Map? ?? const {}),
      ),
      groups: mapList('groups', MenuGroup.fromJson),
      dishes: mapList('dishes', MenuDish.fromJson),
    );
  }
}

class SelectedAddon {
  const SelectedAddon({
    required this.id,
    required this.name,
    required this.price,
  });

  final int id;
  final String name;
  final double price;
}

class CartLine {
  const CartLine({
    required this.key,
    required this.dish,
    required this.qty,
    this.addons = const [],
  });

  final String key;
  final MenuDish dish;
  final int qty;
  final List<SelectedAddon> addons;

  double get unitPrice =>
      dish.price + addons.fold<double>(0, (s, a) => s + a.price);

  double get lineTotal => unitPrice * qty;

  CartLine copyWith({int? qty, List<SelectedAddon>? addons}) {
    return CartLine(
      key: key,
      dish: dish,
      qty: qty ?? this.qty,
      addons: addons ?? this.addons,
    );
  }

  static String makeKey(
    MenuDish dish, {
    List<SelectedAddon> addons = const [],
  }) {
    final parts = <String>[dish.id.toString()];
    if (dish.packageId > 0) {
      parts.addAll([
        'pkg',
        dish.packageId.toString(),
        dish.attrType,
        dish.attrSize,
      ]);
    }
    final ids = addons.map((a) => a.id).toList()..sort();
    if (ids.isNotEmpty) {
      parts.add(ids.join(','));
    }
    return parts.join(':');
  }
}

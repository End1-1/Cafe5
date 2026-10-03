class HomeAddress {
  const HomeAddress({
    required this.label,
    this.id,
    this.street,
    this.lat,
    this.lng,
  });

  final int? id;
  final String label;
  final String? street;
  final double? lat;
  final double? lng;

  bool get hasAddress =>
      id != null || label.trim().isNotEmpty || (street?.trim().isNotEmpty ?? false);

  String get displayLabel {
    final l = label.trim();
    if (l.isNotEmpty) return l;
    final s = street?.trim() ?? '';
    return s;
  }

  factory HomeAddress.fromJson(Map<String, dynamic> json) {
    return HomeAddress(
      id: (json['id'] as num?)?.toInt(),
      label: json['label']?.toString() ?? '',
      street: json['street']?.toString(),
      lat: (json['lat'] as num?)?.toDouble(),
      lng: (json['lng'] as num?)?.toDouble(),
    );
  }
}

class HomePromo {
  const HomePromo({
    required this.title,
    required this.subtitle,
    this.placeholder = true,
  });

  final String title;
  final String subtitle;
  final bool placeholder;

  factory HomePromo.fromJson(Map<String, dynamic> json) {
    return HomePromo(
      title: json['title']?.toString() ?? '',
      subtitle: json['subtitle']?.toString() ?? '',
      placeholder: json['placeholder'] != false,
    );
  }
}

class HomeRestaurant {
  const HomeRestaurant({
    required this.id,
    required this.name,
    required this.score,
    this.imageUrl,
    this.category,
    this.nationalityId,
    this.nationality,
    this.distanceM,
  });

  final int id;
  final String name;
  final int score;
  final String? imageUrl;
  final String? category;
  final int? nationalityId;
  final String? nationality;
  final int? distanceM;

  factory HomeRestaurant.fromJson(Map<String, dynamic> json) {
    return HomeRestaurant(
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
      score: (json['score'] as num?)?.toInt() ?? 0,
      imageUrl: json['image_url']?.toString(),
      category: json['category']?.toString(),
      nationalityId: (json['nationality_id'] as num?)?.toInt(),
      nationality: json['nationality']?.toString(),
      distanceM: (json['distance_m'] as num?)?.toInt(),
    );
  }
}

enum HomeSuggestType { restaurant, dish }

class HomeSuggestItem {
  const HomeSuggestItem({
    required this.type,
    required this.id,
    required this.name,
    required this.restaurantId,
    this.subtitle,
    this.imageUrl,
    this.restaurantName,
    this.price,
  });

  final HomeSuggestType type;
  final int id;
  final String name;
  final int restaurantId;
  final String? subtitle;
  final String? imageUrl;
  final String? restaurantName;
  final double? price;

  factory HomeSuggestItem.fromJson(Map<String, dynamic> json) {
    final typeRaw = json['type']?.toString() ?? 'restaurant';
    return HomeSuggestItem(
      type: typeRaw == 'dish' ? HomeSuggestType.dish : HomeSuggestType.restaurant,
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
      restaurantId: (json['restaurant_id'] as num?)?.toInt() ?? 0,
      subtitle: json['subtitle']?.toString(),
      imageUrl: json['image_url']?.toString(),
      restaurantName: json['restaurant_name']?.toString(),
      price: (json['price'] as num?)?.toDouble(),
    );
  }
}

class HomeGoodsGroup {
  const HomeGoodsGroup({
    required this.id,
    required this.name,
    this.imageUrl,
  });

  final int id;
  final String name;
  final String? imageUrl;

  factory HomeGoodsGroup.fromJson(Map<String, dynamic> json) {
    return HomeGoodsGroup(
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
      imageUrl: json['image_url']?.toString(),
    );
  }
}

class HomeCountry {
  const HomeCountry({
    required this.id,
    required this.name,
  });

  final int id;
  final String name;

  factory HomeCountry.fromJson(Map<String, dynamic> json) {
    return HomeCountry(
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
    );
  }
}

class HomeTopOffer {
  const HomeTopOffer({
    required this.title,
    required this.dishName,
    required this.restaurantName,
    this.restaurantId,
    this.category,
    this.rating,
    this.distanceM,
    this.imageUrl,
    this.placeholder = true,
  });

  final String title;
  final String dishName;
  final String restaurantName;
  final int? restaurantId;
  final String? category;
  final double? rating;
  final int? distanceM;
  final String? imageUrl;
  final bool placeholder;

  factory HomeTopOffer.fromJson(Map<String, dynamic> json) {
    return HomeTopOffer(
      title: json['title']?.toString() ?? '',
      dishName: json['dish_name']?.toString() ?? '',
      restaurantName: json['restaurant_name']?.toString() ?? '',
      restaurantId: (json['restaurant_id'] as num?)?.toInt(),
      category: json['category']?.toString(),
      rating: (json['rating'] as num?)?.toDouble(),
      distanceM: (json['distance_m'] as num?)?.toInt(),
      imageUrl: json['image_url']?.toString(),
      placeholder: json['placeholder'] != false,
    );
  }
}

class HomeFeed {
  const HomeFeed({
    required this.address,
    required this.promo,
    required this.topRestaurants,
    required this.goodsGroups,
    required this.countries,
    required this.topOffer,
  });

  final HomeAddress address;
  final HomePromo promo;
  final List<HomeRestaurant> topRestaurants;
  final List<HomeGoodsGroup> goodsGroups;
  final List<HomeCountry> countries;
  final HomeTopOffer topOffer;

  factory HomeFeed.fromJson(Map<String, dynamic> json) {
    List<T> mapList<T>(String key, T Function(Map<String, dynamic>) map) {
      final raw = json[key];
      if (raw is! List) return const [];
      return raw
          .whereType<Map>()
          .map((e) => map(Map<String, dynamic>.from(e)))
          .toList();
    }

    return HomeFeed(
      address: HomeAddress.fromJson(
        Map<String, dynamic>.from(json['address'] as Map? ?? const {}),
      ),
      promo: HomePromo.fromJson(
        Map<String, dynamic>.from(json['promo'] as Map? ?? const {}),
      ),
      topRestaurants: mapList('top_restaurants', HomeRestaurant.fromJson),
      goodsGroups: mapList('goods_groups', HomeGoodsGroup.fromJson),
      countries: mapList('countries', HomeCountry.fromJson),
      topOffer: HomeTopOffer.fromJson(
        Map<String, dynamic>.from(json['top_offer'] as Map? ?? const {}),
      ),
    );
  }
}

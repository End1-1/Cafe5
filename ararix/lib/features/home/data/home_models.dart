class HomeAddress {
  const HomeAddress({
    required this.label,
    this.lat,
    this.lng,
  });

  final String label;
  final double? lat;
  final double? lng;

  factory HomeAddress.fromJson(Map<String, dynamic> json) {
    return HomeAddress(
      label: json['label']?.toString() ?? '',
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
    this.distanceM,
  });

  final int id;
  final String name;
  final int score;
  final String? imageUrl;
  final String? category;
  final int? distanceM;

  factory HomeRestaurant.fromJson(Map<String, dynamic> json) {
    return HomeRestaurant(
      id: (json['id'] as num?)?.toInt() ?? 0,
      name: json['name']?.toString() ?? '',
      score: (json['score'] as num?)?.toInt() ?? 0,
      imageUrl: json['image_url']?.toString(),
      category: json['category']?.toString(),
      distanceM: (json['distance_m'] as num?)?.toInt(),
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
    this.category,
    this.rating,
    this.distanceM,
    this.imageUrl,
    this.placeholder = true,
  });

  final String title;
  final String dishName;
  final String restaurantName;
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

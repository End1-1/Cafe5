import 'maps_config.dart';

enum BuildingType {
  house,
  apartment,
  office,
  other;

  static BuildingType fromApi(String? value) {
    switch ((value ?? '').toLowerCase()) {
      case 'apartment':
        return BuildingType.apartment;
      case 'office':
        return BuildingType.office;
      case 'other':
        return BuildingType.other;
      case 'house':
      default:
        return BuildingType.house;
    }
  }

  String get apiValue => name;
}

class DeliveryAddress {
  const DeliveryAddress({
    this.id,
    required this.label,
    required this.street,
    required this.lat,
    required this.lng,
    this.entranceLat,
    this.entranceLng,
    this.buildingType = BuildingType.house,
    this.floor,
    this.door,
    this.comment,
    this.isActive = false,
  });

  final int? id;
  final String label;
  final String street;
  final double lat;
  final double lng;
  final double? entranceLat;
  final double? entranceLng;
  final BuildingType buildingType;
  final String? floor;
  final String? door;
  final String? comment;
  final bool isActive;

  String get displayLabel {
    final l = label.trim();
    if (l.isNotEmpty) return l;
    return street.trim();
  }

  factory DeliveryAddress.fromJson(Map<String, dynamic> json) {
    return DeliveryAddress(
      id: (json['id'] as num?)?.toInt(),
      label: json['label']?.toString() ?? '',
      street: json['street']?.toString() ?? '',
      lat: (json['lat'] as num?)?.toDouble() ?? MapsConfig.defaultLat,
      lng: (json['lng'] as num?)?.toDouble() ?? MapsConfig.defaultLng,
      entranceLat: (json['entrance_lat'] as num?)?.toDouble(),
      entranceLng: (json['entrance_lng'] as num?)?.toDouble(),
      buildingType: BuildingType.fromApi(json['building_type']?.toString()),
      floor: json['floor']?.toString(),
      door: json['door']?.toString(),
      comment: json['comment']?.toString(),
      isActive: json['is_active'] == true || json['is_active'] == 1,
    );
  }

  Map<String, dynamic> toSaveJson({bool setActive = true}) {
    return {
      if (id != null) 'id': id,
      'label': label,
      'street': street,
      'lat': lat,
      'lng': lng,
      if (entranceLat != null) 'entrance_lat': entranceLat,
      if (entranceLng != null) 'entrance_lng': entranceLng,
      'building_type': buildingType.apiValue,
      if (floor != null && floor!.trim().isNotEmpty) 'floor': floor,
      if (door != null && door!.trim().isNotEmpty) 'door': door,
      if (comment != null && comment!.trim().isNotEmpty) 'comment': comment,
      'set_active': setActive,
    };
  }

  DeliveryAddress copyWith({
    int? id,
    String? label,
    String? street,
    double? lat,
    double? lng,
    double? entranceLat,
    double? entranceLng,
    BuildingType? buildingType,
    String? floor,
    String? door,
    String? comment,
    bool? isActive,
    bool clearEntrance = false,
  }) {
    return DeliveryAddress(
      id: id ?? this.id,
      label: label ?? this.label,
      street: street ?? this.street,
      lat: lat ?? this.lat,
      lng: lng ?? this.lng,
      entranceLat: clearEntrance ? null : (entranceLat ?? this.entranceLat),
      entranceLng: clearEntrance ? null : (entranceLng ?? this.entranceLng),
      buildingType: buildingType ?? this.buildingType,
      floor: floor ?? this.floor,
      door: door ?? this.door,
      comment: comment ?? this.comment,
      isActive: isActive ?? this.isActive,
    );
  }
}

class PlaceSuggestion {
  const PlaceSuggestion({
    required this.placeId,
    required this.primaryText,
    this.secondaryText = '',
    this.lat,
    this.lng,
  });

  final String placeId;
  final String primaryText;
  final String secondaryText;
  final double? lat;
  final double? lng;
}

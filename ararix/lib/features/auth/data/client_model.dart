class ClientModel {
  const ClientModel({
    required this.id,
    required this.countryCode,
    required this.phone,
    this.name,
    this.email,
    this.locale = 'en',
    this.avatarUrl,
    this.status = 1,
  });

  final int id;
  final String countryCode;
  final String phone;
  final String? name;
  final String? email;
  final String locale;
  final String? avatarUrl;
  final int status;

  String get fullPhone => '$countryCode$phone';

  factory ClientModel.fromJson(Map<String, dynamic> json) {
    return ClientModel(
      id: (json['id'] as num).toInt(),
      countryCode: json['country_code'] as String? ?? '+374',
      phone: json['phone'] as String? ?? '',
      name: json['name'] as String?,
      email: json['email'] as String?,
      locale: json['locale'] as String? ?? 'en',
      avatarUrl: json['avatar_url'] as String?,
      status: (json['status'] as num?)?.toInt() ?? 1,
    );
  }

  Map<String, dynamic> toJson() => {
        'id': id,
        'country_code': countryCode,
        'phone': phone,
        'name': name,
        'email': email,
        'locale': locale,
        'avatar_url': avatarUrl,
        'status': status,
      };

  ClientModel copyWith({
    String? countryCode,
    String? phone,
    String? name,
    String? email,
    String? locale,
    String? avatarUrl,
  }) {
    return ClientModel(
      id: id,
      countryCode: countryCode ?? this.countryCode,
      phone: phone ?? this.phone,
      name: name ?? this.name,
      email: email ?? this.email,
      locale: locale ?? this.locale,
      avatarUrl: avatarUrl ?? this.avatarUrl,
      status: status,
    );
  }
}

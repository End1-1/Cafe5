import 'dart:convert';

import 'package:flutter/material.dart';

/// Network URL or data:image base64 from menu API.
class MenuImage extends StatelessWidget {
  const MenuImage({
    super.key,
    required this.source,
    this.fit = BoxFit.cover,
    this.placeholder,
  });

  final String? source;
  final BoxFit fit;
  final Widget? placeholder;

  @override
  Widget build(BuildContext context) {
    final src = source?.trim() ?? '';
    if (src.isEmpty) {
      return placeholder ??
          const ColoredBox(
            color: Color(0xFFF0F0F0),
            child: Center(child: Icon(Icons.restaurant, color: Colors.black26)),
          );
    }
    if (src.startsWith('data:image')) {
      final comma = src.indexOf(',');
      if (comma > 0) {
        try {
          final bytes = base64Decode(src.substring(comma + 1));
          return Image.memory(bytes, fit: fit, gaplessPlayback: true);
        } catch (_) {
          return placeholder ?? const ColoredBox(color: Color(0xFFF0F0F0));
        }
      }
    }
    if (src.startsWith('http')) {
      return Image.network(
        src,
        fit: fit,
        errorBuilder: (_, __, ___) =>
            placeholder ?? const ColoredBox(color: Color(0xFFF0F0F0)),
      );
    }
    // Raw base64 without data URI prefix
    try {
      final bytes = base64Decode(src.replaceAll(RegExp(r'\s+'), ''));
      return Image.memory(bytes, fit: fit, gaplessPlayback: true);
    } catch (_) {
      return placeholder ?? const ColoredBox(color: Color(0xFFF0F0F0));
    }
  }
}

String formatMoney(double amount) {
  final v = amount == amount.roundToDouble()
      ? amount.toStringAsFixed(0)
      : amount.toStringAsFixed(0);
  return '$v AMD';
}

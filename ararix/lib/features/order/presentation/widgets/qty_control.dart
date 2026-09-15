import 'package:flutter/material.dart';

class QtyControl extends StatelessWidget {
  const QtyControl({
    super.key,
    required this.qty,
    required this.onPlus,
    required this.onMinusOrRemove,
    this.compact = false,
  });

  final int qty;
  final VoidCallback onPlus;
  final VoidCallback onMinusOrRemove;
  final bool compact;

  static const _yellow = Color(0xFFF5E6A3);
  static const _mint = Color(0xFFA6E24A);

  @override
  Widget build(BuildContext context) {
    final size = compact ? 36.0 : 40.0;
    if (qty <= 0) {
      return _RoundBtn(
        size: size,
        color: _mint,
        icon: Icons.add,
        onTap: onPlus,
      );
    }

    return Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        _RoundBtn(
          size: size,
          color: _yellow,
          icon: qty == 1 ? Icons.delete_outline : Icons.remove,
          onTap: onMinusOrRemove,
        ),
        SizedBox(
          width: compact ? 28 : 32,
          child: Text(
            '$qty',
            textAlign: TextAlign.center,
            style: TextStyle(
              fontWeight: FontWeight.w700,
              fontSize: compact ? 14 : 16,
            ),
          ),
        ),
        _RoundBtn(
          size: size,
          color: _mint,
          icon: Icons.add,
          onTap: onPlus,
        ),
      ],
    );
  }
}

/// Pill used on dish detail: trash/− | qty | +
class QtyPill extends StatelessWidget {
  const QtyPill({
    super.key,
    required this.qty,
    required this.onPlus,
    required this.onMinusOrRemove,
  });

  final int qty;
  final VoidCallback onPlus;
  final VoidCallback onMinusOrRemove;

  static const _yellow = Color(0xFFF5E6A3);
  static const _mint = Color(0xFFA6E24A);

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: BoxDecoration(
        color: const Color(0xFFF0F0F0),
        borderRadius: BorderRadius.circular(16),
      ),
      padding: const EdgeInsets.all(4),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          _SquareBtn(
            color: _yellow,
            icon: qty <= 1 ? Icons.delete_outline : Icons.remove,
            onTap: onMinusOrRemove,
          ),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 18),
            child: Text(
              '$qty',
              style: const TextStyle(fontWeight: FontWeight.w800, fontSize: 18),
            ),
          ),
          _SquareBtn(
            color: _mint,
            icon: Icons.add,
            onTap: onPlus,
          ),
        ],
      ),
    );
  }
}

class _RoundBtn extends StatelessWidget {
  const _RoundBtn({
    required this.size,
    required this.color,
    required this.icon,
    required this.onTap,
  });

  final double size;
  final Color color;
  final IconData icon;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return Material(
      color: color,
      borderRadius: BorderRadius.circular(12),
      child: InkWell(
        onTap: onTap,
        borderRadius: BorderRadius.circular(12),
        child: SizedBox(
          width: size,
          height: size,
          child: Icon(icon, size: size * 0.5),
        ),
      ),
    );
  }
}

class _SquareBtn extends StatelessWidget {
  const _SquareBtn({
    required this.color,
    required this.icon,
    required this.onTap,
  });

  final Color color;
  final IconData icon;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return Material(
      color: color,
      borderRadius: BorderRadius.circular(12),
      child: InkWell(
        onTap: onTap,
        borderRadius: BorderRadius.circular(12),
        child: SizedBox(
          width: 44,
          height: 44,
          child: Icon(icon, size: 22),
        ),
      ),
    );
  }
}

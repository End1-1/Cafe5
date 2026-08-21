import 'package:flutter/material.dart';

import '../theme/app_theme.dart';

class ChannelButtons extends StatelessWidget {
  const ChannelButtons({
    super.key,
    required this.selected,
    required this.onSelect,
    required this.smsLabel,
    required this.whatsappLabel,
  });

  final String selected;
  final ValueChanged<String> onSelect;
  final String smsLabel;
  final String whatsappLabel;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          child: _ChannelButton(
            selected: selected == 'sms',
            label: smsLabel,
            icon: Icons.sms_outlined,
            color: Colors.white,
            borderColor: const Color(0xFF90CAF9),
            onTap: () => onSelect('sms'),
          ),
        ),
        const SizedBox(width: 12),
        Expanded(
          child: _ChannelButton(
            selected: selected == 'whatsapp',
            label: whatsappLabel,
            icon: Icons.chat,
            color: AppColors.accent,
            borderColor: AppColors.accent,
            onTap: () => onSelect('whatsapp'),
          ),
        ),
      ],
    );
  }
}

class _ChannelButton extends StatelessWidget {
  const _ChannelButton({
    required this.selected,
    required this.label,
    required this.icon,
    required this.color,
    required this.borderColor,
    required this.onTap,
  });

  final bool selected;
  final String label;
  final IconData icon;
  final Color color;
  final Color borderColor;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return Material(
      color: color,
      borderRadius: BorderRadius.circular(14),
      child: InkWell(
        onTap: onTap,
        borderRadius: BorderRadius.circular(14),
        child: Container(
          height: 52,
          decoration: BoxDecoration(
            borderRadius: BorderRadius.circular(14),
            border: Border.all(
              color: selected ? borderColor : AppColors.border,
              width: selected ? 2 : 1,
            ),
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Icon(icon, size: 20),
              const SizedBox(width: 8),
              Text(label, style: const TextStyle(fontWeight: FontWeight.w700)),
            ],
          ),
        ),
      ),
    );
  }
}

import 'package:flutter/material.dart';

import '../theme/app_theme.dart';

class PhoneFields extends StatelessWidget {
  const PhoneFields({
    super.key,
    required this.countryCode,
    required this.phoneController,
    required this.onPrefixTap,
    required this.prefixLabel,
    required this.phoneLabel,
  });

  final String countryCode;
  final TextEditingController phoneController;
  final VoidCallback onPrefixTap;
  final String prefixLabel;
  final String phoneLabel;

  @override
  Widget build(BuildContext context) {
    return Row(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Expanded(
          flex: 2,
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(prefixLabel, style: const TextStyle(color: AppColors.muted)),
              const SizedBox(height: 6),
              InkWell(
                onTap: onPrefixTap,
                borderRadius: BorderRadius.circular(12),
                child: Container(
                  height: 52,
                  padding: const EdgeInsets.symmetric(horizontal: 10),
                  decoration: BoxDecoration(
                    color: Colors.white,
                    borderRadius: BorderRadius.circular(12),
                    border: Border.all(color: AppColors.border),
                  ),
                  child: Row(
                    children: [
                      const Text('🇦🇲', style: TextStyle(fontSize: 18)),
                      const SizedBox(width: 6),
                      Expanded(
                        child: Text(
                          countryCode,
                          style: const TextStyle(fontWeight: FontWeight.w600),
                        ),
                      ),
                      const Icon(Icons.keyboard_arrow_down, size: 18),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
        const SizedBox(width: 12),
        Expanded(
          flex: 3,
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(phoneLabel, style: const TextStyle(color: AppColors.muted)),
              const SizedBox(height: 6),
              TextField(
                controller: phoneController,
                keyboardType: TextInputType.phone,
                decoration: const InputDecoration(),
              ),
            ],
          ),
        ),
      ],
    );
  }
}

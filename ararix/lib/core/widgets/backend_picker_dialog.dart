import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../network/api_client.dart';
import '../network/backend_hosts.dart';
import '../theme/app_theme.dart';

Future<void> showBackendPickerDialog(BuildContext context, WidgetRef ref) async {
  final current = ref.read(apiBaseUrlProvider);

  await showModalBottomSheet<void>(
    context: context,
    backgroundColor: Colors.white,
    shape: const RoundedRectangleBorder(
      borderRadius: BorderRadius.vertical(top: Radius.circular(20)),
    ),
    builder: (context) {
      return SafeArea(
        child: Padding(
          padding: const EdgeInsets.fromLTRB(8, 12, 8, 16),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Padding(
                padding: EdgeInsets.fromLTRB(16, 8, 16, 12),
                child: Text(
                  'Backend',
                  style: TextStyle(fontSize: 18, fontWeight: FontWeight.w700),
                ),
              ),
              ...BackendHosts.all.map((host) {
                final selected = host.enabled && host.baseUrl == current;
                return ListTile(
                  enabled: host.enabled,
                  leading: Icon(
                    selected ? Icons.radio_button_checked : Icons.radio_button_off,
                    color: host.enabled ? AppColors.text : AppColors.muted,
                  ),
                  title: Text(
                    host.label,
                    style: TextStyle(
                      color: host.enabled ? AppColors.text : AppColors.muted,
                      fontWeight: selected ? FontWeight.w700 : FontWeight.w500,
                    ),
                  ),
                  subtitle: Text(
                    host.enabled
                        ? host.baseUrl
                        : 'Production address not configured yet',
                    style: const TextStyle(color: AppColors.muted, fontSize: 12),
                  ),
                  onTap: host.enabled
                      ? () async {
                          await ref
                              .read(apiBaseUrlProvider.notifier)
                              .setBaseUrl(host.baseUrl);
                          if (context.mounted) {
                            Navigator.of(context).pop();
                            ScaffoldMessenger.of(context).showSnackBar(
                              SnackBar(content: Text('Backend: ${host.label}')),
                            );
                          }
                        }
                      : null,
                );
              }),
            ],
          ),
        ),
      );
    },
  );
}

import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../core/network/api_exception.dart';
import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/backend_picker_dialog.dart';
import '../../../core/widgets/channel_buttons.dart';
import '../../../core/widgets/phone_fields.dart';
import '../../../l10n/app_localizations.dart';
import '../data/auth_repository.dart';

class WelcomeScreen extends ConsumerStatefulWidget {
  const WelcomeScreen({super.key});

  @override
  ConsumerState<WelcomeScreen> createState() => _WelcomeScreenState();
}

class _WelcomeScreenState extends ConsumerState<WelcomeScreen> {
  final _phone = TextEditingController();
  String _channel = 'whatsapp';
  String _countryCode = '+374';
  bool _loading = false;

  @override
  void dispose() {
    _phone.dispose();
    super.dispose();
  }

  Future<void> _requestOtp() async {
    final l10n = AppLocalizations.of(context);
    final phone = _phone.text.replaceAll(RegExp(r'\D'), '');
    if (phone.length < 6) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(l10n.invalidPhone)),
      );
      return;
    }
    setState(() => _loading = true);
    try {
      await ref.read(authRepositoryProvider).requestOtp(
            countryCode: _countryCode,
            phone: phone,
            channel: _channel,
          );
      if (!mounted) return;
      context.push('/otp', extra: {
        'country_code': _countryCode,
        'phone': phone,
        'channel': _channel,
      });
    } on ApiException catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(e.message)));
    } catch (_) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(l10n.errorGeneric)),
      );
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    return Scaffold(
      backgroundColor: AppColors.background,
      body: SafeArea(
        child: SingleChildScrollView(
          child: Column(
            children: [
              ClipRRect(
                borderRadius: const BorderRadius.vertical(
                  bottom: Radius.circular(28),
                ),
                child: Container(
                  height: 240,
                  width: double.infinity,
                  decoration: const BoxDecoration(
                    gradient: LinearGradient(
                      begin: Alignment.topLeft,
                      end: Alignment.bottomRight,
                      colors: [Color(0xFF3E2A1F), Color(0xFF8B5E3C)],
                    ),
                  ),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Container(
                        width: 72,
                        height: 72,
                        decoration: BoxDecoration(
                          color: Colors.white,
                          borderRadius: BorderRadius.circular(18),
                        ),
                        alignment: Alignment.center,
                        child: const Text(
                          'A',
                          style: TextStyle(
                            color: AppColors.accent,
                            fontSize: 36,
                            fontWeight: FontWeight.w800,
                          ),
                        ),
                      ),
                      const SizedBox(height: 12),
                      Text(
                        l10n.appName,
                        style: const TextStyle(
                          color: Colors.white,
                          fontSize: 22,
                          fontWeight: FontWeight.w700,
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              Padding(
                padding: const EdgeInsets.fromLTRB(20, 24, 20, 20),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      l10n.welcome,
                      style: const TextStyle(
                        fontSize: 32,
                        fontWeight: FontWeight.w800,
                      ),
                    ),
                    const SizedBox(height: 6),
                    Text(
                      l10n.registerWithPhone,
                      style: const TextStyle(color: AppColors.muted),
                    ),
                    const SizedBox(height: 20),
                    PhoneFields(
                      countryCode: _countryCode,
                      phoneController: _phone,
                      prefixLabel: l10n.prefix,
                      phoneLabel: l10n.phoneNumber,
                      onPrefixTap: () async {
                        final code = await context.push<String>('/country');
                        if (code != null) {
                          setState(() => _countryCode = code);
                        }
                      },
                    ),
                    const SizedBox(height: 16),
                    ChannelButtons(
                      selected: _channel,
                      smsLabel: l10n.sms,
                      whatsappLabel: l10n.whatsapp,
                      onSelect: (v) {
                        setState(() => _channel = v);
                        final phone = _phone.text.replaceAll(RegExp(r'\D'), '');
                        if (v == 'sms' && phone == '1981') {
                          showBackendPickerDialog(context, ref);
                          return;
                        }
                        _requestOtp();
                      },
                    ),
                    if (_loading) ...[
                      const SizedBox(height: 12),
                      const Center(child: CircularProgressIndicator()),
                    ],
                    const SizedBox(height: 24),
                    Row(
                      children: [
                        const Expanded(child: Divider()),
                        Padding(
                          padding: const EdgeInsets.symmetric(horizontal: 12),
                          child: Text(
                            l10n.orWith,
                            style: const TextStyle(color: AppColors.muted),
                          ),
                        ),
                        const Expanded(child: Divider()),
                      ],
                    ),
                    const SizedBox(height: 16),
                    Row(
                      children: [
                        Expanded(
                          child: OutlinedButton.icon(
                            onPressed: () {
                              ScaffoldMessenger.of(context).showSnackBar(
                                SnackBar(content: Text(l10n.socialLoginSoon)),
                              );
                            },
                            icon: const Icon(Icons.g_mobiledata, size: 28),
                            label: Text(l10n.google),
                          ),
                        ),
                        const SizedBox(width: 12),
                        Expanded(
                          child: OutlinedButton.icon(
                            onPressed: () {
                              ScaffoldMessenger.of(context).showSnackBar(
                                SnackBar(content: Text(l10n.socialLoginSoon)),
                              );
                            },
                            icon: const Icon(Icons.facebook, color: Color(0xFF1877F2)),
                            label: Text(l10n.facebook),
                          ),
                        ),
                      ],
                    ),
                    const SizedBox(height: 24),
                    Text(
                      l10n.termsFooter,
                      textAlign: TextAlign.center,
                      style: const TextStyle(
                        color: AppColors.muted,
                        fontSize: 12,
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

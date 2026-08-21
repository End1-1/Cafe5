import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../core/network/api_exception.dart';
import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/channel_buttons.dart';
import '../../../core/widgets/otp_input.dart';
import '../../../core/widgets/phone_fields.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';
import '../../auth/application/session_notifier.dart';
import '../data/profile_repository.dart';

class ChangePhoneScreen extends ConsumerStatefulWidget {
  const ChangePhoneScreen({super.key});

  @override
  ConsumerState<ChangePhoneScreen> createState() => _ChangePhoneScreenState();
}

class _ChangePhoneScreenState extends ConsumerState<ChangePhoneScreen> {
  final _phone = TextEditingController();
  String _countryCode = '+374';
  String _channel = 'whatsapp';
  bool _otpStep = false;
  bool _loading = false;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) {
      final client = ref.read(sessionProvider).client;
      if (client != null && mounted) {
        setState(() {
          _countryCode = client.countryCode;
          _phone.text = client.phone;
        });
      }
    });
  }

  @override
  void dispose() {
    _phone.dispose();
    super.dispose();
  }

  Future<void> _request() async {
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
      await ref.read(profileRepositoryProvider).changePhoneRequest(
            countryCode: _countryCode,
            phone: phone,
            channel: _channel,
          );
      if (!mounted) return;
      setState(() => _otpStep = true);
    } on ApiException catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(e.message)));
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  Future<void> _verify(String code) async {
    final phone = _phone.text.replaceAll(RegExp(r'\D'), '');
    setState(() => _loading = true);
    try {
      final client = await ref.read(profileRepositoryProvider).changePhoneVerify(
            countryCode: _countryCode,
            phone: phone,
            code: code,
          );
      await ref.read(sessionProvider.notifier).updateClient(client);
      if (!mounted) return;
      context.pop();
    } on ApiException catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(e.message)));
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final fullPhone = '$_countryCode${_phone.text.replaceAll(RegExp(r'\D'), '')}';

    return Scaffold(
      backgroundColor: AppColors.background,
      appBar: ScreenAppBar(title: l10n.phoneNumber),
      body: Padding(
        padding: const EdgeInsets.all(20),
        child: Container(
          padding: const EdgeInsets.all(20),
          decoration: BoxDecoration(
            color: Colors.white,
            borderRadius: BorderRadius.circular(20),
          ),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Icon(Icons.chat, size: 24),
              const SizedBox(height: 12),
              Text(
                l10n.enterYourPhoneNumber,
                style: const TextStyle(fontSize: 24, fontWeight: FontWeight.w800),
              ),
              const SizedBox(height: 20),
              PhoneFields(
                countryCode: _countryCode,
                phoneController: _phone,
                prefixLabel: l10n.prefix,
                phoneLabel: l10n.phoneNumber,
                onPrefixTap: () async {
                  final code = await context.push<String>('/country');
                  if (code != null) setState(() => _countryCode = code);
                },
              ),
              const SizedBox(height: 16),
              ChannelButtons(
                selected: _channel,
                smsLabel: l10n.sms,
                whatsappLabel: l10n.whatsapp,
                onSelect: (v) {
                  setState(() => _channel = v);
                  _request();
                },
              ),
              if (_otpStep) ...[
                const SizedBox(height: 20),
                Text(
                  _channel == 'sms'
                      ? l10n.insertCodeSms(fullPhone)
                      : l10n.insertCodeWhatsapp(fullPhone),
                  style: const TextStyle(color: AppColors.muted),
                ),
                const SizedBox(height: 16),
                OtpInput(onCompleted: _verify),
              ],
              if (_loading) ...[
                const SizedBox(height: 16),
                const Center(child: CircularProgressIndicator()),
              ],
            ],
          ),
        ),
      ),
    );
  }
}

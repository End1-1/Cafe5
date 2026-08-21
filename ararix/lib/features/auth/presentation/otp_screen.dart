import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../../core/network/api_exception.dart';
import '../../../core/theme/app_theme.dart';
import '../../../core/widgets/otp_input.dart';
import '../../../core/widgets/settings_tile.dart';
import '../../../l10n/app_localizations.dart';
import '../application/session_notifier.dart';
import '../data/auth_repository.dart';

class OtpScreen extends ConsumerStatefulWidget {
  const OtpScreen({
    super.key,
    required this.countryCode,
    required this.phone,
    required this.channel,
  });

  final String countryCode;
  final String phone;
  final String channel;

  @override
  ConsumerState<OtpScreen> createState() => _OtpScreenState();
}

class _OtpScreenState extends ConsumerState<OtpScreen> {
  int _seconds = 60;
  Timer? _timer;
  bool _loading = false;

  @override
  void initState() {
    super.initState();
    _startTimer();
  }

  @override
  void dispose() {
    _timer?.cancel();
    super.dispose();
  }

  void _startTimer() {
    _timer?.cancel();
    setState(() => _seconds = 60);
    _timer = Timer.periodic(const Duration(seconds: 1), (t) {
      if (_seconds <= 1) {
        t.cancel();
        setState(() => _seconds = 0);
      } else {
        setState(() => _seconds -= 1);
      }
    });
  }

  String get _fullPhone => '${widget.countryCode}${widget.phone}';

  Future<void> _verify(String code) async {
    final l10n = AppLocalizations.of(context);
    if (code.length != 4) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(l10n.invalidOtp)),
      );
      return;
    }
    setState(() => _loading = true);
    try {
      final result = await ref.read(authRepositoryProvider).verifyOtp(
            countryCode: widget.countryCode,
            phone: widget.phone,
            code: code,
          );
      await ref.read(sessionProvider.notifier).setAuthenticated(
            token: result.token,
            client: result.client,
          );
      if (!mounted) return;
      context.go('/home');
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

  Future<void> _resend() async {
    if (_seconds > 0) return;
    try {
      await ref.read(authRepositoryProvider).resendOtp(
            countryCode: widget.countryCode,
            phone: widget.phone,
            channel: widget.channel,
          );
      if (!mounted) return;
      _startTimer();
    } on ApiException catch (e) {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(e.message)));
    }
  }

  @override
  Widget build(BuildContext context) {
    final l10n = AppLocalizations.of(context);
    final mm = (_seconds ~/ 60).toString().padLeft(2, '0');
    final ss = (_seconds % 60).toString().padLeft(2, '0');
    final hint = widget.channel == 'sms'
        ? l10n.insertCodeSms(_fullPhone)
        : l10n.insertCodeWhatsapp(_fullPhone);

    return Scaffold(
      backgroundColor: Colors.white,
      appBar: ScreenAppBar(title: l10n.phoneValidation),
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Icon(Icons.chat, size: 28),
            const SizedBox(height: 16),
            Text(
              l10n.enterTheCode,
              style: const TextStyle(fontSize: 28, fontWeight: FontWeight.w800),
            ),
            const SizedBox(height: 8),
            Text(hint, style: const TextStyle(color: AppColors.muted, height: 1.4)),
            const SizedBox(height: 28),
            OtpInput(onCompleted: _verify),
            const SizedBox(height: 20),
            if (_seconds > 0)
              Text(
                l10n.resendIn('$mm:$ss'),
                style: const TextStyle(color: AppColors.muted),
              )
            else
              TextButton(
                onPressed: _resend,
                child: Text(l10n.resendNow),
              ),
            if (_loading) ...[
              const SizedBox(height: 20),
              const Center(child: CircularProgressIndicator()),
            ],
          ],
        ),
      ),
    );
  }
}

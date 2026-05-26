import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/bloc/app_cubits.dart';
import 'package:picassowaiter/model/model.dart';
import 'package:picassowaiter/screen/dashboard.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/styles.dart';
import 'package:picassowaiter/utils/waiter_auth.dart';

/// Stage 2 — waiter PIN ([DlgScreen]). Stage 1 already done in [InitAppBloc].
class WMLogin extends StatefulWidget {
  final WMModel model;

  const WMLogin(this.model, {super.key});

  @override
  State<WMLogin> createState() => _WMLoginState();
}

class _WMLoginState extends State<WMLogin> {
  String _pin = '';

  void _appendDigit(String d) {
    setState(() => _pin += d);
  }

  void _clearPin() {
    setState(() => _pin = '');
  }

  void _backspace() {
    if (_pin.isEmpty) return;
    setState(() => _pin = _pin.substring(0, _pin.length - 1));
  }

  Future<void> _submitPin() async {
    final pin = _pin;
    _clearPin();
    if (pin.isEmpty) return;

    if (!WaiterAuth.isProgramReady) {
      BlocProvider.of<AppBloc>(context).add(
        AppEventError('Program session is not ready. Retry from settings.'),
      );
      return;
    }

    context.read<AppLoadingCubit>().change(AppLoadingState.loading);

    final pinErr = await WaiterAuth.pinLogin(pin);
    if (!mounted) return;
    if (pinErr != null && pinErr.isNotEmpty) {
      context.read<AppLoadingCubit>().change(AppLoadingState.idle);
      BlocProvider.of<AppBloc>(context).add(AppEventError(pinErr));
      return;
    }

    final cashErr = await WaiterAuth.finishWaiterLogin();
    if (!mounted) return;

    context.read<AppLoadingCubit>().change(AppLoadingState.idle);

    if (cashErr != null && cashErr.isNotEmpty) {
      WaiterAuth.clearWaiterSession();
      BlocProvider.of<AppBloc>(context).add(AppEventError(cashErr));
      return;
    }

    if (prefs.getBool('stayloggedin') ?? false) {
      // waiter_token already saved in applyWaiterLoginPayload
    }

    Navigator.pushAndRemoveUntil(
      context,
      MaterialPageRoute(builder: (_) => WMDashboard(model: widget.model)),
      (_) => false,
    );
  }

  @override
  Widget build(BuildContext context) {
    return BlocBuilder<AppBloc, AppState>(
      builder: (context, state) {
        return Container(
          constraints: const BoxConstraints(maxWidth: 420),
          padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                'PICASSO WAITER',
                style: TextStyle(
                  fontSize: 22,
                  fontWeight: FontWeight.bold,
                  color: Styling.appBarBackgroundColor,
                ),
              ),
              Styling.columnSpacingWidget(),
              Text(
                widget.model.tr('Waiter PIN'),
                style: const TextStyle(fontSize: 14, color: Colors.black54),
              ),
              Styling.columnSpacingWidget(),
              Text(
                'v${prefs.string('appversion')} · PIN login',
                style: const TextStyle(color: Colors.black45, fontSize: 11),
              ),
              Styling.columnSpacingWidget(),
              _pinDots(),
              Styling.columnSpacingWidget(),
              _pinPad(),
              Styling.columnSpacingWidget(),
              Row(
                children: [
                  WMCheckbox(widget.model.tr('Stay in'), (b) {
                    prefs.setBool('stayloggedin', b ?? false);
                  }, prefs.getBool('stayloggedin') ?? false),
                  const Spacer(),
                  Styling.textButton(
                    () => widget.model.navigation
                        .openConnectionSettings(context),
                    widget.model.tr('Connection settings'),
                  ),
                ],
              ),
              if (state is AppStateError) ...[
                Styling.columnSpacingWidget(),
                Styling.textError(state.text),
              ],
            ],
          ),
        );
      },
    );
  }

  Widget _pinDots() {
    return SizedBox(
      height: 40,
      child: Center(
        child: Text(
          List.filled(_pin.length, '●').join(),
          style: const TextStyle(fontSize: 28, letterSpacing: 6),
        ),
      ),
    );
  }

  Widget _pinPad() {
    Widget key(String label, VoidCallback onTap, {Color? color}) {
      return Expanded(
        child: Padding(
          padding: const EdgeInsets.all(4),
          child: Material(
            color: color ?? const Color(0xffe8f4f6),
            borderRadius: BorderRadius.circular(8),
            child: InkWell(
              onTap: onTap,
              borderRadius: BorderRadius.circular(8),
              child: SizedBox(
                height: 56,
                child: Center(
                  child: Text(
                    label,
                    style: TextStyle(
                      fontSize: label.length > 1 ? 16 : 24,
                      fontWeight: FontWeight.w600,
                      color: color != null ? Colors.white : Colors.black87,
                    ),
                  ),
                ),
              ),
            ),
          ),
        ),
      );
    }

    Widget row(List<Widget> keys) => Row(children: keys);

    return Column(
      children: [
        row([
          key('1', () => _appendDigit('1')),
          key('2', () => _appendDigit('2')),
          key('3', () => _appendDigit('3')),
        ]),
        row([
          key('4', () => _appendDigit('4')),
          key('5', () => _appendDigit('5')),
          key('6', () => _appendDigit('6')),
        ]),
        row([
          key('7', () => _appendDigit('7')),
          key('8', () => _appendDigit('8')),
          key('9', () => _appendDigit('9')),
        ]),
        row([
          key('C', _clearPin, color: Colors.orange.shade700),
          key('0', () => _appendDigit('0')),
          key('⌫', _backspace, color: Colors.grey.shade600),
        ]),
        Styling.columnSpacingWidget(),
        row([
          key(widget.model.tr('Enter'), _submitPin,
              color: Styling.appBarBackgroundColor),
        ]),
      ],
    );
  }
}

import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/model/model.dart';
import 'package:picassowaiter/screen/login.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/styles.dart';
import 'package:picassowaiter/utils/waiter_auth.dart';

/// Stage 1 in background (program session), stage 2 = [WMLogin] PIN only.
class AppStartup extends StatefulWidget {
  final WMModel model;

  const AppStartup({super.key, required this.model});

  @override
  State<AppStartup> createState() => _AppStartupState();
}

class _AppStartupState extends State<AppStartup> {
  bool _restoreStarted = false;

  @override
  Widget build(BuildContext context) {
    return BlocListener<InitAppBloc, InitAppState>(
      listener: (context, state) {
        if (_restoreStarted) return;
        if (state is! InitAppStateFinished) return;
        if (!state.programReady) return;
        if (!(prefs.getBool('stayloggedin') ?? false)) return;
        if (prefs.string('waiter_token').isEmpty) return;

        _restoreStarted = true;
        widget.model.loginSessionRestore();
      },
      child: BlocBuilder<InitAppBloc, InitAppState>(
        builder: (context, state) {
          if (!WaiterAuth.isConnectionConfigured()) {
            return _NeedConnectionSettings(model: widget.model);
          }

          if (state is! InitAppStateFinished) {
            return _StartupLoading(
              status: widget.model.tr('Prepare to work'),
            );
          }

          if (state.error) {
            return _StartupError(
              model: widget.model,
              context: context,
              title: widget.model.tr('Connection'),
              message: state.errorText,
              onRetry: () => context.read<InitAppBloc>().add(InitAppEvent()),
            );
          }
          if (!state.programReady) {
            return _StartupError(
              model: widget.model,
              context: context,
              title: widget.model.tr('Program login'),
              message: state.programError.isNotEmpty
                  ? state.programError
                  : widget.model.tr('Check service login in Configuration'),
              onRetry: () => context.read<InitAppBloc>().add(InitAppEvent()),
              openSettings: true,
            );
          }

          return WMLogin(widget.model);
        },
      ),
    );
  }
}

/// No login/password here — only link to Configuration.
class _NeedConnectionSettings extends StatelessWidget {
  final WMModel model;

  const _NeedConnectionSettings({required this.model});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(24),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          const Text(
            'PICASSO WAITER',
            textAlign: TextAlign.center,
            style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
          ),
          Styling.columnSpacingWidget(),
          Text(
            model.tr(
              'Set server address and service login in Connection settings. '
              'Then sign in with waiter PIN.',
            ),
            textAlign: TextAlign.center,
          ),
          Styling.columnSpacingWidget(),
          Styling.textButton(
            () => model.navigation.openConnectionSettings(context),
            model.tr('Connection settings'),
          ),
        ],
      ),
    );
  }
}

class _StartupLoading extends StatelessWidget {
  final String status;

  const _StartupLoading({required this.status});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          const SizedBox(
            width: 36,
            height: 36,
            child: CircularProgressIndicator(strokeWidth: 2.5),
          ),
          Styling.columnSpacingWidget(),
          Text(status, style: const TextStyle(fontSize: 16)),
          const SizedBox(height: 8),
          Text(
            prefs.string('appversion'),
            style: const TextStyle(color: Colors.black45, fontSize: 12),
          ),
        ],
      ),
    );
  }
}

class _StartupError extends StatelessWidget {
  final WMModel model;
  final BuildContext context;
  final String title;
  final String message;
  final VoidCallback onRetry;
  final bool openSettings;

  const _StartupError({
    required this.model,
    required this.context,
    required this.title,
    required this.message,
    required this.onRetry,
    this.openSettings = false,
  });

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(16),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Text(
            title,
            style: const TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
          ),
          Styling.columnSpacingWidget(),
          Styling.textError(message),
          Styling.columnSpacingWidget(),
          Styling.textButton(onRetry, model.tr('Retry')),
          if (openSettings)
            Styling.textButton(
              () => model.navigation.openConnectionSettings(context),
              model.tr('Connection settings'),
            ),
        ],
      ),
    );
  }
}

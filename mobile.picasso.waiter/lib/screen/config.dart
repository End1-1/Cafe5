import 'package:flutter/material.dart';
import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/bloc/app_cubits.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/styles.dart';
import 'package:picassowaiter/utils/waiter_auth.dart';
import 'package:flutter_bloc/flutter_bloc.dart';

import 'app.dart';

/// Connection settings only — not waiter sign-in. Same as PC connection dialog.
class WMConfig extends WMApp {
  WMConfig({super.key, required super.model});

  @override
  String titleText() => model.tr('Connection settings');

  @override
  Widget? leadingButton(BuildContext context) {
    return IconButton(
      icon: const Icon(Icons.arrow_back),
      onPressed: () => Navigator.pop(context),
    );
  }

  @override
  List<Widget> actions() => [];

  @override
  Widget body() {
    return SingleChildScrollView(
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Styling.columnSpacingWidget(),
          Text(
            model.tr(
              'Service account: used by the app in the background '
              '(settings, workstation). Waiter signs in with PIN on the next screen.',
            ),
            style: const TextStyle(fontSize: 13, color: Colors.black54),
          ),
          Styling.columnSpacingWidget(),
          Styling.textFormField(
            model.serverTextController,
            model.tr('Server address'),
          ),
          Styling.columnSpacingWidget(),
          Styling.textFormField(
            model.serverUserTextController,
            model.tr('Login'),
          ),
          Styling.columnSpacingWidget(),
          Styling.textFormFieldPassword(
            model.serverPasswordTextController,
            model.tr('Password'),
          ),
          Styling.columnSpacingWidget(),
          const Row(children: [Expanded(child: _HttpRadioGroup())]),
          Styling.columnSpacingWidget(),
          Center(
            child: Styling.textButton(_saveAndClose, model.tr('Save')),
          ),
          Center(
            child: Styling.textButton(
              model.registerOnServer,
              model.tr('Test login on server'),
            ),
          ),
          Row(
            children: [
              Expanded(child: Styling.textCenter(prefs.string('appversion'))),
            ],
          ),
        ],
      ),
    );
  }

  Future<void> _saveAndClose() async {
    model.saveServerConfig();
    if (!WaiterAuth.isConnectionConfigured()) {
      BlocProvider.of<AppBloc>(prefs.context()).add(
        AppEventError(model.tr('Fill server, login and password')),
      );
      return;
    }

    prefs.context().read<AppLoadingCubit>().change(AppLoadingState.loading);
    final err = await WaiterAuth.startupProgramSession();
    prefs.context().read<AppLoadingCubit>().change(AppLoadingState.idle);

    if (err != null && err.isNotEmpty) {
      BlocProvider.of<AppBloc>(prefs.context()).add(AppEventError(err));
      return;
    }
    if (prefs.context().mounted) {
      Navigator.pop(prefs.context(), true);
    }
  }
}

class _HttpRadioGroup extends StatefulWidget {
  const _HttpRadioGroup();

  @override
  State<_HttpRadioGroup> createState() => _HttpRadioGroupState();
}

class _HttpRadioGroupState extends State<_HttpRadioGroup> {
  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          child: RadioListTile<bool>(
            title: const Text('http'),
            value: false,
            groupValue: prefs.getBool('https') ?? false,
            onChanged: (value) {
              setState(() {});
              prefs.setBool('https', false);
            },
          ),
        ),
        Expanded(
          child: RadioListTile<bool>(
            title: const Text('https'),
            value: true,
            groupValue: prefs.getBool('https') ?? false,
            onChanged: (value) {
              setState(() {});
              prefs.setBool('https', true);
            },
          ),
        ),
      ],
    );
  }
}

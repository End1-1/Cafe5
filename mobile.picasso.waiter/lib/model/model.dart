import 'package:flutter/foundation.dart';
import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/bloc/app_cubits.dart';
import 'package:picassowaiter/bloc/question_bloc.dart';
import 'package:picassowaiter/screen/dashboard.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/res.dart';
import 'package:picassowaiter/utils/waiter_auth.dart';
import 'package:picassowaiter/utils/waiter_init_v2.dart';
import 'package:picassowaiter/utils/waiter_order_v2.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:url_launcher/url_launcher.dart';

import 'navigation.dart';

part 'model.menu.dart';

class AppStateAppBar extends AppState {
  AppStateAppBar():super(0);
}

class AppEventAppBar extends AppEvent {}

class WMModel {
  final serverTextController = TextEditingController();
  final serverUserTextController = TextEditingController();
  final serverPasswordTextController = TextEditingController();
  final configPinTextController = TextEditingController();

  late final Navigation navigation;

  WMModel() {
    navigation = Navigation(this);
    serverTextController.text = prefs.string('serveraddress');
    serverUserTextController.text = prefs.string('server_user');
    serverPasswordTextController.text = prefs.string('server_password');
  }

  String tr(String s) {
    return Res.tr[s] ?? s;
  }

  void saveServerConfig() {
    prefs.setString('serveraddress', serverTextController.text.trim());
    WaiterAuth.saveProgramCredentials(
      serverUserTextController.text.trim(),
      serverPasswordTextController.text,
    );
  }

  void registerOnServer() {
    saveServerConfig();
    BlocProvider.of<AppBloc>(Prefs.navigatorKey.currentContext!)
        .add(AppEventLoading(tr('Registering on server'), WaiterAuth.routeProgramLogin, {
      'nootp': true,
      'username': serverUserTextController.text,
      'password': serverPasswordTextController.text,
    }, (e, d) async {
      if (!e) {
        serverPasswordTextController.clear();
        final err = await WaiterAuth.startupProgramSession();
        if (!prefs.context().mounted) return;
        if (err != null) {
          BlocProvider.of<AppBloc>(prefs.context()).add(AppEventError(err));
          return;
        }
        Navigator.pop(prefs.context(), true);
      }
    }, AppStateFinished(data: null)));
  }

  Future<void> loginSessionRestore() async {
    final ctx = prefs.context();
    ctx.read<AppLoadingCubit>().change(AppLoadingState.loading);

    final err = await WaiterAuth.restoreWaiterSession();
    if (err != null) {
      ctx.read<AppLoadingCubit>().change(AppLoadingState.idle);
      WaiterAuth.clearWaiterSession();
      BlocProvider.of<AppBloc>(ctx).add(AppEventError(err));
      return;
    }

    final cashErr = await WaiterAuth.finishWaiterLogin();
    ctx.read<AppLoadingCubit>().change(AppLoadingState.idle);
    if (cashErr != null) {
      prefs.setBool('stayloggedin', false);
      BlocProvider.of<AppBloc>(ctx).add(AppEventError(cashErr));
      return;
    }
    if (!ctx.mounted) return;
    Navigator.pushAndRemoveUntil(
      ctx,
      MaterialPageRoute(builder: (_) => WMDashboard(model: this)),
      (_) => false,
    );
  }

  void closeDialog() {
    BlocProvider.of<AppBloc>(Prefs.navigatorKey.currentContext!)
        .add(AppEvent());
  }

  void closeQuestionDialog() {
    BlocProvider.of<QuestionBloc>(Prefs.navigatorKey.currentContext!)
        .add(QuestionEvent());
  }

  void menuRaise() {
    BlocProvider.of<AppAnimateBloc>(prefs.context())
        .add(AppAnimateEventRaise());
  }

  void downloadLatestVersion() async {
    launchUrl(
        Uri.parse('https://download.picasso.am/'),
        mode: LaunchMode.inAppBrowserView);
  }
}

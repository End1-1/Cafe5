import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/bloc/question_bloc.dart';
import 'package:picassowaiter/main.dart';
import 'package:picassowaiter/model/model.dart';
import 'package:picassowaiter/screen/config.dart';
import 'package:picassowaiter/screen/goods_info.dart';
import 'package:picassowaiter/screen/order.dart';
import 'package:picassowaiter/utils/barcode.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/waiter_auth.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';

class Navigation {
  final WMModel model;

  Navigation(this.model);

  /// Connection: server + service login/password (program account).
  Future<void> openConnectionSettings(BuildContext context) {
    model.serverTextController.text = prefs.string('serveraddress');
    model.serverUserTextController.text = prefs.string('server_user');
    model.serverPasswordTextController.text = prefs.string('server_password');
    return Navigator.push(
      context,
      MaterialPageRoute(builder: (_) => WMConfig(model: model)),
    ).then((_) {
      if (context.mounted) {
        context.read<InitAppBloc>().add(InitAppEvent());
      }
    });
  }

  Future<void> config(BuildContext context) => openConnectionSettings(context);

  Future<void> settings(BuildContext context) {
    hideMenu(context);
    return openConnectionSettings(context);
  }

  void logout() {
    final ctx = prefs.maybeContext;
    if (ctx == null) return;
    hideMenu(ctx);
    BlocProvider.of<QuestionBloc>(ctx).add(QuestionEventRaise(model.tr('Logout?'), (){
      BlocProvider.of<QuestionBloc>(Prefs.navigatorKey.currentContext!)
          .add(QuestionEvent());
      BlocProvider.of<AppBloc>(prefs.context()).add(AppEventLoading(model.tr('Logout'), 'engine/logout.php', {}, (e, d) {
          WaiterAuth.clearWaiterSession();
          prefs.setString('sessionkey', '');
          Navigator.pushAndRemoveUntil(prefs.context(), MaterialPageRoute(builder: (builder) =>  App()), (route) => false);
      }, AppStateFinished(data: null)));
    }, null));

  }

  void hideMenu(BuildContext context) {
    context.read<AppAnimateBloc>().add(AppAnimateEvent());
  }

  Future<String?> readBarcode() async {
    return Navigator.push(prefs.context(), MaterialPageRoute(builder: (builder) => MLKitBarcodeScanner()));
  }

  Future<Object?> goodsInfo(Map<String,dynamic> info) async {
    return Navigator.push(prefs.context(), MaterialPageRoute(builder: (builder) => WMGoodsInfo(info, model: model)));
  }




  Future<Map<String, dynamic>?> openWaiterTable(int table) {
    return Navigator.push<Map<String, dynamic>?>(
      prefs.context(),
      MaterialPageRoute(
        builder: (builder) => WMOrder(model: model, table: table),
      ),
    );
  }
}

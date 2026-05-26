import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/bloc/app_cubits.dart';
import 'package:picassowaiter/bloc/question_bloc.dart';
import 'package:picassowaiter/model/model.dart';
import 'package:picassowaiter/utils/calendar.dart';
import 'package:picassowaiter/utils/http_query.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/styles.dart';
import 'package:picassowaiter/utils/waiter_init_v2.dart';

import 'app.dart';

part 'dashboard.model.dart';
part 'dashboard.waiter.dart';

class WMDashboard extends WMApp {
  final _model = DashboardModel();

  WMDashboard({super.key, required super.model}) {
    getDashboard();
  }

  @override
  Widget? leadingButton(BuildContext context) {
    return null;
  }

  @override
  String titleText() {
    return Prefs.config['first_page_title'] ?? 'Picasso';
  }

  @override
  List<Widget> actions() {
    return [
      IconButton(onPressed: getDashboard, icon: const Icon(Icons.refresh)),
      IconButton(onPressed: model.menuRaise, icon: const Icon(Icons.menu))
    ];
  }

  @override
  List<Widget> menuWidgets() {
    return [
      Styling.menuButton(() {
        final ctx = prefs.maybeContext;
        if (ctx != null) {
          model.navigation.openConnectionSettings(ctx);
        }
      }, 'config', model.tr('Configuration')),
      Styling.menuButton(model.navigation.logout, 'logout', model.tr('Logout')),
    ];
  }

  @override
  Widget body() {
    return bodyWaiter();
  }
}

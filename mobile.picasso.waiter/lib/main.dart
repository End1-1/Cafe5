import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:package_info_plus/package_info_plus.dart';
import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/bloc/app_cubits.dart';
import 'package:picassowaiter/bloc/question_bloc.dart';
import 'package:picassowaiter/screen/app_startup.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter_localizations/flutter_localizations.dart';

import 'bloc/date_bloc.dart' show DateBloc;
import 'l10n/app_localizations.dart';
import 'model/model.dart';

/// Bump when auth flow changes — clears old single-step login session once.
const int kAuthFlowVersion = 2;

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  prefs = await SharedPreferences.getInstance();

  if ((prefs.getInt('auth_flow_version') ?? 0) < kAuthFlowVersion) {
    prefs.setString('waiter_token', '');
    prefs.setString('program_token', '');
    prefs.setString('token', '');
    prefs.setString('sessionkey', '');
    prefs.setBool('stayloggedin', false);
    prefs.setBool('program_ready', false);
    prefs.setInt('auth_flow_version', kAuthFlowVersion);
  }

  PackageInfo pa = await PackageInfo.fromPlatform();
  prefs.setString('appversion', '${pa.version}.${pa.buildNumber}');
  prefs.setString('appbuild', pa.buildNumber);
  prefs.init();

  runApp(MultiBlocProvider(
    providers: [
      BlocProvider<AppBloc>(create: (context) => AppBloc()),
      BlocProvider<InitAppBloc>(
        create: (context) => InitAppBloc()..add(InitAppEvent()),
      ),
      BlocProvider<AppAnimateBloc>(create: (context) => AppAnimateBloc()),
      BlocProvider<DateBloc>(create: (context) => DateBloc()),
      BlocProvider<QuestionBloc>(create: (context) => QuestionBloc()),
      BlocProvider<AppLoadingCubit>(create: (_) => AppLoadingCubit()),
      BlocProvider<AppCubits>(create: (_) => AppCubits()),
    ],
    child: const PicassoWaiterApp(),
  ));
}

class PicassoWaiterApp extends StatelessWidget {
  const PicassoWaiterApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Picasso Waiter',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      locale: const Locale('hy'),
      localizationsDelegates: const [
        AppLocalizations.delegate,
        GlobalMaterialLocalizations.delegate,
        GlobalWidgetsLocalizations.delegate,
        GlobalCupertinoLocalizations.delegate,
      ],
      supportedLocales: const [Locale('en'), Locale('hy'), Locale('ru')],
      navigatorKey: Prefs.navigatorKey,
      home: AppRoot(),
    );
  }
}

/// Kept name [App] for logout navigation (`MaterialPageRoute(builder: (_) => App())`).
class App extends StatelessWidget {
  const App({super.key});

  @override
  Widget build(BuildContext context) => const AppRoot();
}

class AppRoot extends StatefulWidget {
  const AppRoot({super.key});

  @override
  State<AppRoot> createState() => _AppRootState();
}

class _AppRootState extends State<AppRoot> {
  final model = WMModel();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(child: AppStartup(model: model)),
    );
  }
}

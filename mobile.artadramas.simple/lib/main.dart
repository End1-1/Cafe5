import 'package:flutter/material.dart';

import 'app_model.dart';
import 'home_page.dart';
import 'i18n.dart';
import 'pin_page.dart';
import 'settings_page.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await appModel.load();
  runApp(const ArtadramasApp());
}

class ArtadramasApp extends StatelessWidget {
  const ArtadramasApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: appModel,
      builder: (context, _) {
        return MaterialApp(
          title: S.t(appModel.lang, 'title'),
          debugShowCheckedModeBanner: false,
          theme: ThemeData(
            colorScheme: ColorScheme.fromSeed(seedColor: const Color(0xFF1F4E3D)),
            useMaterial3: true,
            textTheme: const TextTheme(bodyLarge: TextStyle(fontSize: 18)),
          ),
          home: const Gate(),
        );
      },
    );
  }
}

class Gate extends StatelessWidget {
  const Gate({super.key});

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: appModel,
      builder: (context, _) {
        if (!appModel.configured) {
          return const SettingsPage(requiredSetup: true);
        }
        if (!appModel.signedIn) {
          return const PinPage();
        }
        if (!appModel.storesReady) {
          return const SettingsPage(requiredSetup: true);
        }
        return const HomePage();
      },
    );
  }
}

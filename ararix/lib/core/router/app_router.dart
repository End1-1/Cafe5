import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';

import '../../features/auth/application/session_notifier.dart';
import '../../features/auth/presentation/country_screen.dart';
import '../../features/auth/presentation/otp_screen.dart';
import '../../features/auth/presentation/welcome_screen.dart';
import '../../features/faq/presentation/faq_screen.dart';
import '../../features/home/presentation/home_screen.dart';
import '../../features/language/presentation/language_screen.dart';
import '../../features/order/presentation/cart_screen.dart';
import '../../features/order/presentation/dish_detail_screen.dart';
import '../../features/order/presentation/package_pick_screen.dart';
import '../../features/order/presentation/restaurant_screen.dart';
import '../../features/payments/presentation/payments_screen.dart';
import '../../features/profile/presentation/account_screen.dart';
import '../../features/profile/presentation/change_phone_screen.dart';
import '../../features/profile/presentation/profile_hub_screen.dart';
import '../../features/shell/presentation/main_shell.dart';
import '../../l10n/app_localizations.dart';

final appRouterProvider = Provider<GoRouter>((ref) {
  final refresh = ValueNotifier<int>(0);
  ref.listen(sessionProvider, (previous, next) {
    refresh.value++;
  });
  ref.onDispose(refresh.dispose);

  return GoRouter(
    initialLocation: '/welcome',
    refreshListenable: refresh,
    redirect: (context, state) {
      final session = ref.read(sessionProvider);
      if (!session.ready) return null;
      final loggingIn = state.matchedLocation == '/welcome' ||
          state.matchedLocation == '/country' ||
          state.matchedLocation == '/otp';
      if (!session.isAuthenticated && !loggingIn) {
        return '/welcome';
      }
      if (session.isAuthenticated && loggingIn) {
        return '/home';
      }
      return null;
    },
    routes: [
      GoRoute(
        path: '/welcome',
        builder: (context, state) => const WelcomeScreen(),
      ),
      GoRoute(
        path: '/country',
        builder: (context, state) => const CountryScreen(),
      ),
      GoRoute(
        path: '/otp',
        builder: (context, state) {
          final extra = state.extra as Map<String, dynamic>? ?? {};
          return OtpScreen(
            countryCode: extra['country_code'] as String? ?? '+374',
            phone: extra['phone'] as String? ?? '',
            channel: extra['channel'] as String? ?? 'whatsapp',
          );
        },
      ),
      GoRoute(
        path: '/restaurant/:id',
        builder: (context, state) {
          final id = int.tryParse(state.pathParameters['id'] ?? '') ?? 0;
          return RestaurantScreen(restaurantId: id);
        },
        routes: [
          GoRoute(
            path: 'dish/:dishId',
            builder: (context, state) {
              final id = int.tryParse(state.pathParameters['id'] ?? '') ?? 0;
              final dishId =
                  int.tryParse(state.pathParameters['dishId'] ?? '') ?? 0;
              return DishDetailScreen(restaurantId: id, dishId: dishId);
            },
          ),
          GoRoute(
            path: 'package/:dishId',
            builder: (context, state) {
              final id = int.tryParse(state.pathParameters['id'] ?? '') ?? 0;
              final dishId =
                  int.tryParse(state.pathParameters['dishId'] ?? '') ?? 0;
              return PackagePickScreen(restaurantId: id, dishId: dishId);
            },
          ),
        ],
      ),
      GoRoute(
        path: '/cart',
        builder: (context, state) => const CartScreen(),
      ),
      StatefulShellRoute.indexedStack(
        builder: (context, state, navigationShell) {
          return MainShell(navigationShell: navigationShell);
        },
        branches: [
          StatefulShellBranch(
            routes: [
              GoRoute(
                path: '/home',
                builder: (context, state) => const HomeScreen(),
              ),
            ],
          ),
          StatefulShellBranch(
            routes: [
              GoRoute(
                path: '/menu',
                builder: (context, state) {
                  final l10n = AppLocalizations.of(context);
                  return PlaceholderTab(title: l10n.menu);
                },
              ),
            ],
          ),
          StatefulShellBranch(
            routes: [
              GoRoute(
                path: '/orders',
                builder: (context, state) {
                  final l10n = AppLocalizations.of(context);
                  return PlaceholderTab(title: l10n.orders);
                },
              ),
            ],
          ),
          StatefulShellBranch(
            routes: [
              GoRoute(
                path: '/profile',
                builder: (context, state) => const ProfileHubScreen(),
                routes: [
                  GoRoute(
                    path: 'account',
                    builder: (context, state) => const AccountScreen(),
                  ),
                  GoRoute(
                    path: 'change-phone',
                    builder: (context, state) => const ChangePhoneScreen(),
                  ),
                  GoRoute(
                    path: 'payments',
                    builder: (context, state) => const PaymentsScreen(),
                  ),
                  GoRoute(
                    path: 'faq',
                    builder: (context, state) => const FaqScreen(),
                  ),
                  GoRoute(
                    path: 'language',
                    builder: (context, state) => const LanguageScreen(),
                  ),
                ],
              ),
            ],
          ),
        ],
      ),
    ],
  );
});

import 'dart:convert';

import 'package:picassowaiter/utils/http_query.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/res.dart';
import 'package:picassowaiter/utils/waiter_auth.dart';
import 'package:equatable/equatable.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter_bloc/flutter_bloc.dart';

import 'app_cubits.dart';

part 'event_bloc.dart';
part 'state_bloc.dart';

class AppBloc extends Bloc<AppEvent, AppState> {
  AppBloc() : super(AppState(0)) {
    on<AppEvent>((event, emit) => emit(AppState(0)));
    on<AppEventLoading>((event, emit) => loadingData(event));
    on<AppEventError>((event, emit) => emit(AppStateError(event.text)));
    on<AppEventLoading2>((event, emit)=>_loadingData(event));
  }

  void loadingData(AppEventLoading event) async {
    if (event.route.isEmpty) {
      emit(event.state!);
      return;
    }
    prefs.context().read<AppLoadingCubit>().change(AppLoadingState.loading);
    final result = await HttpQuery(event.route).request(event.data);
    prefs.context().read<AppLoadingCubit>().change(AppLoadingState.idle);
    if (result['status'] == 0 && event.route != 'engine/logout.php') {
      emit(AppStateError(result['data']));
      if (event.callback != null) {
        event.callback!(true, result['data'] ?? result);
      }
      return;
    }
    if (event.state != null) {
      emit(event.state!..data = result['data'] ?? result);
    }
    if (event.callback != null) {
      event.callback!(result['status'] == 0, result['data'] ?? result);
    }
  }

  void _loadingData(AppEventLoading2 event) async {
    prefs.context().read<AppLoadingCubit>().change(AppLoadingState.loading);
    final result = await HttpQuery(event.route).request(event.data);
    prefs.context().read<AppLoadingCubit>().change(AppLoadingState.idle);
    if (result['status'] != 1) {
      emit(AppStateError(result['data']));
      return;
    }
      emit(event.state!..id = event.id ..data = result['data'] ?? result);
  }
}

class InitAppBloc extends Bloc<InitAppEvent, InitAppState> {
  InitAppBloc() : super(InitAppState()) {
    on<InitAppEvent>((event, emit) => configureClient(event));
  }

  void configureClient(InitAppEvent event) async {
    if (kIsWeb) {
      prefs.setString('serveraddress', Uri.base.host);
      final result = await HttpQuery('engine/client-config.php')
          .request({'res_version': prefs.getInt('res_version') ?? 0});
      initRes(result);
      String? programError;
      if (result['status'] == 0) {
        programError = result['data']?.toString() ?? 'Client config failed';
      } else {
        programError = await WaiterAuth.startupProgramSession();
      }
      emit(InitAppStateFinished(
        result['status'] == 0,
        result['status'] == 0 ? '${result['data']}' : '',
        result['data'],
        programReady: result['status'] != 0 && programError == null,
        programError: programError ?? '',
      ));
      return;
    }
    if (!WaiterAuth.isConnectionConfigured()) {
      emit(InitAppStateFinished(
        false,
        '',
        null,
        programReady: false,
        programError: '',
      ));
      return;
    }
    emit(InitAppStateLoading());
    final result = await HttpQuery('engine/client-config.php')
        .request({'res_version': prefs.getInt('res_version') ?? 0});
    initRes(result);

    String? programError;
    if (result['status'] == 0) {
      programError = result['data']?.toString() ?? 'Client config failed';
    } else {
      programError = await WaiterAuth.startupProgramSession();
    }

    emit(InitAppStateFinished(
      result['status'] == 0,
      result['status'] == 0 ? '${result['data']}' : '',
      result['data'],
      programReady: result['status'] != 0 && programError == null,
      programError: programError ?? '',
    ));
  }

  void initRes(dynamic result) {
    if (result['status'] == 1) {
      dynamic d = result['data'];
      if (d['res_version'] != (prefs.getInt('res_version') ?? 0)) {
        prefs.setString('translator_hy', d['translator_hy']);
        prefs.setString('res', jsonEncode(d['res']));
        prefs.setInt('res_version', d['res_version']);
      }
      Res.initFrom(prefs.string('res'));
      Res.initTr(prefs.string('translator_hy'));
    }
  }
}

class AppAnimateBloc extends Bloc<AppAnimateEvent, AppAnimateStateIdle> {
  AppAnimateBloc() : super(AppAnimateStateIdle()) {
    on<AppAnimateEvent>((event, emit) => emit(AppAnimateStateIdle()));
    on<AppAnimateEventRaise>((event, emit) => emit(AppAnimateStateRaise()));
    on<AppAnimateEventShowMenu>((event, emit) => emit(AppAnimateShowMenu()));
    on<AppAnimateEventHideMenu>((event, emit) => emit(AppAnimateStateIdle()));
  }
}

import 'package:flutter_bloc/flutter_bloc.dart';

enum AppMenuState {msOpen, msClosed}

class AppMenuCubit extends Cubit<AppMenuState> {
  AppMenuCubit() : super(AppMenuState.msClosed);

  void toggle() => emit(state == AppMenuState.msClosed ? AppMenuState.msOpen : AppMenuState.msClosed);
}

enum AppLoadingState {idle, loading}

class AppLoadingCubit extends Cubit<AppLoadingState> {
  AppLoadingCubit() : super(AppLoadingState.idle);

  void change(AppLoadingState s) => emit(s);
}

const int cubIsMenuOpen = 1 << 0;
const int cubIsPasswordShow = 1 << 2;

class AppCubits extends Cubit<int> {

  AppCubits() : super(0);

  void toggleMenu() {
    int currentState = state;
    currentState ^= cubIsMenuOpen;
    emit (currentState);
  }

  void toggleShowPassword() {
    int currentState = state;
    currentState ^= cubIsPasswordShow;
    emit (currentState);
  }

}
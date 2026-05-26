import 'package:flutter_bloc/flutter_bloc.dart';

class DateState {}

class DateEvent {}

class DateBloc extends Bloc<DateEvent, DateState> {
  DateBloc() : super(DateState()) {
    on<DateEvent>((event, emit) => emit(DateState()));
  }
}

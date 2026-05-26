import 'package:equatable/equatable.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter_bloc/flutter_bloc.dart';

class QuestionState extends Equatable {
  @override
  List<Object?> get props => [];
}

class QuestionStateRaise extends QuestionState {
  final String question;
  final VoidCallback ifYes;
  final VoidCallback? ifNo;

  QuestionStateRaise(this.question, this.ifYes, this.ifNo);
}

class QuestionStateList extends QuestionState {
  final List<String> variants;
  final Function(int) callback;
  QuestionStateList(this.variants, this.callback);
}

class QuestionEvent extends Equatable {
  @override
  List<Object?> get props => [];
}

class QuestionEventRaise extends QuestionEvent {
  final String question;
  final VoidCallback ifYes;
  final VoidCallback? ifNo;

  QuestionEventRaise(this.question, this.ifYes, this.ifNo);
}

class QuestionEventList extends QuestionEvent {
  final List<String> variants;
  final Function(int) callback;
  QuestionEventList(this.variants, this.callback);
}

class QuestionBloc extends Bloc<QuestionEvent, QuestionState> {
  QuestionBloc() : super(QuestionState()) {
    on<QuestionEvent>((event, emit) => emit(QuestionState()));
    on<QuestionEventRaise>(
        (event, emit) => emit(QuestionStateRaise(event.question, event.ifYes, event.ifNo)));
    on<QuestionEventList>((event, emit) => emit(QuestionStateList(event.variants, event.callback)));
  }
}

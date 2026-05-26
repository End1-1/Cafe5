import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/model/model.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/styles.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';

class WMAppMenu extends StatefulWidget {
  final WMModel model;
  final List<Widget> menuWidgets;

  const WMAppMenu(this.model, this.menuWidgets, {super.key});

  @override
  State<StatefulWidget> createState() => _WMAppMenu();
}

class _WMAppMenu extends State<WMAppMenu> {
  var pos = 0.0;

  @override
  Widget build(BuildContext context) {
    final width = MediaQuery.sizeOf(context).width * 0.8;
    return BlocBuilder<AppAnimateBloc, AppAnimateStateIdle>(
        builder: (builder, state) {
      return Stack(
        children: [
          AnimatedSwitcher(
              duration: const Duration(milliseconds: 500),
              child: state.runtimeType == AppAnimateStateRaise
                  ? InkWell(
                      onTap: () {
                        pos = 0;
                        BlocProvider.of<AppAnimateBloc>(prefs.context())
                            .add(AppAnimateEvent());
                      },
                      child: Container(color: Colors.black38))
                  : Container()),
          AnimatedPositioned(
              width: width,
              height: MediaQuery.sizeOf(context).height,
              right:
                  state.runtimeType == AppAnimateStateRaise ? pos : 0 - width,
              duration: const Duration(milliseconds: 300),
              child: GestureDetector(
                  onPanUpdate: (d) {
                    if (pos - d.delta.dx > 0) {
                      return;
                    }
                    if ((pos - d.delta.dx).abs() > width * 0.5) {
                      pos = 0;
                      BlocProvider.of<AppAnimateBloc>(prefs.context())
                          .add(AppAnimateEvent());
                      return;
                    }
                    setState(() {
                      pos -= d.delta.dx;
                    });
                  },
                  onPanEnd: (d) {
                    setState(() {
                      pos = 0;
                    });
                  },
                  child: Container(
                    color: Color(0xff1e3d4b),
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.start,
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        for (final w in widget.menuWidgets) ...[
                          Styling.columnSpacingWidget(),
                          w,
                        ]
                      ],
                    ),
                  )))
        ],
      );
    });
  }
}

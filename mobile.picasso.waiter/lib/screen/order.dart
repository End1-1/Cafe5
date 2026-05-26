import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:flutter_slidable/flutter_slidable.dart';
import 'package:picassowaiter/bloc/app_bloc.dart';
import 'package:picassowaiter/model/model.dart';
import 'package:picassowaiter/screen/app.dart';
import 'package:picassowaiter/utils/http_query.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/waiter_cashbox.dart';
import 'package:picassowaiter/utils/waiter_dish_remove.dart';
import 'package:picassowaiter/utils/waiter_order_v2.dart';
import 'package:picassowaiter/utils/waiter_print_service.dart';
import 'package:picassowaiter/utils/styles.dart';

part 'order.model.dart';

class WMOrder extends WMApp {
  late final OrderModel _model;
  final _scrollController = ScrollController();

  WMOrder({super.key, required super.model, required int table}) {
    _model = OrderModel(table);
  }

  Map<String, dynamic> _popUnlockPayload() {
    return {
      'orderId': _model.order['f_id'] ?? '',
      'emptyOrder': _model.dishes.isEmpty,
    };
  }

  @override
  Widget? leadingButton(BuildContext context) {
    return IconButton(
      icon: const Icon(Icons.arrow_back),
      onPressed: () {
        Navigator.pop(context, _popUnlockPayload());
      },
    );
  }

  @override
  List<Widget> actions() {
    return [
      IconButton(
          onPressed: printService, icon: const Icon(Icons.print_outlined)),
      IconButton(
          onPressed: showDishMenu, icon: const Icon(Icons.menu_book_outlined)),
      IconButton(onPressed: _readQr, icon: const Icon(Icons.qr_code)),
    ];
  }

  @override
  String titleText() {
    return _model.tableName;
  }

  @override
  Widget body() {
    return Column(
      children: [Expanded(child: dishesWidget()), menuWidget(), Row(
        children: [
          Expanded(child: Container()),
          IconButton(onPressed: _readQr, icon: Icon(Icons.qr_code)),
          Expanded(child: Container())
        ],
      )],
    );
  }

  Widget dishesWidget() {
    return BlocBuilder<AppBloc, AppState>(
        buildWhen: (p, c) => c is AppStateDishes,
        builder: (builder, state) {
          return SingleChildScrollView(
            controller: _scrollController,
            child: Column(
              children: [
                for (final d in _model.dishes) ...[
                  Container(
                      decoration: const BoxDecoration(
                          border: Border(
                              bottom: BorderSide(color: Colors.black26))),
                      height: 80,
                      child: Slidable(
                          key: Key(d['f_id']),
                          endActionPane: ActionPane(
                            extentRatio: 0.25,
                            motion: const ScrollMotion(),
                            children: [
                              SlidableAction(
                                // An action can be bigger than the others.
                                flex: 1,
                                onPressed: (_) {
                                  removeDish(d['f_id']);
                                },
                                backgroundColor: const Color(0xFFFF6C6C),
                                foregroundColor: Colors.white,
                                icon: Icons.delete_outline,
                                label: model.tr('Remove'),
                              )
                            ],
                          ),
                          child: InkWell(
                              onTap: () {},
                              child: Row(children: [
                                SizedBox(
                                    width: 200,
                                    child: Text('${d['f_name']}',
                                        style: TextStyle(
                                            decoration: ((d['f_state'] as num?) ?? 0) > 1
                                                ? TextDecoration.lineThrough
                                                : TextDecoration.none,
                                            color: Colors.black))),
                                Expanded(child: Container()),
                                InkWell(
                                    onTap: () {
                                      changeQty(d);
                                    },
                                    onDoubleTap: () {
                                      changeQty1(d);
                                    },
                                    child: Container(
                                        alignment: Alignment.center,
                                        width: 40,
                                        height: 40,
                                        child: Text('${d['f_qty1']}',
                                            style: TextStyle(
                                                fontWeight: FontWeight.bold,
                                                color: ((d['f_qty2'] as num?) ?? 0) > 0
                                                    ? Colors.black
                                                    : Colors.red,
                                                decoration: ((d['f_state'] as num?) ?? 0) > 1
                                                    ? TextDecoration.lineThrough
                                                    : TextDecoration.none)))),
                                Styling.rowSpacingWidget(),
                                Styling.rowSpacingWidget(),
                              ])))),
                ]
              ],
            ),
          );
        });
  }

  Widget menuWidget() {
    final width = MediaQuery.sizeOf(prefs.context()).width;
    final height = MediaQuery.sizeOf(prefs.context()).height;
    return ValueListenableBuilder<int>(
      valueListenable: _model.menuTick,
      builder: (context, _, __) {
        return BlocBuilder<AppAnimateBloc, AppAnimateStateIdle>(
            builder: (builder, state) {
          return AnimatedContainer(
          width: width,
          height: state.runtimeType == AppAnimateShowMenu ? height * 0.65 : 0,
          duration: const Duration(milliseconds: 300),
          color: const Color(0xff1e3d4b),
          clipBehavior: Clip.hardEdge,
          child: SingleChildScrollView(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.start,
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Row(
                  children: [
                    Expanded(
                        child: SingleChildScrollView(
                            scrollDirection: Axis.horizontal,
                            child: Row(
                              children: [
                                for (final e in _model.menu.part1()) ...[
                                  InkWell(
                                    onTap: () => selectPart1('$e'),
                                    child: Container(
                                      height: 30,
                                      margin: const EdgeInsets.all(2),
                                      padding: const EdgeInsets.fromLTRB(
                                          5, 2, 5, 2),
                                      decoration: BoxDecoration(
                                        color: _model.currentPart1Filter ==
                                                '$e'
                                            ? const Color(0xffffe082)
                                            : Colors.white,
                                      ),
                                      child: Styling.text('$e'),
                                    ),
                                  ),
                                ],
                              ],
                            ))),
                    IconButton(
                        onPressed: topLevel,
                        icon: Icon(
                          Icons.home_outlined,
                          color: Colors.white,
                        ))
                  ],
                ),
                if (_model.showMode == OrderModel.show_part1)
                  Wrap(
                    children: [
                      for (final e in _model.menu.part2(
                        _model.currentMenuName,
                        _model.currentPart1Filter,
                      )) ...[
                        InkWell(
                            onTap: () {
                              filterDishes('$e');
                            },
                            child: Container(
                              height: 30,
                              margin: const EdgeInsets.all(3),
                              padding: const EdgeInsets.fromLTRB(5, 3, 5, 3),
                              decoration:
                                  const BoxDecoration(color: Colors.white),
                              child: Styling.text('$e'),
                            ))
                      ]
                    ],
                  ),
                if (_model.showMode == OrderModel.show_dishes)
                  Wrap(
                    children: [
                      for (final e in _model.menu.dishes(
                        _model.currentMenuName,
                        _model.currentPart2Filter,
                        part1filter: _model.currentPart1Filter,
                      )) ...[
                        InkWell(
                            onTap: () {
                              addDish(e);
                            },
                            child: Container(
                                margin: const EdgeInsets.all(3),
                                padding: const EdgeInsets.fromLTRB(5, 3, 5, 3),
                                decoration:
                                    const BoxDecoration(color: Colors.white),
                                child: Column(children: [
                                  Styling.text(e['f_name']),
                                  Styling.text('${e['f_price']}')
                                ])))
                      ]
                    ],
                  ),
              ],
            ),
          ));
        });
      },
    );
  }
}

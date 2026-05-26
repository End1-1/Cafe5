part of 'order.dart';

class AppLoadOpenOrder extends AppEventLoading {
  AppLoadOpenOrder(
    super.text,
    super.route,
    super.data,
    super.callback,
    super.state,
  );
}

class AppLoadDishes extends AppEventLoading {
  AppLoadDishes() : super('', '', {}, null, AppStateDishes());
}

class AppOpenOrderState extends AppStateFinished {
  AppOpenOrderState({required super.data});
}

class AppStateDishes extends AppStateFinished {
  AppStateDishes() : super(data: null);
}

class OrderModel {
  static const show_part1 = 1;
  static const show_dishes = 2;

  late final int table;
  var tableName = '??';
  var locked = false;
  var showMenu = false;
  final menu = Menu();
  var currentMenuName = '';
  var currentPart1Filter = '';
  var currentPart2Filter = '';
  var showMode = show_part1;
  var order = <String, dynamic>{};
  final dishes = [];
  /// Drives menu panel rebuild (WMOrder is stateless; AppAnimateBloc alone is not enough).
  final menuTick = ValueNotifier<int>(0);

  void bumpMenu() => menuTick.value++;

  OrderModel(int t) {
    table = t;
    refresh();
    if (menu.menu.isNotEmpty) {
      currentMenuName = '${menu.menu.entries.first.key}';
    }
  }

  void applyOrderFromApi(dynamic result) {
    final o = WaiterOrderV2.orderFromResponse(result);
    if (o == null) return;
    final om = Map<String, dynamic>.from(o);
    order = WaiterOrderV2.normalizeHeader(om);
    final next = WaiterOrderV2.dishesFromOrder(om);
    dishes
      ..clear()
      ..addAll(next);
  }

  void refresh() {
    WaiterCashbox.ensureSession().then((cashErr) {
      if (cashErr != null) {
        BlocProvider.of<AppBloc>(prefs.context()).add(AppEventError(cashErr));
        return;
      }
      BlocProvider.of<AppBloc>(prefs.context()).add(
        AppLoadOpenOrder(
          'Wait',
          WaiterOrderV2.routeOpenTable,
          {
            'table': table,
            'locksrc': 'mobilewaiter-${prefs.getInt('userid')}',
            'create_empty': true,
            'cashbox_id': prefs.cashboxId(),
          },
          (e, d) {
            locked = e;
            if (e) {
              return;
            }
            applyOrderFromApi(d);
            tableName = '${order['f_table_name'] ?? '??'}';
            BlocProvider.of<AppBloc>(prefs.context()).add(AppLoadDishes());
          },
          AppOpenOrderState(data: null),
        ),
      );
    });
  }
}

extension WMEOrder on WMOrder {
  void showDishMenu() {
    if (_model.showMenu) {
      BlocProvider.of<AppAnimateBloc>(
        prefs.context(),
      ).add(AppAnimateEventHideMenu());
    } else {
      _model.showMode = OrderModel.show_part1;
      _model.currentPart2Filter = '';
      BlocProvider.of<AppAnimateBloc>(
        prefs.context(),
      ).add(AppAnimateEventShowMenu());
      _model.bumpMenu();
    }
    _model.showMenu = !_model.showMenu;
  }

  void _refreshMenuPanel() {
    _model.bumpMenu();
    if (_model.showMenu) {
      BlocProvider.of<AppAnimateBloc>(
        prefs.context(),
      ).add(AppAnimateEventShowMenu());
    }
  }

  void printService() {
    final oid = _model.order['f_id'];
    if (oid == null || '$oid'.isEmpty) {
      return;
    }
    final state =
        int.tryParse('${_model.order['f_state'] ?? ''}') ?? 0;
    final route = state == WaiterPrintService.orderStatePreorder
        ? WaiterOrderV2.routePrintServicePreorder
        : WaiterOrderV2.routePrintServiceCheck;
    BlocProvider.of<AppBloc>(prefs.context()).add(
      AppLoadOpenOrder(
        'Wait',
        route,
        {'header_id': oid},
        (e, d) async {
          if (e) {
            return;
          }
          if (d is Map) {
            final doc = Map<String, dynamic>.from(d);
            _model.applyOrderFromApi(doc);
            final printErr = await WaiterPrintService.dispatchServiceCheck(doc);
            if (printErr != null && printErr.isNotEmpty) {
              if (!prefs.context().mounted) return;
              BlocProvider.of<AppBloc>(prefs.context())
                  .add(AppEventError(printErr));
              return;
            }
          }
          if (!prefs.context().mounted) return;
          Navigator.pop(prefs.context(), _popUnlockPayload());
        },
        AppOpenOrderState(data: null),
      ),
    );
  }

  void topLevel() {
    _model.showMenu = true;
    _model.currentPart1Filter = '';
    _model.currentPart2Filter = '';
    _model.showMode = OrderModel.show_part1;
    _refreshMenuPanel();
  }

  void selectPart1(String part1) {
    if (_model.currentPart1Filter == part1 &&
        _model.showMode == OrderModel.show_dishes) {
      topLevel();
      return;
    }
    _model.currentPart1Filter = part1;
    _model.currentPart2Filter = '';
    _model.showMode = OrderModel.show_dishes;
    _model.showMenu = true;
    _refreshMenuPanel();
  }

  void filterDishes(String filter) {
    _model.currentPart2Filter = filter;
    _model.showMode = OrderModel.show_dishes;
    _model.showMenu = true;
    _refreshMenuPanel();
  }

  void addDish(dynamic e) {
    if (_model.locked) {
      BlocProvider.of<AppBloc>(
        prefs.context(),
      ).add(AppEventError(model.tr('View only')));
      return;
    }
    final row = Map<String, dynamic>.from(e as Map);
    final d = <String, dynamic>{
      'dish': row['f_dish'],
      'dish_name': row['f_name'],
      'table': _model.table,
      'qty': 1,
      'type': 1,
      'row': _model.dishes.length * 100,
      'price': row['f_price'],
      'count_service': WaiterOrderV2.as01(row['f_service']),
      'count_discount': WaiterOrderV2.as01(row['f_discount']),
      'service_factor': _model.order['f_servicefactor'] ?? 0,
      'discount_factor': _model.order['f_discountfactor'] ?? 0,
      'f_data': WaiterOrderV2.menuItemToFData(row),
      'store': row['f_store'],
      'print1': row['f_print1'],
      'print2': row['f_print2'],
      'empty_order': _model.dishes.isEmpty,
    };
    BlocProvider.of<AppBloc>(prefs.context()).add(
      AppLoadOpenOrder('Wait', WaiterOrderV2.routeAddDish, d, (err, d) {
        if (err) {
          return;
        }
        _model.applyOrderFromApi(d);
        BlocProvider.of<AppBloc>(prefs.context()).add(AppLoadDishes());
        _scrollController.animateTo(
          100000,
          duration: const Duration(milliseconds: 100),
          curve: Curves.ease,
        );
      }, AppOpenOrderState(data: null)),
    );
  }

  void removeDish(String id) {
    if (_model.locked) {
      BlocProvider.of<AppBloc>(
        prefs.context(),
      ).add(AppEventError(model.tr('View only')));
      return;
    }
    final e = _model.dishes.cast<Map>().firstWhere(
          (element) => '${element['f_id']}' == id,
          orElse: () => <String, dynamic>{},
        );
    if (e.isEmpty) return;

    final row = Map<String, dynamic>.from(e);
    final ctx = prefs.context();
    WaiterDishRemove.run(
      model: model,
      order: _model.order,
      dish: row,
      onSuccess: (response) {
        _model.applyOrderFromApi(response);
        BlocProvider.of<AppBloc>(ctx).add(AppLoadDishes());
      },
      onError: (message) {
        BlocProvider.of<AppBloc>(ctx).add(AppEventError(message));
      },
    );
  }

  void changeQty(dynamic ddd) {
    if (((ddd['f_qty2'] as num?) ?? 0) > 0) {
      return;
    }
    final _controller = TextEditingController();
    var alert = AlertDialog(
      title: Text('${ddd['f_name']}'),
      content: TextField(
        style: const TextStyle(decoration: TextDecoration.none),
        maxLines: 1,
        keyboardType: TextInputType.number,
        autofocus: true,
        enabled: true,
        onSubmitted: (String text) {
          var qty = double.tryParse(text) ?? 1;
          Navigator.pop(prefs.context(), qty);
        },
        controller: _controller,
        decoration: InputDecoration(
          errorStyle: const TextStyle(color: Colors.redAccent),
          border: UnderlineInputBorder(
            borderSide: const BorderSide(
              color: Color.fromRGBO(40, 40, 40, 1.0),
            ),
            borderRadius: BorderRadius.circular(10.0),
          ),
          focusedBorder: UnderlineInputBorder(
            borderSide: const BorderSide(
              color: Color.fromRGBO(40, 40, 40, 1.0),
            ),
            borderRadius: BorderRadius.circular(10.0),
          ),
          disabledBorder: UnderlineInputBorder(
            borderSide: const BorderSide(
              color: Color.fromRGBO(40, 40, 40, 1.0),
            ),
            borderRadius: BorderRadius.circular(10.0),
          ),
          prefixIcon: const Icon(Icons.playlist_add, size: 18.0),
        ),
      ),
    );

    showDialog(
      context: prefs.context(),
      builder: (context) {
        return alert;
      },
    ).then((value) {
      if (value != null && value > 0) {
        _setDishQty(ddd, value as num);
      }
    });
  }

  void _setDishQty(dynamic row, num newQty) {
    final oldQty = (row['f_qty1'] as num?) ?? 0;
    final fd = Map<String, dynamic>.from(
      (row['_f_data'] as Map?)?.cast<String, dynamic>() ?? {},
    );
    final emarks = row['f_emarks'];
    final body = <String, dynamic>{
      'id': row['f_id'],
      'remove_emarks': emarks != null && '$emarks'.isNotEmpty,
      'dish': row['f_dish'],
      'dish_name': row['f_name'],
      'new_qty': newQty.toDouble(),
      'new_state': WaiterOrderV2.dishStateOk,
      'data': fd,
      'order_id': _model.order['f_id'],
    };
    final deltaStop = oldQty - newQty.toDouble();
    if (deltaStop > 0) {
      body['restore_stoplist'] = deltaStop;
    }
    BlocProvider.of<AppBloc>(prefs.context()).add(
      AppLoadOpenOrder('Wait', WaiterOrderV2.routeSetDishQty, body, (err, d) {
        if (err) {
          return;
        }
        _model.applyOrderFromApi(d);
        BlocProvider.of<AppBloc>(prefs.context()).add(AppLoadDishes());
      }, AppOpenOrderState(data: null)),
    );
  }

  void changeQty1(dynamic e) {
    if (((e['f_qty2'] as num?) ?? 0) > 0) {
      return;
    }
    final qty = ((e['f_qty1'] as num?) ?? 0) + 1;
    _setDishQty(e, qty);
  }

  void _readQr() async {
    var v = await nav.readBarcode();

    if (v != null) {
      var barcode = '';
      if (v.length == 13 || v.length == 8) {
        barcode = v;
      } else if (v.length > 28) {
        if (v.substring(0, 6) == '000000') {
          barcode = v.substring(3, 11);
        } else if (v.substring(0, 3) == '010') {
          if (v.substring(0, 8) == '01000000') {
            barcode = v.substring(8, 16);
          } else {
            barcode = v.substring(3, 16);
          }
        }
      }
      if (barcode.isEmpty) {
        if (kDebugMode) {
          print('wrong barcode 1, $v');
        }
        BlocProvider.of<AppBloc>(prefs.context()).add(
          AppEventError(locale().wrongBarcode + '\r\n' + v + '\r\n' + barcode),
        );
        return;
      }

      final dish = _model.menu.firstDishOfBarcode(
        _model.currentMenuName,
        barcode,
      );
      if (dish == null) {
        if (kDebugMode) {
          print('wrong barcode 2, $v $barcode');
        }
        BlocProvider.of<AppBloc>(prefs.context()).add(
          AppEventError(locale().wrongBarcode + '\r\n' + v + '\r\n' + barcode),
        );
        return;
      }
      HttpQuery('engine/picasso.waiter/')
          .request({'class': 'waiter', 'method': 'checkQr', 'qr': v})
          .then((reply) {
            if (reply['status'] == 1) {
              dish['f_emarks'] = v;
              addDish(dish);
            } else {
              BlocProvider.of<AppBloc>(
                prefs.context(),
              ).add(AppEventError(reply['data']));
            }
          });
    }
  }
}

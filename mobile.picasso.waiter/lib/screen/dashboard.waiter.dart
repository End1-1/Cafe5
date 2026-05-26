part of 'dashboard.dart';

extension WaiterDashboard on WMDashboard {
  void getDashboardWaiter() {
    final ctx = prefs.context();
    ctx.read<AppLoadingCubit>().change(AppLoadingState.loading);
    Future(() async {
      final hallRes = await HttpQuery(WaiterInitV2.routeHallGet).request({});
      if (hallRes['status'] != 1) {
        ctx.read<AppLoadingCubit>().change(AppLoadingState.idle);
        if (!ctx.mounted) return;
        BlocProvider.of<AppBloc>(ctx).add(
          AppEventError('${hallRes['data'] ?? hallRes}'),
        );
        return;
      }
      WaiterInitV2.applyHall(
        hallRes['data'] ?? hallRes,
        halls: _model.halls,
        tables: _model.tables,
        openTables: _model.openTables,
        filteredTables: _model.filteredTables,
      );
      final menuRes = await HttpQuery(WaiterInitV2.routeMenuGet).request({});
      ctx.read<AppLoadingCubit>().change(AppLoadingState.idle);
      if (!ctx.mounted) return;
      if (menuRes['status'] != 1) {
        BlocProvider.of<AppBloc>(ctx).add(
          AppEventError('${menuRes['data'] ?? menuRes}'),
        );
        return;
      }
      Menu().buildFromV2(menuRes['data'] ?? menuRes);
      BlocProvider.of<AppBloc>(ctx).add(AppEventLoading(
        '',
        '',
        {},
        (_, __) {},
        AppStateDashboard(data: _model),
      ));
    });
  }

  Widget bodyWaiter() {
    return BlocBuilder<AppBloc, AppState>(
        buildWhen: (p, c) => c is AppStateDashboard,
        builder: (builder, state) {
          if (state is! AppStateDashboard) {
            return Container();
          }

          return SingleChildScrollView(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Row(children: [
                  Expanded(
                      child: SingleChildScrollView(
                          scrollDirection: Axis.horizontal,
                          child: Row(
                            children: [
                              InkWell(
                                  onTap: () {
                                    filterTables(0);
                                  },
                                  child: Styling.text(model.tr('All'))),
                              Styling.rowSpacingWidget(),
                              InkWell(
                                  onTap: () {
                                    filterTables(-1);
                                  },
                                  child: Styling.text(model.tr('My'))),
                              Styling.rowSpacingWidget(),
                              for (final f in _model.halls) ...[
                                InkWell(
                                    onTap: () {
                                      filterTables(f['f_id']);
                                    },
                                    child: Styling.text(f['f_name'])),
                                Styling.rowSpacingWidget(),
                              ]
                            ],
                          )))
                ]),
                const Divider(),
                ...[...hallWidget()]
              ],
            ),
          );
        });
  }

  void filterTables(int hallid) {
    _model.hallFilter = hallid;
    BlocProvider.of<AppBloc>(prefs.context()).add(AppEventLoading(
        model.tr('Wait'),
        '',
        {},
        (p0, p1) => null,
        AppStateDashboard(data: _model)));
  }

  List<Widget> hallWidget() {
    if (_model.hallFilter < 0) {
      return [
        Align(
            alignment: Alignment.topLeft,
            child: Wrap(
                crossAxisAlignment: WrapCrossAlignment.start,
                runAlignment: WrapAlignment.start,
                alignment: WrapAlignment.start,
                children: tablesWidgets(-1)))
      ];
    }
    final hl = _model.hallFilter > 0
        ? _model.halls.where((element) => element['f_id'] == _model.hallFilter)
        : _model.halls;

    return [
      for (final h in hl) ...[
        Row(children: [Styling.text(h['f_name'])]),
        const Divider(),
        Align(
            alignment: Alignment.topLeft,
            child: Wrap(
              crossAxisAlignment: WrapCrossAlignment.start,
              runAlignment: WrapAlignment.start,
              alignment: WrapAlignment.start,
              children: tablesWidgets(h['f_id']),
            ))
      ]
    ];
  }

  Color tableColor(dynamic table) {
    if (_model.openTables.containsKey(table['f_id'])) {
      final o = _model.openTables[table['f_id']];
      if (o['f_precheck'] > 0) {
        return Color(0xffff9797);
      }
      return Color(0xff6fff76);
    }
    return Colors.white;
  }

  dynamic orderValue(dynamic table, String key) {
    if (_model.openTables.containsKey(table['f_id'])) {
      final o = _model.openTables[table['f_id']];
      return o[key] ?? '???';
    }
    return '';
  }

  List<dynamic> myTables() {
    final l = [];
    for (final t in _model.tables) {
      if (_model.openTables.containsKey(t['f_id'])) {
        final o = _model.openTables[t['f_id']];
        if (o['f_staff'] == prefs.getInt('userid')) {
          l.add(t);
        }
      }
    }
    return l;
  }

  static const _tableTileStyle = TextStyle(color: Colors.black, fontSize: 11);

  Widget _tableTileLine(String text, {FontWeight fontWeight = FontWeight.normal}) {
    return Text(
      text,
      maxLines: 1,
      overflow: TextOverflow.ellipsis,
      style: _tableTileStyle.copyWith(fontWeight: fontWeight),
    );
  }

  List<Widget> tablesWidgets(int hall) {
    final tl = hall < 0 ? myTables() : _model.filteredTables[hall];
    return [
      for (final t in tl) ...[
        InkWell(
            onTap: () {
              model.navigation
                  .openWaiterTable(t['f_id'])
                  .then((value) => unlockTable(t['f_id'], value));
            },
            child: Container(
              padding: const EdgeInsets.all(4),
              margin: const EdgeInsets.all(3),
              decoration: BoxDecoration(
                border:
                    Border.fromBorderSide(BorderSide(color: Colors.black12)),
                color: tableColor(t),
              ),
              height: 100,
              width: 100,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  _tableTileLine('${t['f_name']}', fontWeight: FontWeight.w600),
                  _tableTileLine(orderValue(t, 'f_staffname')),
                  const Spacer(),
                  _tableTileLine('${orderValue(t, 'f_amounttotal')}'),
                ],
              ),
            ))
      ]
    ];
  }

  void unlockTable(int id, [dynamic popResult]) {
    var orderId = '';
    var emptyOrder = true;
    if (popResult is Map) {
      orderId = '${popResult['orderId'] ?? ''}';
      final eo = popResult['emptyOrder'];
      if (eo is bool) {
        emptyOrder = eo;
      }
    }
    BlocProvider.of<AppBloc>(prefs.context()).add(AppEventLoading(
        'Wait',
        'engine/v2/waiter/order/unlock-table',
        {
          'table': id,
          'locksrc': 'mobilewaiter-${prefs.getInt('userid')}',
          'id': orderId,
          'empty_order': emptyOrder,
        },
        (e, d) {
          if (e) {
            return;
          }
        },
        AppStateDashboard(data: model)));
  }
}

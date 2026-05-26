import 'dart:async';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:picassowaiter/bloc/app_cubits.dart';
import 'package:picassowaiter/bloc/question_bloc.dart';
import 'package:picassowaiter/model/model.dart';
import 'package:picassowaiter/utils/http_query.dart';
import 'package:picassowaiter/utils/prefs.dart';
import 'package:picassowaiter/utils/waiter_auth.dart';
import 'package:picassowaiter/utils/waiter_order_v2.dart';
import 'package:picassowaiter/utils/waiter_print_service.dart';

/// Dish removal flow aligned with [DlgOrder::on_btnVoid_clicked].
class WaiterDishRemove {
  WaiterDishRemove._();

  static Map<String, dynamic>? _lastResponse;

  static Future<void> run({
    required WMModel model,
    required Map<String, dynamic> order,
    required Map<String, dynamic> dish,
    required void Function(Map<String, dynamic> response) onSuccess,
    required void Function(String message) onError,
  }) async {
    if (_orderLockedByPrecheck(order)) {
      onError(model.tr('Order is not editable'));
      return;
    }

    if (!WaiterOrderV2.canRemoveDishRow(dish)) {
      return;
    }

    final type = dish['f_type'] as int? ?? 0;
    if (type == WaiterOrderV2.goodsTypePackage) {
      await _removePackage(
        model: model,
        order: order,
        dish: dish,
        onSuccess: onSuccess,
        onError: onError,
      );
      return;
    }

    await _removeLine(
      model: model,
      order: order,
      dish: dish,
      onSuccess: onSuccess,
      onError: onError,
    );
  }

  static bool _orderLockedByPrecheck(Map<String, dynamic> order) {
    final state = int.tryParse('${order['f_state'] ?? ''}') ?? 0;
    if (state == WaiterOrderV2.orderStatePreorder) return false;
    final precheck = (order['f_precheck'] as num?)?.toInt() ?? 0;
    return precheck > 0;
  }

  static Future<void> _removePackage({
    required WMModel model,
    required Map<String, dynamic> order,
    required Map<String, dynamic> dish,
    required void Function(Map<String, dynamic> response) onSuccess,
    required void Function(String message) onError,
  }) async {
    if (!await _confirmRemove(model)) return;

    if (WaiterOrderV2.isPrintedDishRow(dish)) {
      await _removePrinted(
        model: model,
        order: order,
        dish: dish,
        isPackage: true,
        onSuccess: onSuccess,
        onError: onError,
      );
      return;
    }

    final body = _baseBody(order, dish)
      ..['new_qty'] = 0
      ..['new_state'] = 0;

    final err = await _postRemove(
      route: WaiterOrderV2.routeRemovePackage,
      body: body,
      isPackage: true,
      order: order,
    );
    if (err != null) {
      onError(err);
      return;
    }
    onSuccess(_lastResponse!);
  }

  static Future<void> _removeLine({
    required WMModel model,
    required Map<String, dynamic> order,
    required Map<String, dynamic> dish,
    required void Function(Map<String, dynamic> response) onSuccess,
    required void Function(String message) onError,
  }) async {
    if (!await _confirmRemove(model)) return;

    if (WaiterOrderV2.isPrintedDishRow(dish)) {
      await _removePrinted(
        model: model,
        order: order,
        dish: dish,
        isPackage: false,
        onSuccess: onSuccess,
        onError: onError,
      );
      return;
    }

    final body = _baseBody(order, dish)
      ..['new_qty'] = 0
      ..['new_state'] = 0;

    final err = await _postRemove(
      route: WaiterOrderV2.routeSetDishQty,
      body: body,
      isPackage: false,
      order: order,
    );
    if (err != null) {
      onError(err);
      return;
    }
    onSuccess(_lastResponse!);
  }

  static Future<void> _removePrinted({
    required WMModel model,
    required Map<String, dynamic> order,
    required Map<String, dynamic> dish,
    required bool isPackage,
    required void Function(Map<String, dynamic> response) onSuccess,
    required void Function(String message) onError,
  }) async {
    final reason = await _pickRemoveReason(model);
    if (reason == null) return;

    final newState = await _pickRemoveState(model);
    if (newState == null || newState == 0) return;

    final qty = (dish['f_qty1'] as num?)?.toDouble() ?? 0;
    final body = _baseBody(order, dish)
      ..['new_qty'] = qty
      ..['new_state'] = newState
      ..['remove_reason'] = reason;

    Future<String?> callApi() => _postRemove(
          route: isPackage
              ? WaiterOrderV2.routeRemovePackage
              : WaiterOrderV2.routeSetDishQty,
          body: body,
          isPackage: isPackage,
          order: order,
        );

    String? err;
    if (WaiterAuth.hasRemovePrintedGoodsPermission()) {
      err = await callApi();
    } else {
      final pin = await _promptPin(model, model.tr('Remove printed dish'));
      if (pin == null || pin.isEmpty) return;
      err = await WaiterAuth.withRemovePrintedElevation(pin, callApi);
    }

    if (err != null && err.isNotEmpty) {
      onError(err);
      return;
    }
    onSuccess(_lastResponse!);
  }

  static Map<String, dynamic> _baseBody(
    Map<String, dynamic> order,
    Map<String, dynamic> dish,
  ) {
    final fd = Map<String, dynamic>.from(
      (dish['_f_data'] as Map?)?.cast<String, dynamic>() ?? {},
    );
    final emarks = dish['f_emarks'];
    return {
      'id': dish['f_id'],
      'order_id': order['f_id'],
      'dish': dish['f_dish'],
      'dish_name': dish['f_name'],
      'data': fd,
      'remove_emarks': emarks != null && '$emarks'.isNotEmpty,
      'restore_stoplist': dish['f_qty1'],
    };
  }

  static Future<String?> _postRemove({
    required String route,
    required Map<String, dynamic> body,
    required bool isPackage,
    required Map<String, dynamic> order,
  }) async {
    final ctx = prefs.context();
    if (ctx.mounted) {
      ctx.read<AppLoadingCubit>().change(AppLoadingState.loading);
    }
    final r = await HttpQuery(route).request(body);
    if (ctx.mounted) {
      ctx.read<AppLoadingCubit>().change(AppLoadingState.idle);
    }
    if (r['status'] == 0) {
      return '${r['data'] ?? r}';
    }
    final payload = Map<String, dynamic>.from(r);
    _lastResponse = payload;

    final orderMap = WaiterOrderV2.orderFromResponse(payload) ?? order;
    final header = Map<String, dynamic>.from(orderMap);

    if (isPackage) {
      final list = payload['removed_dishes'];
      if (list is List) {
        for (final item in list) {
          if (item is Map) {
            await WaiterPrintService.dispatchRemovedDish(
              Map<String, dynamic>.from(item),
              header: header,
            );
          }
        }
      }
    } else if (payload['f_removed_dish_name'] != null) {
      await WaiterPrintService.dispatchRemovedDish(payload, header: header);
    }
    return null;
  }

  static Future<bool> _confirmRemove(WMModel model) async {
    final completer = Completer<bool>();
    BlocProvider.of<QuestionBloc>(prefs.context()).add(
      QuestionEventRaise(
        model.tr('Do you want to remove this item'),
        () {
          if (!completer.isCompleted) completer.complete(true);
        },
        () {
          if (!completer.isCompleted) completer.complete(false);
        },
      ),
    );
    return completer.future;
  }

  static Future<String?> _pickRemoveReason(WMModel model) async {
    final r = await HttpQuery(WaiterOrderV2.routeGetRemoveReason).request({});
    List<dynamic> rows = [];
    dynamic payload = r['data'] ?? r;
    if (payload is String && payload.isNotEmpty) {
      try {
        payload = jsonDecode(payload);
      } catch (_) {}
    }
    if (payload is Map && payload['data'] is List) {
      rows = payload['data'] as List;
    } else if (payload is List) {
      rows = payload;
    }

    final labels = <String>[];
    for (final row in rows) {
      if (row is Map) {
        final name = '${row['f_name'] ?? ''}'.trim();
        if (name.isNotEmpty) labels.add(name);
      }
    }
    if (labels.isEmpty) {
      return '';
    }

    final completer = Completer<String?>();
    BlocProvider.of<QuestionBloc>(prefs.context()).add(
      QuestionEventList([
        ...labels,
        model.tr('Cancel'),
      ], (index) {
        model.closeQuestionDialog();
        if (!completer.isCompleted) {
          if (index < 0 || index >= labels.length) {
            completer.complete(null);
          } else {
            completer.complete(labels[index]);
          }
        }
      }),
    );
    return completer.future;
  }

  static Future<int?> _pickRemoveState(WMModel model) async {
    final labels = [
      model.tr('Mistake'),
      model.tr('With store output'),
      model.tr('Cancel'),
    ];
    final values = [
      WaiterOrderV2.dishStateMistake,
      WaiterOrderV2.dishStateVoid,
      0,
    ];

    final completer = Completer<int?>();
    BlocProvider.of<QuestionBloc>(prefs.context()).add(
      QuestionEventList(labels, (index) {
        model.closeQuestionDialog();
        if (!completer.isCompleted) {
          if (index < 0 || index >= values.length) {
            completer.complete(null);
          } else {
            completer.complete(values[index]);
          }
        }
      }),
    );
    return completer.future;
  }

  static Future<String?> _promptPin(WMModel model, String title) async {
    final ctx = prefs.context();
    final controller = TextEditingController();
    return showDialog<String>(
      context: ctx,
      builder: (dialogCtx) => AlertDialog(
        title: Text(title),
        content: TextField(
          controller: controller,
          obscureText: true,
          keyboardType: TextInputType.number,
          autofocus: true,
          decoration: const InputDecoration(
            border: OutlineInputBorder(),
          ),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(dialogCtx),
            child: Text(model.tr('Cancel')),
          ),
          TextButton(
            onPressed: () => Navigator.pop(dialogCtx, controller.text.trim()),
            child: Text(model.tr('OK')),
          ),
        ],
      ),
    );
  }
}

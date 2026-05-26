import 'package:flutter/material.dart';

import 'app.dart';

class WMGoodsInfo extends WMApp {
  final Map<String, dynamic> info;
  WMGoodsInfo(this.info, {super.key, required super.model});

  @override
  String titleText() {
    return info['f_goodsname'];
  }

  @override
  Widget body() {
   return Container();
  }

}
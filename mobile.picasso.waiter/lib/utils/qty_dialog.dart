import 'package:picassowaiter/utils/prefs.dart';
import 'package:flutter/material.dart';

class QtyDialog {
  final textController = TextEditingController();
  void btn(String s) {
    if (s == '.') {
      if (textController.text.contains(s)) {
        return;
      }
    }
    textController.text += s;
  }

  void backspace() {
    textController.text = textController.text.substring(0, textController.text.length - 1);
  }

  Future<double?> getQty() {
    return showDialog(
      builder: (builder) {
        return SimpleDialog(
          children: [
            Row(children: [
              Expanded(child: TextFormField(controller: textController)),
              IconButton(onPressed: backspace, icon: const Icon(Icons.backspace_outlined))
            ]),
            Row(children: [
              TextButton(onPressed: (){btn('1');}, child: const Text('1')),
              TextButton(onPressed: (){btn('2');}, child: const Text('2')),
              TextButton(onPressed: (){btn('3');}, child: const Text('3')),
            ]),
            Row(children: [
              TextButton(onPressed: (){btn('4');}, child: const Text('4')),
              TextButton(onPressed: (){btn('5');}, child: const Text('5')),
              TextButton(onPressed: (){btn('6');}, child: const Text('6')),
            ]),
            Row(children: [
              TextButton(onPressed: (){btn('7');}, child: const Text('7')),
              TextButton(onPressed: (){btn('8');}, child: const Text('8')),
              TextButton(onPressed: (){btn('9');}, child: const Text('9')),
            ]),
            Row(children: [
              TextButton(onPressed: (){btn('0');}, child: const Text('0')),
              TextButton(onPressed: (){btn('.');}, child: const Text('.')),
              TextButton(onPressed: (){Navigator.pop(builder);}, child: const Text('X')),
            ]),
            Row(children: [
              TextButton(onPressed: (){Navigator.pop(builder, double.tryParse(textController.text) ?? 0);}, child: const Text('OK')),
            ])
          ],
        );
      }, context: prefs.context()
    );
  }


}
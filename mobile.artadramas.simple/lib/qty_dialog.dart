import 'package:flutter/material.dart';

import 'app_model.dart';
import 'i18n.dart';

Future<double?> showQtyDialog(
  BuildContext context, {
  required String title,
  required String unit,
  String initial = '',
}) {
  return showDialog<double>(
    context: context,
    builder: (context) => _QtyDialog(title: title, unit: unit, initial: initial),
  );
}

class _QtyDialog extends StatefulWidget {
  const _QtyDialog({required this.title, required this.unit, required this.initial});

  final String title;
  final String unit;
  final String initial;

  @override
  State<_QtyDialog> createState() => _QtyDialogState();
}

class _QtyDialogState extends State<_QtyDialog> {
  late String _text;

  @override
  void initState() {
    super.initState();
    _text = widget.initial;
  }

  void _key(String key) {
    setState(() {
      if (key == 'back') {
        if (_text.isNotEmpty) {
          _text = _text.substring(0, _text.length - 1);
        }
        return;
      }
      if (key == '.') {
        if (_text.contains('.')) {
          return;
        }
        _text = _text.isEmpty ? '0.' : '$_text.';
        return;
      }
      if (_text == '0') {
        _text = key;
      } else if (_text.length < 10) {
        _text += key;
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    final lang = appModel.lang;
    final qty = parseQty(_text);
    const keys = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '0', 'back'];
    return AlertDialog(
      title: Text(widget.title),
      content: SizedBox(
        width: 360,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(
              _text.isEmpty ? '0' : _text,
              style: const TextStyle(fontSize: 40, fontWeight: FontWeight.w600),
            ),
            if (widget.unit.isNotEmpty) Text(widget.unit),
            const SizedBox(height: 8),
            GridView.count(
              crossAxisCount: 3,
              shrinkWrap: true,
              childAspectRatio: 1.7,
              mainAxisSpacing: 8,
              crossAxisSpacing: 8,
              physics: const NeverScrollableScrollPhysics(),
              children: keys.map((key) {
                return OutlinedButton(
                  onPressed: () => _key(key),
                  child: key == 'back'
                      ? const Icon(Icons.backspace_outlined)
                      : Text(key, style: const TextStyle(fontSize: 24)),
                );
              }).toList(),
            ),
          ],
        ),
      ),
      actions: [
        TextButton(
          onPressed: () => Navigator.pop(context),
          child: Text(S.t(lang, 'cancel')),
        ),
        FilledButton(
          onPressed: qty == null ? null : () => Navigator.pop(context, qty),
          child: Text(S.t(lang, 'done')),
        ),
      ],
    );
  }
}

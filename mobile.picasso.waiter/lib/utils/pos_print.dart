/// ESC/POS-style command list compatible with [C5Printing::jsonData] / print server.
class PosPrint {
  final List<Map<String, dynamic>> _cmds = [];

  List<Map<String, dynamic>> jsonData() => List<Map<String, dynamic>>.from(_cmds);

  void setFontSize(int size) {
    _cmds.add({'cmd': 'fontsize', 'size': size});
  }

  void setFontBold(bool bold) {
    _cmds.add({'cmd': 'fontbold', 'bold': bold});
  }

  void ltext(String text, {int x = 0, int textWidth = 0}) {
    _cmds.add({
      'cmd': 'ltext',
      'text': text,
      'x': x,
      if (textWidth > 0) 'textwidth': textWidth,
    });
  }

  void ctext(String text) {
    _cmds.add({'cmd': 'ctext', 'text': text});
  }

  void rtext(String text) {
    _cmds.add({'cmd': 'rtext', 'text': text});
  }

  void line({int width = 1}) {
    if (width > 1) {
      _cmds.add({'cmd': 'line2', 'width': width});
    } else {
      _cmds.add({'cmd': 'line'});
    }
  }

  void br([num height = 0]) {
    _cmds.add({'cmd': 'br', 'height': height});
  }
}

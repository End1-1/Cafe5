import 'package:artadramas/i18n.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  test('qty accepts integers and decimals', () {
    expect(parseQty('30'), 30);
    expect(parseQty('1.5'), 1.5);
    expect(parseQty('1,25'), 1.25);
    expect(parseQty(''), null);
    expect(parseQty('0'), null);
    expect(formatQty(30), '30');
    expect(formatQty(1.5), '1.5');
  });
}

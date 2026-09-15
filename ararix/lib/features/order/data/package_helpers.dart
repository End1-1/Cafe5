import 'menu_models.dart';

const attributeKeys = ['Type', 'Size'];

String componentAttr(PackageComponent c, String key) {
  if (key == 'Type') return c.attrType;
  if (key == 'Size') return c.attrSize;
  return '';
}

bool componentMatches(
  PackageComponent c,
  Map<String, String> selections,
) {
  for (final entry in selections.entries) {
    final selected = entry.value;
    if (selected.isEmpty) continue;
    if (componentAttr(c, entry.key) != selected) return false;
  }
  return true;
}

Map<String, List<String>> uniqueAttributeOptions(
  List<PackageComponent> components,
) {
  final result = <String, List<String>>{};
  for (final key in attributeKeys) {
    final values = <String>[];
    for (final c in components) {
      final value = componentAttr(c, key);
      if (value.isEmpty || values.contains(value)) continue;
      values.add(value);
    }
    result[key] = values;
  }
  return result;
}

PackageComponent? findPackageComponent(
  List<PackageComponent> components,
  Map<String, String> selections,
) {
  for (final c in components) {
    if (componentMatches(c, selections)) return c;
  }
  return null;
}

double fixedAttributeOptionPrice(
  List<PackageComponent> components,
  String key,
  String value,
) {
  for (final c in components) {
    if (key == 'Type' && c.attrType == value) return c.attrTypePrice;
    if (key == 'Size' && c.attrSize == value) return c.attrSizePrice;
  }
  return 0;
}

bool packageNeedsAttributePicker(List<PackageComponent> components) {
  if (components.isEmpty) return false;
  final options = uniqueAttributeOptions(components);
  return attributeKeys.any((k) => (options[k] ?? const []).length > 1);
}

double packageLinePrice(MenuDish pkg, PackageComponent component) {
  return pkg.price + component.attrTypePrice + component.attrSizePrice;
}

/// Resolve package + selected component into a cart line dish (SelfBoard logic).
MenuDish resolvePackageCartLine(MenuDish pkg, PackageComponent component) {
  return pkg.copyWith(
    id: component.goodsId,
    packageId: pkg.id,
    packageName: pkg.name,
    name: pkg.name,
    price: packageLinePrice(pkg, component),
    image: (component.image != null && component.image!.isNotEmpty)
        ? component.image
        : pkg.image,
    type: 1,
    attrType: component.attrType,
    attrSize: component.attrSize,
    attrMeasurement: component.attrMeasurement,
    packageComponents: const [],
    needsConfig: false,
  );
}

String attributeDisplayLabel(String value, String measurement) {
  if (value.isEmpty) return '';
  if (measurement.isEmpty) return value;
  return '$value $measurement';
}

Map<String, String> defaultAttributeSelections(
  List<PackageComponent> components,
) {
  final options = uniqueAttributeOptions(components);
  final selections = <String, String>{};
  for (final key in attributeKeys) {
    final values = options[key] ?? const [];
    selections[key] = values.isNotEmpty ? values.first : '';
  }
  if (components.isEmpty) return selections;

  // Prefer first component's attrs when possible.
  final first = components.first;
  if (first.attrType.isNotEmpty) selections['Type'] = first.attrType;
  if (first.attrSize.isNotEmpty) selections['Size'] = first.attrSize;
  return selections;
}

bool dishNeedsPackagePicker(MenuDish dish) {
  if (!dish.isPackage) return false;
  return dish.relatedDrinks.isNotEmpty ||
      dish.relatedOther.isNotEmpty ||
      dish.modificators.isNotEmpty ||
      packageNeedsAttributePicker(dish.packageComponents) ||
      dish.packageComponents.isNotEmpty;
}

#include "menuhelpers.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

namespace {

QString formatCookingTime(int minutes)
{
    if (minutes <= 0) {
        return QString();
    }
    return QCoreApplication::translate("MenuHelpers", "%1 Min").arg(minutes);
}

QString dynStr(const QJsonValue &v)
{
    if (v.isObject()) {
        return v.toObject().value(QStringLiteral("value")).toString().trimmed();
    }
    return v.toString().trimmed();
}

double dynPrice(const QJsonValue &v)
{
    if (v.isObject()) {
        return v.toObject().value(QStringLiteral("price")).toDouble(0.0);
    }
    return 0.0;
}

QString dynMeasurement(const QJsonObject &dynObj, const QString &key)
{
    const QJsonValue val = dynObj.value(key);
    if (val.isObject()) {
        const QString m = val.toObject().value(QStringLiteral("measurement")).toString().trimmed();
        if (!m.isEmpty()) {
            return m;
        }
    }
    if (key == QStringLiteral("Size")) {
        return dynObj.value(QStringLiteral("Measurement")).toString().trimmed();
    }
    return {};
}

bool componentMatchesSelections(
    const MenuPackageComponent &component,
    const QHash<QString, QString> &selections)
{
    for (auto it = selections.constBegin(); it != selections.constEnd(); ++it) {
        const QString &key = it.key();
        const QString &selected = it.value();
        if (selected.isEmpty()) {
            continue;
        }
        QString actual;
        if (key == QStringLiteral("Type")) {
            actual = component.attributes.type;
        } else if (key == QStringLiteral("Size")) {
            actual = component.attributes.size;
        } else {
            continue;
        }
        if (actual != selected) {
            return false;
        }
    }
    return true;
}

bool componentMatchesSelections(
    const MenuPackageComponent &component,
    const QHash<QString, QString> &selections,
    const QString &ignoreKey)
{
    QHash<QString, QString> filtered = selections;
    filtered.remove(ignoreKey);
    return componentMatchesSelections(component, filtered);
}

} // namespace

QJsonObject MenuDynamicAttributes::toJsonObject() const
{
    QJsonObject obj;
    obj.insert(QStringLiteral("Type"), type);
    obj.insert(QStringLiteral("Size"), size);
    if (!measurement.isEmpty()) {
        obj.insert(QStringLiteral("Measurement"), measurement);
    }
    return obj;
}

MenuDynamicAttributes MenuDynamicAttributes::fromJsonObject(const QJsonObject &obj)
{
    MenuDynamicAttributes attrs;
    attrs.type = MenuHelpers::dynamicAttributeValue(obj, QStringLiteral("Type"));
    attrs.size = MenuHelpers::dynamicAttributeValue(obj, QStringLiteral("Size"));
    attrs.measurement = obj.value(QStringLiteral("Measurement")).toString().trimmed();
    if (attrs.measurement.isEmpty()) {
        attrs.measurement = MenuHelpers::dynamicAttributeMeasurement(obj, QStringLiteral("Size"));
    }
    attrs.typePrice = MenuHelpers::dynamicAttributePrice(obj, QStringLiteral("Type"));
    attrs.sizePrice = MenuHelpers::dynamicAttributePrice(obj, QStringLiteral("Size"));
    return attrs;
}

QString MenuDynamicAttributes::displaySize() const
{
    return MenuHelpers::attributeDisplayLabel(size, measurement);
}

MenuDynamicAttributes MenuDish::dynamicAttributes() const
{
    MenuDynamicAttributes attrs;
    attrs.type = attrType;
    attrs.size = attrSize;
    attrs.measurement = attrMeasurement;
    return attrs;
}

namespace MenuHelpers {

QStringList packageAttributeKeys()
{
    return {QStringLiteral("Type"), QStringLiteral("Size")};
}

QJsonObject parseGoodsDataObject(const QJsonValue &vData)
{
    if (vData.isObject()) {
        return vData.toObject();
    }
    const QString raw = vData.toString();
    if (raw.trimmed().isEmpty()) {
        return {};
    }
    const QJsonDocument doc = QJsonDocument::fromJson(raw.toUtf8());
    if (doc.isObject()) {
        return doc.object();
    }
    return {};
}

QString dynamicAttributeValue(const QJsonObject &dynObj, const QString &key)
{
    return dynStr(dynObj.value(key));
}

QString dynamicAttributeMeasurement(const QJsonObject &dynObj, const QString &key)
{
    return dynMeasurement(dynObj, key);
}

double dynamicAttributePrice(const QJsonObject &dynObj, const QString &key)
{
    return dynPrice(dynObj.value(key));
}

QString attributeDisplayLabel(const QString &value, const QString &measurement)
{
    if (value.isEmpty()) {
        return {};
    }
    if (measurement.isEmpty()) {
        return value;
    }
    return value + QLatin1Char(' ') + measurement;
}

MenuDynamicAttributes parseDynamicAttributes(const QJsonObject &dynObj)
{
    return MenuDynamicAttributes::fromJsonObject(dynObj);
}

void applyDietaryAndBju(MenuDish &dish, const QJsonObject &dataObj)
{
    const QJsonObject badgeObj = dataObj.value(QStringLiteral("f_dietary_badge")).toObject();
    dish.glutenFree = badgeObj.value(QStringLiteral("gluten_free")).toBool();
    dish.vegetarian = badgeObj.value(QStringLiteral("vegetarian")).toBool();
    dish.vegan = badgeObj.value(QStringLiteral("vegan")).toBool();
    dish.noGmo = badgeObj.value(QStringLiteral("no_gmo")).toBool();
    dish.noLactose = badgeObj.value(QStringLiteral("no_lactose")).toBool();
    dish.noSugar = badgeObj.value(QStringLiteral("no_sugar")).toBool();
    dish.containsNuts = badgeObj.value(QStringLiteral("contains_nuts")).toBool();
    dish.halalKosher = badgeObj.value(QStringLiteral("halal_kosher")).toBool();

    const QJsonObject bjuObj = dataObj.value(QStringLiteral("f_bju")).toObject();
    dish.kcal = bjuObj.value(QStringLiteral("kcal")).toDouble();
    dish.protein = bjuObj.value(QStringLiteral("protein")).toDouble();
    dish.fat = bjuObj.value(QStringLiteral("fat")).toDouble();
    dish.carbs = bjuObj.value(QStringLiteral("carbs")).toDouble();
}

void applyDietaryAndBjuToComponent(MenuPackageComponent &component, const QJsonObject &dataObj)
{
    const QJsonObject badgeObj = dataObj.value(QStringLiteral("f_dietary_badge")).toObject();
    component.glutenFree = badgeObj.value(QStringLiteral("gluten_free")).toBool();
    component.vegetarian = badgeObj.value(QStringLiteral("vegetarian")).toBool();
    component.vegan = badgeObj.value(QStringLiteral("vegan")).toBool();
    component.noGmo = badgeObj.value(QStringLiteral("no_gmo")).toBool();
    component.noLactose = badgeObj.value(QStringLiteral("no_lactose")).toBool();
    component.noSugar = badgeObj.value(QStringLiteral("no_sugar")).toBool();
    component.containsNuts = badgeObj.value(QStringLiteral("contains_nuts")).toBool();
    component.halalKosher = badgeObj.value(QStringLiteral("halal_kosher")).toBool();

    const QJsonObject bjuObj = dataObj.value(QStringLiteral("f_bju")).toObject();
    component.kcal = bjuObj.value(QStringLiteral("kcal")).toDouble();
    component.protein = bjuObj.value(QStringLiteral("protein")).toDouble();
    component.fat = bjuObj.value(QStringLiteral("fat")).toDouble();
    component.carbs = bjuObj.value(QStringLiteral("carbs")).toDouble();
}

MenuPackageComponent parsePackageComponent(const QJsonObject &row, const QString &imagePath)
{
    MenuPackageComponent component;
    component.goodsId = row.value(QStringLiteral("f_goods")).toInt();
    component.name = row.value(QStringLiteral("f_goods_name")).toString();
    component.price = row.value(QStringLiteral("f_price")).toDouble();
    component.qty = row.value(QStringLiteral("f_qty")).toDouble(1.0);
    component.imagePath = imagePath;

    const QJsonObject dataObj = parseGoodsDataObject(row.value(QStringLiteral("f_data")));
    const QJsonObject dynObj = dataObj.value(QStringLiteral("f_dynamic_attributes")).toObject();
    component.attributes = parseDynamicAttributes(dynObj);
    applyDietaryAndBjuToComponent(component, dataObj);
    return component;
}

void fillDishFromMenuRow(MenuDish &dish, const QJsonObject &row)
{
    dish.id = row.value(QStringLiteral("f_dish")).toInt();
    dish.groupId = row.value(QStringLiteral("f_group")).toInt();
    dish.type = row.value(QStringLiteral("f_type")).toInt(MenuGoods::kTypeGoods);
    dish.name = row.value(QStringLiteral("f_goods_name")).toString();
    dish.groupName = row.value(QStringLiteral("f_group_name")).toString();
    dish.description = row.value(QStringLiteral("f_description")).toString();
    dish.price = row.value(QStringLiteral("f_price")).toDouble();
    dish.store = row.value(QStringLiteral("f_store")).toInt();
    dish.print1 = row.value(QStringLiteral("f_print1")).toString();
    dish.print2 = row.value(QStringLiteral("f_print2")).toString();
    dish.popular = row.value(QStringLiteral("f_recent")).toInt() > 0;

    const QJsonObject dataObj = parseGoodsDataObject(row.value(QStringLiteral("f_data")));
    dish.prepTime = formatCookingTime(dataObj.value(QStringLiteral("f_cooking_time")).toInt());
    dish.countService = dataObj.value(QStringLiteral("f_count_service")).toBool();
    dish.countDiscount = dataObj.value(QStringLiteral("f_count_discount")).toBool();
    applyDietaryAndBju(dish, dataObj);

    const QJsonObject dynObj = dataObj.value(QStringLiteral("f_dynamic_attributes")).toObject();
    dish.attrType = dynamicAttributeValue(dynObj, QStringLiteral("Type"));
    dish.attrSize = dynamicAttributeValue(dynObj, QStringLiteral("Size"));
    dish.attrMeasurement = dynObj.value(QStringLiteral("Measurement")).toString().trimmed();
    if (dish.attrMeasurement.isEmpty()) {
        dish.attrMeasurement = dynamicAttributeMeasurement(dynObj, QStringLiteral("Size"));
    }

    // Package modificators are assigned in MenuCache from the package row only.
    if (!dish.isPackage()) {
        dish.modificators = parseModificators(dataObj);
    } else {
        dish.modificators.clear();
    }
    dish.selectedModificators.clear();
}

const MenuPackageComponent *findPackageComponent(
    const QVector<MenuPackageComponent> &components,
    const QHash<QString, QString> &selectedByKey)
{
    for (const MenuPackageComponent &component : components) {
        if (componentMatchesSelections(component, selectedByKey)) {
            return &component;
        }
    }
    return nullptr;
}

QHash<QString, QStringList> uniqueAttributeOptions(
    const QVector<MenuPackageComponent> &components,
    const QStringList &keys)
{
    QHash<QString, QStringList> result;
    for (const QString &key : keys) {
        QStringList values;
        for (const MenuPackageComponent &component : components) {
            QString value;
            if (key == QStringLiteral("Type")) {
                value = component.attributes.type;
            } else if (key == QStringLiteral("Size")) {
                value = component.attributes.size;
            }
            if (value.isEmpty() || values.contains(value)) {
                continue;
            }
            values.append(value);
        }
        result.insert(key, values);
    }
    return result;
}

double fixedAttributeOptionPrice(
    const QVector<MenuPackageComponent> &components,
    const QString &key,
    const QString &value)
{
    for (const MenuPackageComponent &component : components) {
        if (key == QStringLiteral("Type") && component.attributes.type == value) {
            return component.attributes.typePrice;
        }
        if (key == QStringLiteral("Size") && component.attributes.size == value) {
            return component.attributes.sizePrice;
        }
    }
    return 0.0;
}

double attributeOptionPrice(
    const QVector<MenuPackageComponent> &components,
    const QString &key,
    const QString &value,
    const QHash<QString, QString> &otherSelections)
{
    double found = -1.0;
    for (const MenuPackageComponent &component : components) {
        QString actual;
        double attrPrice = 0.0;
        if (key == QStringLiteral("Type")) {
            actual = component.attributes.type;
            attrPrice = component.attributes.typePrice;
        } else if (key == QStringLiteral("Size")) {
            actual = component.attributes.size;
            attrPrice = component.attributes.sizePrice;
        } else {
            continue;
        }
        if (actual != value) {
            continue;
        }
        if (!componentMatchesSelections(component, otherSelections, key)) {
            continue;
        }
        if (found < 0.0 || attrPrice < found) {
            found = attrPrice;
        }
    }
    return found < 0.0 ? 0.0 : found;
}

double priceForAttributeOption(
    const QVector<MenuPackageComponent> &components,
    const QString &key,
    const QString &value,
    const QHash<QString, QString> &otherSelections)
{
    double minPrice = -1.0;
    for (const MenuPackageComponent &component : components) {
        QString actual;
        if (key == QStringLiteral("Type")) {
            actual = component.attributes.type;
        } else if (key == QStringLiteral("Size")) {
            actual = component.attributes.size;
        }
        if (actual != value) {
            continue;
        }
        if (!componentMatchesSelections(component, otherSelections, key)) {
            continue;
        }
        if (minPrice < 0.0 || component.price < minPrice) {
            minPrice = component.price;
        }
    }
    return minPrice < 0.0 ? 0.0 : minPrice;
}

bool isAttributeOptionAvailable(
    const QVector<MenuPackageComponent> &components,
    const QString &key,
    const QString &value,
    const QHash<QString, QString> &otherSelections)
{
    QHash<QString, QString> sel = otherSelections;
    sel.insert(key, value);
    for (const MenuPackageComponent &component : components) {
        if (componentMatchesSelections(component, sel)) {
            return true;
        }
    }
    return false;
}

bool packageNeedsAttributePicker(const QVector<MenuPackageComponent> &components)
{
    if (components.isEmpty()) {
        return false;
    }
    const QHash<QString, QStringList> options = uniqueAttributeOptions(components, packageAttributeKeys());
    for (const QString &key : packageAttributeKeys()) {
        if (options.value(key).size() > 1) {
            return true;
        }
    }
    return false;
}

double componentAttributePriceSum(const MenuPackageComponent &component)
{
    return component.attributes.typePrice + component.attributes.sizePrice;
}

double packageLinePrice(const MenuDish &package, const MenuPackageComponent &component)
{
    return package.price + componentAttributePriceSum(component);
}

MenuDish resolvePackageCartLine(const MenuDish &package, const MenuPackageComponent *component)
{
    if (!component) {
        return package;
    }

    MenuDish line = package;
    line.id = component->goodsId;
    line.packageId = package.id;
    line.packageName = package.name;
    line.name = package.name;
    line.price = packageLinePrice(package, *component);
    line.imagePath = component->imagePath;
    line.type = MenuGoods::kTypeGoods;
    line.attrType = component->attributes.type;
    line.attrSize = component->attributes.size;
    line.attrMeasurement = component->attributes.measurement;
    // Dietary badges and BJU stay from the package (line = package above).
    line.packageComponents.clear();
    line.modificators.clear();
    line.selectedModificators.clear();
    return line;
}

QVector<MenuModificatorOption> parseModificators(const QJsonObject &dataObj)
{
    QVector<MenuModificatorOption> result;
    const QJsonArray arr = dataObj.value(QStringLiteral("f_modificators")).toArray();
    for (const QJsonValue &value : arr) {
        if (!value.isObject()) {
            continue;
        }
        const QJsonObject item = value.toObject();
        const int id = item.value(QStringLiteral("f_id")).toInt();
        const QString name = item.value(QStringLiteral("f_name")).toString().trimmed();
        if (id <= 0 || name.isEmpty()) {
            continue;
        }
        MenuModificatorOption option;
        option.id = id;
        option.name = name;
        option.price = item.value(QStringLiteral("f_price")).toDouble();
        option.required = item.value(QStringLiteral("f_required")).toBool();
        result.append(option);
    }
    return result;
}

QVector<MenuRelatedItem> parseRelatedItems(const QJsonValue &value)
{
    QVector<MenuRelatedItem> result;
    const QJsonArray arr = value.isArray() ? value.toArray() : QJsonArray();
    for (const QJsonValue &entry : arr) {
        if (!entry.isObject()) {
            continue;
        }
        const QJsonObject item = entry.toObject();
        const int id = item.value(QStringLiteral("f_id")).toInt();
        const QString name = item.value(QStringLiteral("f_name")).toString().trimmed();
        if (id <= 0) {
            continue;
        }
        MenuRelatedItem related;
        related.id = id;
        related.name = name;
        result.append(related);
    }
    return result;
}

QVector<MenuModificatorOption> packageCatalogModificators(const MenuDish &dish)
{
    if (!dish.isPackage()) {
        return {};
    }
    return dish.modificators;
}

QVector<MenuModificatorOption> pickerModificators(const MenuDish &dish)
{
    if (dish.isPackage()) {
        return packageCatalogModificators(dish);
    }
    return dish.modificators;
}

double selectedModificatorsPrice(const MenuDish &dish)
{
    double sum = 0.0;
    for (const MenuSelectedModificator &modificator : dish.selectedModificators) {
        sum += modificator.price;
    }
    return sum;
}

void applySelectedModificators(MenuDish &dish, const QVector<MenuSelectedModificator> &selected)
{
    const double basePrice = dish.price - selectedModificatorsPrice(dish);
    dish.selectedModificators = selected;
    dish.price = basePrice + selectedModificatorsPrice(dish);
}

QString cartLineKey(const MenuDish &dish)
{
    QStringList parts;
    parts << QString::number(dish.id);
    if (dish.packageId > 0) {
        parts << QStringLiteral("pkg") << QString::number(dish.packageId);
        parts << dish.attrType << dish.attrSize;
    }
    QStringList modIds;
    for (const MenuSelectedModificator &modificator : dish.selectedModificators) {
        modIds << QString::number(modificator.id);
    }
    modIds.sort();
    if (!modIds.isEmpty()) {
        parts << modIds.join(QLatin1Char(','));
    }
    return parts.join(QLatin1Char(':'));
}

bool dishNeedsOptionsPicker(const MenuDish &dish)
{
    if (dish.isPackage()) {
        if (!dish.relatedDrinks.isEmpty() || !dish.relatedOther.isEmpty()) {
            return true;
        }
        if (!packageCatalogModificators(dish).isEmpty()) {
            return true;
        }
        return packageNeedsAttributePicker(dish.packageComponents);
    }
    return !dish.modificators.isEmpty();
}

bool dishHasThreeStepPicker(const MenuDish &dish)
{
    return dish.isPackage()
        && !dish.relatedDrinks.isEmpty()
        && !dish.relatedOther.isEmpty();
}

} // namespace MenuHelpers

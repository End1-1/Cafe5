#ifndef MENUHELPERS_H
#define MENUHELPERS_H

#include "menutypes.h"

#include <QHash>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>

namespace MenuHelpers {

QStringList packageAttributeKeys();

QJsonObject parseGoodsDataObject(const QJsonValue &vData);

QString dynamicAttributeValue(const QJsonObject &dynObj, const QString &key);
QString dynamicAttributeMeasurement(const QJsonObject &dynObj, const QString &key);
double dynamicAttributePrice(const QJsonObject &dynObj, const QString &key);
QString attributeDisplayLabel(const QString &value, const QString &measurement);

MenuDynamicAttributes parseDynamicAttributes(const QJsonObject &dynObj);
void applyDietaryAndBju(MenuDish &dish, const QJsonObject &dataObj);
void applyDietaryAndBjuToComponent(MenuPackageComponent &component, const QJsonObject &dataObj);

MenuPackageComponent parsePackageComponent(const QJsonObject &row, const QString &imagePath);
void fillDishFromMenuRow(MenuDish &dish, const QJsonObject &row);

const MenuPackageComponent *findPackageComponent(
    const QVector<MenuPackageComponent> &components,
    const QHash<QString, QString> &selectedByKey);

QHash<QString, QStringList> uniqueAttributeOptions(
    const QVector<MenuPackageComponent> &components,
    const QStringList &keys);

double priceForAttributeOption(
    const QVector<MenuPackageComponent> &components,
    const QString &key,
    const QString &value,
    const QHash<QString, QString> &otherSelections);

/** Price supplement from f_dynamic_attributes (Type/Size .price), 0 if unset. */
double attributeOptionPrice(
    const QVector<MenuPackageComponent> &components,
    const QString &key,
    const QString &value,
    const QHash<QString, QString> &otherSelections);

/** First matching row's attribute price (ignores other attribute selections). */
double fixedAttributeOptionPrice(
    const QVector<MenuPackageComponent> &components,
    const QString &key,
    const QString &value);

bool isAttributeOptionAvailable(
    const QVector<MenuPackageComponent> &components,
    const QString &key,
    const QString &value,
    const QHash<QString, QString> &otherSelections);

/** True when the user must pick Type/Size (more than one distinct value on any axis). */
bool packageNeedsAttributePicker(const QVector<MenuPackageComponent> &components);

/** Sum of Type/Size price supplements on a complectation row. */
double componentAttributePriceSum(const MenuPackageComponent &component);

/** Package menu price + attribute supplements for the selected component. */
double packageLinePrice(const MenuDish &package, const MenuPackageComponent &component);

/** Build cart line from package + selected component (component=null → package menu row as-is). */
MenuDish resolvePackageCartLine(const MenuDish &package, const MenuPackageComponent *component);

QVector<MenuModificatorOption> parseModificators(const QJsonObject &dataObj);

QVector<MenuRelatedItem> parseRelatedItems(const QJsonValue &value);

/** Package row only — never merged from complectation components. */
QVector<MenuModificatorOption> packageCatalogModificators(const MenuDish &dish);

/** Modificators shown in picker: package catalog for packages, dish catalog otherwise. */
QVector<MenuModificatorOption> pickerModificators(const MenuDish &dish);

void applySelectedModificators(MenuDish &dish, const QVector<MenuSelectedModificator> &selected);
double selectedModificatorsPrice(const MenuDish &dish);
QString cartLineKey(const MenuDish &dish);

/** Package attribute picker and/or modificator picker. */
bool dishNeedsOptionsPicker(const MenuDish &dish);

/** Package with Personalize + Drink + Extra steps (related drinks and extras). */
bool dishHasThreeStepPicker(const MenuDish &dish);

} // namespace MenuHelpers

#endif // MENUHELPERS_H

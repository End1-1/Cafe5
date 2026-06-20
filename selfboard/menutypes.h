#ifndef MENUTYPES_H
#define MENUTYPES_H

#include <QJsonObject>
#include <QString>
#include <QVector>

namespace MenuLayout {
constexpr int kGridCardWidth = 376;
constexpr int kGridCardHeight = 186;
constexpr int kPopularCardWidth = 236;
constexpr int kPopularCardHeight = 377;
constexpr int kGridColumns = 2;
constexpr int kGridRows = 3;
constexpr int kGridSpacing = 16;
constexpr int kGridMaxItems = kGridColumns * kGridRows;

constexpr int kListingColumns = 3;
constexpr int kListingCardWidth = 236;
constexpr int kListingCardHeight = 377;
constexpr int kListingSpacing = 16;

inline int gridAreaHeight()
{
    return kGridRows * kGridCardHeight + (kGridRows - 1) * kGridSpacing;
}
} // namespace MenuLayout

namespace MenuGoods {
constexpr int kTypeGoods = 1;
constexpr int kTypeBeer = 2;
constexpr int kTypePackage = 5;
} // namespace MenuGoods

struct MenuDynamicAttributes
{
    QString type;
    QString size;
    QString measurement;
    double typePrice = 0.0;
    double sizePrice = 0.0;

    QJsonObject toJsonObject() const;
    static MenuDynamicAttributes fromJsonObject(const QJsonObject &obj);
    QString displaySize() const;
};

struct MenuModificatorOption
{
    int id = 0;
    QString name;
    double price = 0.0;
    bool required = false;
};

struct MenuSelectedModificator
{
    int id = 0;
    QString name;
    double price = 0.0;
};

/** c_goods.f_data.f_related_drink / f_related_other — dish id from c_menu (f_dish). */
struct MenuRelatedItem
{
    int id = 0;
    QString name;
};

struct MenuPackageComponent
{
    int goodsId = 0;
    QString name;
    double price = 0.0;
    double qty = 1.0;
    QString imagePath;
    MenuDynamicAttributes attributes;

    bool glutenFree = false;
    bool vegetarian = false;
    bool vegan = false;
    bool noGmo = false;
    bool noLactose = false;
    bool noSugar = false;
    bool containsNuts = false;
    bool halalKosher = false;

    double kcal = 0.0;
    double protein = 0.0;
    double fat = 0.0;
    double carbs = 0.0;
};

struct MenuGroup
{
    int id = 0;
    QString name;
    QString iconPath;
};

struct MenuDish
{
    int id = 0;
    int groupId = 0;
    int type = MenuGoods::kTypeGoods;
    QString name;
    QString groupName;
    QString description;
    QString imagePath;
    double price = 0.0;
    int store = 0;
    QString print1;
    QString print2;
    bool countService = false;
    bool countDiscount = false;
    QString prepTime;
    bool popular = false;

    int packageId = 0;
    QString packageName;

    QVector<MenuPackageComponent> packageComponents;

    // c_goods.f_data.f_dietary_badge
    bool glutenFree = false;
    bool vegetarian = false;
    bool vegan = false;
    bool noGmo = false;
    bool noLactose = false;
    bool noSugar = false;
    bool containsNuts = false;
    bool halalKosher = false;

    // c_goods.f_data.f_bju (per 100 g)
    double kcal = 0.0;
    double protein = 0.0;
    double fat = 0.0;
    double carbs = 0.0;

    // c_goods.f_data.f_dynamic_attributes (simple goods or cart selection)
    QString attrType;
    QString attrSize;
    QString attrMeasurement;

    // c_goods.f_data.f_modificators (catalog options on the dish/package)
    QVector<MenuModificatorOption> modificators;
    // Selected options for a cart / order line
    QVector<MenuSelectedModificator> selectedModificators;

    QVector<MenuRelatedItem> relatedDrinks;
    QVector<MenuRelatedItem> relatedOther;

    bool isPackage() const { return type == MenuGoods::kTypePackage; }
    MenuDynamicAttributes dynamicAttributes() const;
};

#endif // MENUTYPES_H

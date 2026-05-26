#ifndef MENUTYPES_H
#define MENUTYPES_H

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
    QString name;
    QString groupName;
    QString imagePath;
    double price = 0.0;
    QString prepTime;
};

#endif // MENUTYPES_H

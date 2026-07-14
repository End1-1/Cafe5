#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QWidget>

class KitchenFineItemRow;
class QPushButton;
class QLabel;
class QScrollArea;
class QVBoxLayout;

enum class KitchenFineCardColor {
    Blue = 1,
    Yellow = 2,
    Green = 3,
    Overdue = 4,
};

constexpr int kKitchenFineCardWidth = 310;

class KitchenFineOrderCard : public QWidget
{
    Q_OBJECT

public:
    explicit KitchenFineOrderCard(QWidget *parent = nullptr);

    void setOrderData(const QJsonObject &order,
                      const QJsonArray &lines,
                      KitchenFineCardColor color,
                      const QString &statusButtonText,
                      bool readonly);

    const QJsonObject &order() const { return mOrder; }

    KitchenFineCardColor cardColor() const { return mColor; }

    void updateColor(KitchenFineCardColor color, const QString &statusButtonText);

signals:
    void headerClicked();
    void statusButtonClicked();
    void lineClicked(const QString &lineId);

private:
    void applyColorTheme(KitchenFineCardColor color);

    QJsonObject mOrder;
    KitchenFineCardColor mColor = KitchenFineCardColor::Blue;
    bool mReadonly = false;

    QLabel *mLbOrderPrefix = nullptr;
    QLabel *mLbTime = nullptr;
    QWidget *mHeader = nullptr;
    QScrollArea *mScrollItems = nullptr;
    QVBoxLayout *mItemsLayout = nullptr;
    QPushButton *mBtnStatus = nullptr;
    QVector<KitchenFineItemRow *> mItemRows;
};

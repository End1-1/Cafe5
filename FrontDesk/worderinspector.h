#pragma once

#include <QJsonObject>
#include "c5officewidget.h"
#include "struct_waiter_order.h"

namespace Ui
{
class WOrderInspector;
}

class WOrderInspector : public C5OfficeWidget
{
    Q_OBJECT
public:
    explicit WOrderInspector(C5User *user, const QString &title, QIcon icon, QWidget *parent = nullptr);

    ~WOrderInspector() override;

    void setOrder(const WaiterOrder &order);

    void setOrderFromJson(const QJsonObject &jo);

private slots:
    void dishSelectionChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void copyOrderIdToClipboard();

    void calcQueueCellChanged(int row, int column);

    void dishCellDoubleClicked(int row, int column);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::WOrderInspector *ui = nullptr;

    WaiterOrder mOrder;

    static QString toPrettyJson(const QJsonObject &obj);

    static QString toPrettyJson(const QJsonValue &value);

    static QString orderStateName(int state);

    void fillHeader();

    void fillDishes();

    void fillLog();

    void fillCalcQueue();

    void updateCalcQueueSumRow();

    void refreshCalcQueueRowTotal(int row);

    bool mCalcQueueFilling = false;

    static constexpr int calcQueueColCost = 4;

    static constexpr int calcQueueColTotal = 5;

    static constexpr int dishColAppendTime = 9;

    static constexpr int dishColPrintTime = 10;

    static constexpr int dishColKitchen = 11;
};

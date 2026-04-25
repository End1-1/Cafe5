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

private:
    Ui::WOrderInspector *ui = nullptr;

    WaiterOrder mOrder;

    static QString toPrettyJson(const QJsonObject &obj);

    static QString toPrettyJson(const QJsonValue &value);

    static QString orderStateName(int state);

    void fillHeader();

    void fillDishes();
};

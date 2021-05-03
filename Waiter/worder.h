#ifndef WORDER_H
#define WORDER_H

#include <QWidget>

namespace Ui {
class WOrder;
}

class C5WaiterOrderDoc;

class WOrder : public QWidget
{
    Q_OBJECT

public:
    explicit WOrder(QWidget *parent = nullptr);

    ~WOrder();

    void config(C5WaiterOrderDoc *order);

    void itemsToTable(int guest);

    void setOrder(C5WaiterOrderDoc *order, int guest);

    void addItem(const QJsonObject &o);

    void updateItem(const QJsonObject &o, int index);

    int rowCount();

    bool currentRow(int &row);

    void updatePayment(int guest);

private:
    Ui::WOrder *ui;

    C5WaiterOrderDoc *fOrder;

    int fGuest;
};

#endif // WORDER_H

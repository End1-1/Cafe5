#ifndef C5STOREINVENTORY_H
#define C5STOREINVENTORY_H

#include <QWidget>

namespace Ui {
class C5StoreInventory;
}

class C5StoreInventory : public QWidget
{
    Q_OBJECT

public:
    explicit C5StoreInventory(QWidget *parent = 0);
    ~C5StoreInventory();

private:
    Ui::C5StoreInventory *ui;
};

#endif // C5STOREINVENTORY_H

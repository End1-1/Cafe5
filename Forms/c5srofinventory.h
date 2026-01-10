#ifndef C5SROFINVENTORY_H
#define C5SROFINVENTORY_H

#include "c5dialog.h"

namespace Ui
{
class C5SrOfInventory;
}

class C5SrOfInventory : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5SrOfInventory(C5User *user);
    ~C5SrOfInventory();
    void setGoods(const QDate &date, int store, int goods);
    QString fUuid;

private slots:
    void on_btnDelete_clicked();

private:
    Ui::C5SrOfInventory* ui;
};

#endif // C5SROFINVENTORY_H

#ifndef SESSIONORDERS_H
#define SESSIONORDERS_H

#include "c5dialog.h"

namespace Ui {
class SessionOrders;
}

class SessionOrders : public C5Dialog
{
    Q_OBJECT

public:
    explicit SessionOrders(C5User *user);
    ~SessionOrders();

private slots:
    void on_btnClose_clicked();

private:
    Ui::SessionOrders *ui;

    void loadOrders();
};

#endif // SESSIONORDERS_H

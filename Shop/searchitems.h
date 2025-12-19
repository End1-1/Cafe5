#ifndef SEARCHITEMS_H
#define SEARCHITEMS_H

#include "c5shopdialog.h"

namespace Ui
{
class SearchItems;
}

class SearchItems : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit SearchItems(C5User *user);

    ~SearchItems();

private slots:
    void on_btnSearch_clicked();

    void on_leCode_returnPressed();

    void on_btnReserve_clicked();

    void on_btnViewReservations_clicked();

    void on_btnViewAllReservations_clicked();

    void on_btnEditReserve_clicked();

    void on_btnExit_clicked();

private:
    Ui::SearchItems* ui;
};

#endif // SEARCHITEMS_H

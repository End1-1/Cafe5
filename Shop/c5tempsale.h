#ifndef C5TEMPSALE_H
#define C5TEMPSALE_H

#include "c5dialog.h"

namespace Ui {
class C5TempSale;
}

class WOrder;
class QTableWidgetItem;

class C5TempSale : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5TempSale();
    ~C5TempSale();
    void openDraft(WOrder *o);

private slots:
    void on_tbl_itemDoubleClicked(QTableWidgetItem *item);

    void on_btnExit_clicked();

    void on_btnOpen_clicked();

    void on_btnTrash_clicked();

private:
    Ui::C5TempSale *ui;
    void refreshData();
};

#endif // C5TEMPSALE_H

#ifndef DLGSMARTREPORTS_H
#define DLGSMARTREPORTS_H

#include "c5dialog.h"

namespace Ui {
class DlgSmartReports;
}

class QListWidgetItem;

class DlgSmartReports : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSmartReports();
    ~DlgSmartReports();

private slots:
    void on_list_itemClicked(QListWidgetItem *item);

private:
    Ui::DlgSmartReports *ui;

    void reportCommonDishes();

    void reportCommonDishesWithTime();

    void printDeliveryReport();
};

#endif // DLGSMARTREPORTS_H

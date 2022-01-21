#ifndef VIEWORDER_H
#define VIEWORDER_H

#include "c5dialog.h"

namespace Ui {
class ViewOrder;
}

class C5Database;

class ViewOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit ViewOrder(const QString &order);

    ~ViewOrder();

private slots:
    void on_btnReturn_clicked();

    void on_pushButton_2_clicked();

    void on_btnRetryUpload_clicked();

    void on_btnSave_clicked();

    void on_btnTaxReturn_clicked();

private:
    Ui::ViewOrder *ui;

    int fSaleType;

    int fPartner;

    QString fSaleDoc;

    QString fUuid;

    void returnFalse(const QString &msg, C5Database *db);
};

#endif // VIEWORDER_H

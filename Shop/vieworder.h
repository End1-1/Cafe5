#ifndef VIEWORDER_H
#define VIEWORDER_H

#include <QDialog>

namespace Ui {
class ViewOrder;
}

class C5Database;

class ViewOrder : public QDialog
{
    Q_OBJECT

public:
    explicit ViewOrder(const QString &order, QWidget *parent = nullptr);

    ~ViewOrder();

private slots:
    void on_btnReturn_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ViewOrder *ui;

    int fSaleType;

    int fPartner;

    QString fSaleDoc;

    void returnFalse(const QString &msg, C5Database *db);
};

#endif // VIEWORDER_H

#ifndef DLGORDER_H
#define DLGORDER_H

#include "c5dialog.h"
#include <QTableWidgetItem>

namespace Ui {
class DlgOrder;
}

class C5User;
class C5Order;

class DlgOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgOrder(QWidget *parent = 0);

    ~DlgOrder();

    static void openTable(const QJsonObject &table, C5User *user);

protected:
    virtual void accept();

    virtual void reject();

private:
    Ui::DlgOrder *ui;

    C5User *fUser;

    C5Order *fOrder;

    QString fMenuName;

    QString fPart1Name;

    void load(int table);

    void buildMenu(const QString &menu, QString part1, QString part2);

    void addDishToOrder(const QJsonObject &obj);

    void loadOrder(const QJsonObject &obj);

    void changeQty(double qty);

    void itemsToTable();

private slots:
    void handleOpenTable(const QJsonObject &obj);

    void handlePrintService(const QJsonObject &obj);

    void saveAndQuit(const QJsonObject &obj);

    void handleGoPayment(const QJsonObject &obj);

    virtual void handleError(int err, const QString &msg);

    void on_tblPart1_itemClicked(QTableWidgetItem *item);

    void on_tblPart2_itemClicked(QTableWidgetItem *item);

    void on_tblDishes_itemClicked(QTableWidgetItem *item);

    void on_btnPlus1_clicked();

    void on_btnMin1_clicked();

    void on_btnPlus05_clicked();

    void on_btnMin05_clicked();

    void on_btnCustom_clicked();

    void on_btnPrintService_clicked();

    void on_btnPayment_clicked();

    void on_btnExit_clicked();
};

#endif // DLGORDER_H

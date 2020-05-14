#ifndef C5WELCOMEPAGE_H
#define C5WELCOMEPAGE_H

#include "c5widget.h"

namespace Ui {
class C5WelcomePage;
}

class C5WelcomePage : public C5Widget
{
    Q_OBJECT

public:
    explicit C5WelcomePage(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5WelcomePage();

    void setDatabaseName(const QString &database);

private slots:
    void on_btnNewStoreInput_clicked();

    void on_btnNewStoreOutput_clicked();

    void on_btnNewStoreMovement_clicked();

    void on_btnNewStoreInventory_clicked();

    void on_btnDocs_clicked();

    void on_btnMaterialsInStore_clicked();

    void on_btnMovementInStore_clicked();

    void on_btnTStoreExtra_clicked();

    void on_btnSalesCommon_clicked();

    void on_btnDatabase_clicked();

    void on_btnStoreDocs_clicked();

    void on_btnStorages_clicked();

    void on_btnGoodsGroups_clicked();

    void on_btnGoods_clicked();

    void on_btnUnits_clicked();

    void on_btnMenuName_clicked();

    void on_btnDishDepts_clicked();

    void on_btnTypesOfDishes_clicked();

    void on_btnDishes_clicked();

    void on_btnNewComplectation_clicked();

    void on_btnNewStoreDecomplectation_clicked();

    void on_btnSaleOfItems_clicked();

    void on_btnNewCashDocument_clicked();

    void on_btnCashMovement_clicked();

    void on_btnSaleDishes_clicked();

private:
    Ui::C5WelcomePage *ui;

    QString fDbName;
};

#endif // C5WELCOMEPAGE_H

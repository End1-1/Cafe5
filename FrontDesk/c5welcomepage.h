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
    explicit C5WelcomePage(const QStringList &dbParams, QWidget *parent = 0);

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

    void on_btnGoodsOutputBySales_clicked();

    void on_btnSalesCommon_clicked();

    void on_btnDatabase_clicked();

private:
    Ui::C5WelcomePage *ui;

    QString fDbName;
};

#endif // C5WELCOMEPAGE_H

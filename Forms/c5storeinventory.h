#ifndef C5STOREINVENTORY_H
#define C5STOREINVENTORY_H

#include "c5officewidget.h"

namespace Ui
{
class C5StoreInventory;
}

class C5StoreInventory : public C5OfficeWidget
{
    Q_OBJECT

public:
    explicit C5StoreInventory(QWidget *parent = nullptr);

    ~C5StoreInventory();

    virtual QToolBar* toolBar();

    bool openDoc(QString id);

    static bool removeDoc(QString id);

    virtual bool allowChangeDatabase();

private slots:
    void keyShortcut();

    void keyPressed(const QChar &c);

    void saveDoc();

    void printDoc();

    void on_btnAddGoods_clicked();

    void on_btnRemoveGoods_clicked();

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void on_btnNew_clicked();

    void on_leSearch_textChanged(const QString &arg1);

    void on_btnCloseSearch_clicked();

    void on_leFind_textChanged(const QString &arg1);

    void on_btnBroadcast_clicked(bool checked);

private:
    Ui::C5StoreInventory* ui;

    QString fInternalID;

    int addGoodsRow();

    void countTotal();

    bool docCheck(QString &err);
};

#endif // C5STOREINVENTORY_H

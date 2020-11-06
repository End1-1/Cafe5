#ifndef C5STOREINVENTORY_H
#define C5STOREINVENTORY_H

#include "c5widget.h"

namespace Ui {
class C5StoreInventory;
}

class C5StoreInventory : public C5Widget
{
    Q_OBJECT

public:
    explicit C5StoreInventory(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5StoreInventory();

    virtual QToolBar *toolBar();

    bool openDoc(QString id);

    static bool removeDoc(const QStringList &dbParams, QString id);

    virtual bool allowChangeDatabase();

    virtual bool event(QEvent *e) override;

private slots:
    void keyPressed(const QChar &c);

    void saveDoc();

    void printDoc();

    void on_btnAddGoods_clicked();

    void on_btnRemoveGoods_clicked();

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void on_btnNew_clicked();

private:
    Ui::C5StoreInventory *ui;

    QString fInternalID;

    int addGoodsRow();

    void countTotal();

    bool docCheck(QString &err);
};

#endif // C5STOREINVENTORY_H

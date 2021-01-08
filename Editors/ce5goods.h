#ifndef CE5GOODS_H
#define CE5GOODS_H

#include "ce5editor.h"

namespace Ui {
class CE5Goods;
}

class Barcode;

class CE5Goods : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Goods(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5Goods();

    virtual QString title();

    virtual QString table();

    virtual QString dbError(QString err);

    virtual void setId(int id);

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

    virtual void clear();

    virtual QPushButton *b1() override;

private slots:
    void printCard();

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void uploadImage();

    void removeImage();

    void newScancode();

    void removeScancode();

    void tblMultiscancodeContextMenu(const QPoint &p);

    void on_btnNewGroup_clicked();

    void on_btnNewUnit_clicked();

    void on_btnAddGoods_clicked();

    void on_btnRemoveGoods_clicked();

    void on_btnNewGoods_clicked();

    void on_btnNewPartner_clicked();

    void on_btnNewGroup1_clicked();

    void on_btnNewGroup2_clicked();

    void on_btnNewGroup3_clicked();

    void on_btnNewGroup4_clicked();

    void on_btnCopy_clicked();

    void on_btnPaste_clicked();

    void on_lbImage_customContextMenuRequested(const QPoint &pos);

    void on_tabWidget_currentChanged(int index);

    void on_leScanCode_textChanged(const QString &arg1);

    void on_btnSetControlSum_clicked();

    void on_chUncomplectIfZero_clicked(bool checked);

    void on_btnPinLast_clicked(bool checked);

    void on_chSameStoreId_clicked();

private:
    Ui::CE5Goods *ui;

    int addGoodsRow();

    void countTotal();

    QSet<QString> fStrings;

    Barcode *fBarcode;

    QStringList fScancodeRemove;

    QStringList fScancodeAppend;
};

#endif // CE5GOODS_H

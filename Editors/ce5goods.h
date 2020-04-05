#ifndef CE5GOODS_H
#define CE5GOODS_H

#include "ce5editor.h"

namespace Ui {
class CE5Goods;
}

class C5Printing;

class PrintBarcodeWidget : public QWidget {
    // QWidget interface
public:
    PrintBarcodeWidget(C5Printing *prn, QWidget *parent = nullptr);
protected:
    C5Printing *print;
    virtual void paintEvent(QPaintEvent *event) override;
};

class CE5Goods : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Goods(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5Goods();

    virtual QString title();

    virtual QString table();

    virtual void setId(int id);

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

    virtual void clear();

private slots:
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

    void on_btnPrintScancode_clicked();

private:
    Ui::CE5Goods *ui;

    PrintBarcodeWidget *fPbw;

    C5Printing *fPrinting;

    int addGoodsRow();

    void countTotal();
};

#endif // CE5GOODS_H

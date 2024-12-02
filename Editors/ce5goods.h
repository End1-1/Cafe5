#ifndef CE5GOODS_H
#define CE5GOODS_H

#include "ce5editor.h"

namespace Ui
{
class CE5Goods;
}

class Barcode;

class CE5Goods : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Goods(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5Goods();

    virtual QString title() override;

    virtual QString table() override;

    virtual QString dbError(QString err) override;

    virtual void setId(int id) override;

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data) override;

    virtual void clear() override;

    virtual QPushButton *b1() override;

    virtual QJsonObject makeJsonObject() override;

    virtual bool acceptOnSave() const override;

    virtual bool isOnline() override;

private slots:
    void saveResponse(const QJsonObject &jdoc);

    void openResponse(const QJsonObject &jdoc);

    void printCard();

    void priceEdited(const QString &arg1);

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void uploadImage();

    void removeImage();

    void genScancode();

    void on_btnNewGroup_clicked();

    void on_btnNewUnit_clicked();

    void on_btnAddGoods_clicked();

    void on_btnRemoveGoods_clicked();

    void on_btnNewGoods_clicked();

    void on_btnNewPartner_clicked();

    void on_btnCopy_clicked();

    void on_btnPaste_clicked();

    void on_lbImage_customContextMenuRequested(const QPoint &pos);

    void on_tabWidget_currentChanged(int index);

    void on_leScanCode_textChanged(const QString &arg1);

    void on_btnSetControlSum_clicked();

    void on_btnPinLast_clicked(bool checked);

    void on_chSameStoreId_clicked();

    void on_leUnitName_textChanged(const QString &arg1);

    void on_btnPrintBarcode_clicked();

    void on_leCostPrice_textEdited(const QString &arg1);

    void on_leMargin_textEdited(const QString &arg1);

    void on_leMargin2_textEdited(const QString &arg1);

    void on_btnNewModel_clicked();

    void on_leScanCode_returnPressed();

    void on_leBarcode_returnPressed();

private:
    Ui::CE5Goods *ui;

    int addGoodsRow();

    void countTotal();

    QSet<QString> fStrings;

    QString fImage;

    bool fScancodeGenerated;

    Barcode *fBarcode;

    QMap<QString, double> fCrossRate;

    void setComplectFlag();

    void countSalePrice(int r, double margin);
};

#endif // CE5GOODS_H

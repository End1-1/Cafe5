#ifndef CE5MFPRODUCT_H
#define CE5MFPRODUCT_H

#include "ce5editor.h"

namespace Ui
{
class CE5MFProduct;
}

class CE5MFProduct : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5MFProduct(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5MFProduct();

    virtual QString title();

    virtual QString table();

    virtual void setId(int id);

    virtual void clear();

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

private slots:
    void openResponse(const QJsonObject &jdoc);

    void saveResponse(const QJsonObject &jdoc);

    void startPriceUpdateOnRow();

    void durationChanged(const QString &arg1);

    void goalPriceChanged(const QString &arg1);

    void on_btnAdd_clicked();

    void on_btnMinus_clicked();

    void on_btnNew_clicked();

    void on_btnPrint_clicked();

    void on_btnClear_clicked();

    void on_btnMoveRowUp_clicked();

    void on_btnMoveRowDown_clicked();

    void on_btnUpdatePrices_clicked();

    void on_chUpdatePrice_clicked(bool checked);

    void on_btnCopy_clicked();

    void on_btnPaste_clicked();

    void on_btnExportExcel_clicked();

    void on_btnAdd_2_clicked();

    void on_btnAddImage_clicked();

    void on_btnAdd_3_clicked();

    void on_btnMinus_2_clicked();

    void on_btnClear_2_clicked();

    void on_btnMinus_3_clicked();

    void on_btnClearTblMaterials_clicked();

    void on_btnCopy_2_clicked();

    void on_btnPaste_2_clicked();

    void on_btnCopy_3_clicked();

    void on_btnPaste_3_clicked();

private:
    Ui::CE5MFProduct *ui;

    QString durationStr(int sec);

    QList<int> fRemovedRow;
};

#endif // CE5MFPRODUCT_H

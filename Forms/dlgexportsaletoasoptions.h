#ifndef DLGEXPORTSALETOASOPTIONS_H
#define DLGEXPORTSALETOASOPTIONS_H

#include "c5dialog.h"

namespace Ui {
class DlgExportSaleToAsOptions;
}

class DlgExportSaleToAsOptions : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgExportSaleToAsOptions();

    ~DlgExportSaleToAsOptions();

    static int getOption(QString &partner, QString &store, QString &service, QString &srvinacc, QString &srvoutacc, QString &iteminacc, QString &itemoutacc, QString &bankacc, QString &vat, bool &exportSimple, QString &simpleItem);

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

    void on_lePartnerCode_textEdited(const QString &arg1);

    void on_leStoreCode_textEdited(const QString &arg1);

    void on_leServiceItem_textEdited(const QString &arg1);

    void on_leServiceOutAccount_textEdited(const QString &arg1);

    void on_leServiceInAccount_textEdited(const QString &arg1);

    void on_leItemOutAccount_textEdited(const QString &arg1);

    void on_leItemInAccount_textEdited(const QString &arg1);

    void on_leBankAcc_textEdited(const QString &arg1);

    void on_leVATValue_textEdited(const QString &arg1);

    void on_chExportSimple_clicked(bool checked);

    void on_leSimpleItem_textChanged(const QString &arg1);

private:
    Ui::DlgExportSaleToAsOptions *ui;
};

#endif // DLGEXPORTSALETOASOPTIONS_H

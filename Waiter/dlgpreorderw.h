#ifndef DLGPREORDERW_H
#define DLGPREORDERW_H

#include "c5dialog.h"

class C5OrderDriver;

namespace Ui {
class DlgPreorder;
}

class DlgPreorder : public C5Dialog
{
    Q_OBJECT

public:
    const static int RESULT_OK = 1;

    const static int RESULT_CANCELED = 2;

    explicit DlgPreorder(C5OrderDriver *w, C5User *user, const QStringList &dbParams);

    DlgPreorder &setHotelMode(bool v);

    ~DlgPreorder();

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

    void on_btnDatePopup_clicked();

    void on_btnTimeUp_clicked();

    void on_btnTimeDown_clicked();

    void on_btnDateUp_clicked();

    void on_btnDateDown_clicked();

    void on_btnGuestUp_clicked();

    void on_btnGuestDown_clicked();

    void on_btnEditCash_clicked();

    void on_btnEditCard_clicked();

    void on_btnEditPayX_clicked();

    void on_btnEditGuestname_clicked();

    void on_btnEditCLCode_clicked();

    void on_btnCancelReserve_clicked();

    void on_btnSelectTable_clicked();

    void on_leDateCheckout_dateChanged(const QDate &date);

    void on_leDate_dateChanged(const QDate &date);

    void on_leDays_valueChanged(int arg1);

    void on_btnCheckDown_clicked();

    void on_btnCheckoutUp_clicked();

    void on_btnEditRate_clicked();

    void on_btnCheckin_clicked();

    void on_btnPrintPrepaymentTax_clicked();

private:
    Ui::DlgPreorder *ui;

    C5OrderDriver *fOrder;

    C5User *fUser;

    bool fHotelMode;

    int fGuestCode;
};

#endif // DLGPREORDERW_H

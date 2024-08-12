#ifndef DLGPREORDERW_H
#define DLGPREORDERW_H

#include "c5dialog.h"

class C5OrderDriver;

namespace Ui
{
class DlgPreorder;
}

class DlgPreorder : public C5Dialog
{
    Q_OBJECT

public:
    const static int RESULT_OK = 1;

    const static int RESULT_CANCELED = 2;

    explicit DlgPreorder(const QJsonObject &jdoc);

    DlgPreorder &setHotelMode(bool v);

    ~DlgPreorder();

private slots:
    void paymentComboboxIndexChanged(int index);

    void openReservationResponse(const QJsonObject &jdoc);

    void checkinResponse(const QJsonObject &jdoc);

    void cancelResponse(const QJsonObject &jdoc);

    void saveResponse(const QJsonObject &jdoc);

    void on_btnCancel_clicked();

    void on_btnOK_clicked();

    void on_btnEditGuestname_clicked();

    void on_btnCancelReserve_clicked();

    void on_btnSelectTable_clicked();

    void on_leDateCheckout_dateChanged(const QDate &date);

    void on_leDate_dateChanged(const QDate &date);

    void on_leDays_valueChanged(int arg1);

    void on_btnCheckin_clicked();

    void on_btnPrintReservation_clicked();

    void on_tblPayment_cellClicked(int row, int column);

    void on_btnCopyID_clicked();

private:
    Ui::DlgPreorder *ui;

    QJsonObject fDoc;

    bool fHotelMode;

    void openDoc();

    void save(bool withcheckin);

    int fPrevRow;

    int fPrevColumn;

    void setVerticalHeaders();

    void setState();
};

#endif // DLGPREORDERW_H

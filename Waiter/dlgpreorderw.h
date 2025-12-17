#ifndef DLGPREORDERW_H
#define DLGPREORDERW_H

#include "c5waiterdialog.h"
#include <QJsonObject>

class C5OrderDriver;

namespace Ui
{
class DlgPreorder;
}

class DlgPreorder : public C5WaiterDialog
{
    Q_OBJECT

public:
    const static int RESULT_OK = 1;

    const static int RESULT_CANCELED = 2;

    explicit DlgPreorder(C5User *user, const QJsonObject &jdoc);

    ~DlgPreorder();

private slots:

    void openReservationResponse(const QJsonObject &jdoc);

    void checkinResponse(const QJsonObject &jdoc);

    void cancelResponse(const QJsonObject &jdoc);

    void saveResponse(const QJsonObject &jdoc);

    void on_btnCancel_clicked();

    void on_btnOK_clicked();

    void on_btnEditGuestname_clicked();

    void on_btnCancelReserve_clicked();

    void on_btnSelectTable_clicked();

    void on_btnCheckin_clicked();

    void on_btnPrintReservation_clicked();

    void on_btnCopyID_clicked();

private:
    Ui::DlgPreorder* ui;

    QJsonObject fDoc;

    void openDoc();

    void save(bool withcheckin);

    void setState();
};

#endif // DLGPREORDERW_H

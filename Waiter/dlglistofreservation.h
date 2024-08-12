#ifndef DLGLISTOFRESERVATION_H
#define DLGLISTOFRESERVATION_H

#include "c5dialog.h"

namespace Ui
{
class DlgListOfReservation;
}

class DlgListOfReservation : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgListOfReservation(QWidget *parent = nullptr);
    ~DlgListOfReservation();

private:
    Ui::DlgListOfReservation *ui;
    int fMode;

private slots:
    void openReservationResponse(const QJsonObject &jdoc);

    void listResponse(const QJsonObject &jdoc);

    void responseHistory(const QJsonObject &jdoc);

    void responseForecast(const QJsonObject &jdoc);

    void on_btnRefreshReservations_clicked();

    void on_btnExit_clicked();

    void on_tbl_cellDoubleClicked(int row, int column);

    void on_btnAllReservation_clicked();

    void on_btnHistory_clicked();

    void on_btnForecast_clicked();
};

#endif // DLGLISTOFRESERVATION_H

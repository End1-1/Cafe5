#ifndef DLGFACE_H
#define DLGFACE_H

#include "c5dialog.h"
#include "c5user.h"
#include <QTableWidgetItem>
#include <QTimer>

namespace Ui
{
class DlgFace;
}

class DlgFace : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgFace(C5User *user);

    ~DlgFace();

    static bool getTable(int &tableId, int hall, C5User *user);

private slots:
    void initResponse(const QJsonObject &jdoc);

    void refreshResponse(const QJsonObject &jdoc);

    void openReservationResponse(const QJsonObject &jdoc);

    void timeout();

    void hallClicked();

    void tableClicked(int id);

    void filterStaffClicked();

    void refreshTables();

    void handleCreditCards(const QJsonObject &obj);

    void on_btnExit_clicked();

    void on_btnCancel_clicked();

    void on_btnViewHall_clicked();

    void on_btnViewWaiter_clicked();

    void on_btnGuests_clicked();

    void on_btnChart_clicked();

    void on_btnTools_clicked();

private:
    Ui::DlgFace *ui;

    C5User *fUser;

    QJsonObject fHallState;

    int fCurrentHall;

    int fCurrentStaff;

    QTimer fTimer;

    bool fModeJustSelectTable;

    int fSelectedTable;

    int fView;

    virtual void showEvent(QShowEvent *e) override;

    void filterHall(int hall, int staff);

    void colorizeHall();

    void viewMode(int m);

    void setViewMode(int v);

};

#endif // DLGFACE_H

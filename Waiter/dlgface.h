#ifndef DLGFACE_H
#define DLGFACE_H

#include "c5dialog.h"
#include "c5user.h"
#include <QTableWidgetItem>
#include <QTimer>

namespace Ui {
class DlgFace;
}

class DlgFace : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgFace(C5User *user);

    ~DlgFace();

    void setup();

    static bool getTable(int &tableId, int hall, C5User *user);

private slots:

    void timeout();

    void hallClicked();

    void tableClicked(int id);

    void filterStaffClicked();

    void handleCreditCards(const QJsonObject &obj);

    void handleDishRemoveReason(const QJsonObject &obj);

    void handleDishComment(const QJsonObject &obj);

    void handleVersion(const QJsonObject &obj);

    void on_btnExit_clicked();

    void on_btnCancel_clicked();

    void on_btnViewHall_clicked();

    void on_btnViewWaiter_clicked();

private:
    Ui::DlgFace *ui;

    C5User *fUser;

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

    void refreshTables();
};

#endif // DLGFACE_H

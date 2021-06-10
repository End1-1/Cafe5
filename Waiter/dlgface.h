#ifndef DLGFACE_H
#define DLGFACE_H

#include "c5dialog.h"
#include <QTcpServer>
#include <QTableWidgetItem>
#include <QTimer>

namespace Ui {
class DlgFace;
}

class DlgFace : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgFace();

    ~DlgFace();

    void setup();

    void accept();

    void reject();

    static bool getTable(int &tableId, const QString &hall);

private slots:

    void timeout();

    void checkVersionTimeout();

    void confTimeout();

    void newConnection();

    void handleHall(const QJsonObject &obj);

    void handleMenu(const QJsonObject &obj);

    void handleConf(const QJsonObject &obj);

    void handleCreditCards(const QJsonObject &obj);

    void handleDishRemoveReason(const QJsonObject &obj);

    void handleDishComment(const QJsonObject &obj);

    void handleVersion(const QJsonObject &obj);

    void handleSocket(const QJsonObject &obj);

    void on_btnConnection_clicked();

    void on_btnExit_clicked();

    void on_tblHall_itemClicked(QTableWidgetItem *item);

    void on_btnHallFilter_clicked();

    void on_btnReports_clicked();

    void on_btnCancel_clicked();

    void on_btnClearDroid_clicked();

    void on_btnSetSession_clicked();

    void on_btnEnter_clicked();

    void on_btnOut_clicked();

private:
    Ui::DlgFace *ui;

    QString fCurrentHall;

    QTcpServer fTcpServer;

    QTimer fTimer;

    QTimer fTimerCheckVersion;

    QTimer fConfTimer;

    bool fCanClose;

    bool fModeJustSelectTable;

    QJsonObject fSelectedTable;

    void filterHall(const QString &hall);

    void viewMode(int m);
};

#endif // DLGFACE_H

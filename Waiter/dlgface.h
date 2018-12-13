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
    explicit DlgFace(QWidget *parent = 0);

    ~DlgFace();

    void setup();

    void accept();

    void reject();

private slots:

    void timeout();

    void newConnection();

    void handleHall(const QJsonObject &obj);

    void handleMenu(const QJsonObject &obj);

    void handleConf(const QJsonObject &obj);

    void handleCreditCards(const QJsonObject &obj);

    void handleDishRemoveReason(const QJsonObject &obj);

    void handleSocket(const QJsonObject &obj);

    void on_btnConnection_clicked();

    void on_btnExit_clicked();

    void on_tblHall_itemClicked(QTableWidgetItem *item);

    void on_btnHallFilter_clicked();

private:
    Ui::DlgFace *ui;

    QString fCurrentHall;

    QTcpServer fTcpServer;

    QTimer fTimer;

    bool fCanClose;

    void filterHall(const QString &hall);
};

#endif // DLGFACE_H

#ifndef DLGFACE_H
#define DLGFACE_H

#include "c5dialog.h"
#include <QTcpServer>
#include <QTableWidgetItem>

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

private slots:
    void newConnection();

    void handleHall(const QJsonObject &obj);

    void handleMenu(const QJsonObject &obj);

    void handleConf(const QJsonObject &obj);

    void handleSocket(const QJsonObject &obj);

    void on_btnConnection_clicked();

    void on_btnExit_clicked();

    void on_tblHall_itemClicked(QTableWidgetItem *item);

private:
    Ui::DlgFace *ui;

    QTcpServer fTcpServer;

    QJsonArray fHalls;

    QJsonArray fTables;

    void filterHall();
};

#endif // DLGFACE_H

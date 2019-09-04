#include "serverwindow.h"
#include "ui_serverwindow.h"
#include "serversocket.h"
#include "widgetcontainer.h"
#include "dateutils.h"
#include "socketthread.h"
#include <QMessageBox>
#include <QDateTime>
#include <QCloseEvent>

ServerWindow::ServerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWindow),
    WidgetControls()
{
    ui->setupUi(this);
    fCanClose = false;
    fTrayMenu.addAction(tr("Quit"), this, SLOT(appTerminate()));
    fTrayIcon.setIcon(QIcon(":/server.png"));
    fTrayIcon.setVisible(true);
    fTrayIcon.show();
    fTrayIcon.setContextMenu(&fTrayMenu);
    connect(&fTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::setWidgetContainer()
{
    fCtrl = ui->wc;
    fCtrl->getWatchList(fCtrl);
    applyConfiguration();
    ServerSocket *ss = new ServerSocket(this);
    if (!ss->listen(static_cast<quint16>(strw("serverport").toInt()))) {
        QMessageBox::critical(this, tr("Socket error"), tr("Cannot listen port"));
    }
}

void ServerWindow::closeEvent(QCloseEvent *e)
{
    if (!fCanClose) {
        e->ignore();
        hide();
        return;
    }
    QWidget::closeEvent(e);
}

void ServerWindow::applyConfiguration()
{
    hostPassword = strw("serversecret");
    dbHost = strw("dbhost");
    dbPort = strw("dbport").toInt();
    dbFile = strw("dbfile");
    dbUser = strw("dbuser");
    dbPass = strw("dbpass");
}

void ServerWindow::registerConnection(const QString &uuid, const QString &inout, const QString &remoteIp, const QString &host, const QString &user)
{
    int row = ui->tblConn->addEmptyRow();
    fUUIDRow[uuid] = row;
    ui->tblConn->setString(row, 0, datetime_str(QDateTime::currentDateTime()));
    ui->tblConn->setString(row, 2, uuid);
    ui->tblConn->setString(row, 3, inout);
    ui->tblConn->setString(row, 4, remoteIp);
    ui->tblConn->setString(row, 5, host);
    ui->tblConn->setString(row, 6, user);
}

void ServerWindow::unregisterConnection(const QString &uuid, const QString &reason)
{
    int row = 0;
    if (!fUUIDRow.contains(uuid)) {
        return;
    } else {
        row = fUUIDRow[uuid];
    }
    ui->tblConn->setString(row, 1, datetime_str(QDateTime::currentDateTime()));
    ui->tblConn->setString(row, 9, reason);
}

void ServerWindow::connectionRXTX(const QString &uuid, int rx, int tx)
{
    int row = 0;
    if (!fUUIDRow.contains(uuid)) {
        return;
    } else {
        row = fUUIDRow[uuid];
    }
    ui->tblConn->setInteger(row, 7, ui->tblConn->getInteger(row, 7) + rx);
    ui->tblConn->setInteger(row, 8, ui->tblConn->getInteger(row, 8) + tx);
}

void ServerWindow::appTerminate()
{
    if (QMessageBox::warning(this, tr("Confirmation"), tr("Are you sure to close application?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        fCanClose = true;
        close();
    }
}

void ServerWindow::iconClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        setVisible(true);
        break;
    default:
        break;
    }
}

void ServerWindow::on_btnSettings_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pSettings);
}

void ServerWindow::on_btnSaveSettings_clicked()
{
    fCtrl->saveChanges();
    applyConfiguration();
    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

void ServerWindow::on_btnConnections_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pConnections);
}

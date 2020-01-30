#include "c5server5.h"
#include "ui_c5server5.h"
#include "serversocket.h"
#include "c5printjson.h"
#include "c5reportsupload.h"
#include "server5settings.h"
#include "notificationwidget.h"
#include "c5scheduler.h"
#include "dbconnection.h"
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

C5Server5::C5Server5(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::C5Server5)
{
    ui->setupUi(this);
    wc = ui->wc;
    wc->getWatchList(this);
    fCanClose = false;
    fTrayMenu.addAction(tr("Quit"), this, SLOT(appTerminate()));
    fTrayIcon.setIcon(QIcon(":/server.png"));
    fTrayIcon.show();
    fTrayIcon.setContextMenu(&fTrayMenu);
    connect(&fTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));
    ServerSocket *ss = new ServerSocket(this);
    if (!ss->listen(static_cast<quint16>(wc->getInt("serverport")))) {
        QMessageBox::critical(this, tr("Socket error"), tr("Cannot listen port"));
    }
    scheduler = new c5scheduler(this);
}

C5Server5::~C5Server5()
{
    delete scheduler;
    delete ui;
}

void C5Server5::closeEvent(QCloseEvent *event)
{
    if (!fCanClose) {
        event->ignore();
        hide();
        return;
    }
    QWidget::closeEvent(event);
}

void C5Server5::processJson(QByteArray &d)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(d);
    QJsonObject jobj = jdoc.object();
    int cmd = jobj["cmd"].toInt();
    switch (cmd) {
    case 1:
        QJsonObject jo;
        jo["cmd"] = "print";
        jo["printer"] = jobj["printer"];
        jo["pagesize"] = jobj["pagesize"];
        QJsonArray ja = jobj["data"].toArray();
        ja.append(jo);
        C5PrintJson *pj = new C5PrintJson(ja);
        pj->start();
        NotificationWidget::showMessage(tr("New order received"));
        break;
    }
}

void C5Server5::appTerminate()
{
    if (QMessageBox::warning(this, tr("Confirmation"), tr("Are you sure to close application?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        fCanClose = true;
        close();
    }
}

void C5Server5::iconClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        setVisible(true);
        break;
    default:
        break;
    }
}

void C5Server5::on_btnApply_clicked()
{
    if (ui->wc->hasChanges()) {
        ui->wc->saveChanges();
    }
}

void C5Server5::clientSocketDataRead(const QString &uuid, QByteArray &d)
{
    int cmd;
    memcpy(&cmd, d.data(), sizeof(cmd));
    d.remove(0, sizeof(cmd));
    switch (cmd) {
    case 1:
        processJson(d);
        break;
    }
    emit sendData(uuid);
}

void C5Server5::on_btnReportsToUpload_clicked()
{
    C5ReportsUpload *r = new C5ReportsUpload(this);
    r->exec();
    delete r;
}

void C5Server5::on_btnDatabase_clicked()
{
    if (!__s.value("dbpassword").toString().isEmpty()) {
        bool ok = false;
        QString pwd = QInputDialog::getText(this, tr("Database password"), tr("Password"), QLineEdit::Password, "", &ok);
        if (!ok) {
            return;
        }
        if (pwd != __s.value("dbpassword").toString()) {
            QMessageBox::critical(this, tr("Error"), tr("Invalid database password"));
            return;
        }
    }
    DbConnection *d = new DbConnection(this);
    d->exec();
    delete d;
}

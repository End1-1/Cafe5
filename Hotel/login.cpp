#include "login.h"
#include "ui_login.h"
#include "connectionsettings.h"
#include "clientsocket.h"
#include "settings.h"
#include "message.h"
#include "cmd.h"
#include "user.h"
#include <QJsonObject>
#include <QJsonArray>

Login::Login() :
    Dialog(),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    if (!__s.s("host").isEmpty()) {
        QJsonObject jo;
        if(!__socket.connectToSocket(__s.s("host"), __s.i("port"), jo)) {
            Message::showMessage(tr("Cannot connect to server") + QString("<br>%1:%2").arg(__s.s("host")).arg(__s.i("port")));
            return;
        }
        QJsonArray ja = jo["dblist"].toArray();
        for (int i = 0; i < ja.count(); i++) {
            QJsonObject jdb = ja.at(i).toObject();
            ui->cbDB->addItem(jdb["fname"].toString(), jdb);
        }
    }
}

Login::~Login()
{
    delete ui;
}

bool Login::login()
{
    Login *l = new Login();
    l->setWidgetContainer();
    if (l->success()) {
        return true;
    } else {
        return false;
    }
}

void Login::setWidgetContainer()
{
    fCtrl = ui->wc;
    fCtrl->getWatchList(fCtrl);
    if (!strw("login").isEmpty()) {
        ui->lePassword->setFocus();
    }
}

bool Login::success()
{
    if (exec() == QDialog::Accepted) {
        fCtrl->saveChanges();
        return true;
    }
    return false;
}

void Login::on_btnCancel_clicked()
{
    reject();
}

void Login::on_btnOk_clicked()
{
    if (ui->cbDB->currentIndex() < 0) {
        Message::showMessage(tr("Please, select the database"));
        return;
    }
    __socket["cmd"] = Cmd::cmd_auth_user;
    __socket["username"] = strw("login");
    __socket["password"] = ui->lePassword->text();
    __socket["db"] = ui->cbDB->currentData().toJsonObject()["fid"].toString();
    if (__socket.sendJson()) {
        QJsonObject jo = __socket.responseJson();
        if (jo["reply"].toInt() == 1) {
            __user->reg(jo);
            accept();
            return;
        } else {
            Message::showMessage(tr("Authorization error"));
        }
    } else {
        Message::showMessage(tr("Connection error"));
    }
}

void Login::on_btnSettings_clicked()
{
    ConnectionSettings::configure();
}

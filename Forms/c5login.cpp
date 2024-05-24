#include "c5login.h"
#include "ui_c5login.h"
#include "c5user.h"
#include "c5servername.h"
#include <QSettings>

C5Login::C5Login() :
    C5Dialog(QStringList()),
    ui(new Ui::C5Login)
{
    ui->setupUi(this);
    readServers();
}

C5Login::~C5Login()
{
    delete ui;
}

void C5Login::on_btnCancel_clicked()
{
    reject();
}

void C5Login::on_btnOk_clicked()
{
    if (!__user) {
        __user = new C5User(0);
    }
    if (!__user->authByUsernamePass(ui->leUsername->currentText(), ui->lePassword->text())) {
        C5Message::error(tr("Login failed"));
        return;
    }
    if (!__user->isActive()) {
        C5Message::error(tr("User is inactive"));
        return;
    }
    for (int i = 0; i < fServers.size(); i++) {
        QJsonObject js = fServers.at(i).toObject();
        js["lastusername"] = "";
        js["lastdb"] = "";
        fServers[i] = js;
    }
    int index = ui->cbDatabases->currentIndex();
    QJsonObject js = fServers.at(index).toObject();
    js["lastusername"] = ui->leUsername->currentText();
    js["lastdb"] = ui->cbDatabases->currentText();
    fServers[index] = js;
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    s.setValue("servers", QJsonDocument(fServers).toJson());
    accept();
}

void C5Login::on_cbDatabases_currentIndexChanged(int index)
{
    for (int i = 0; i < fServers.count(); i++) {
        QJsonObject js = fServers.at(i).toObject();
        js["current"] = i == index;
        fServers[i] = js;
    }
    const QJsonObject &js = fServers.at(index).toObject();
    C5Config::fDBHost = js["host"].toString();
    C5Config::fDBPath = js["database"].toString();
    C5Config::fDBUser = js["username"].toString();
    C5Config::fDBPassword = js["password"].toString();
    C5Config::fSettingsName = js["settings"].toString();
    C5Config::fLastUsername = js["lastusername"].toString();
    C5Config::fFullScreen = js["fullscreen"].toBool();
    C5Database db(js);
    db.exec("select f_login from s_user where f_id in (select f_user from s_db_access where f_permit=1)");
    ui->leUsername->clear();
    while (db.nextRow()) {
        ui->leUsername->addItem(db.getString(0));
    }
    ui->leUsername->setCurrentIndex(-1);
    if (js["lastusername"].toString().length() > 0) {
        ui->leUsername->setCurrentText(js["lastusername"].toString());
        ui->lePassword->setFocus();
    }
}

void C5Login::readServers()
{
    fServers = C5ServerName::mServers;
    ui->cbDatabases->clear();
    int dbindex = 0;
    for (int i = 0; i < fServers.count(); i++) {
        const QJsonObject &js = fServers.at(i).toObject();
        if (!js["lastdb"].toString().isEmpty()) {
            dbindex = i;
        }
        ui->cbDatabases->addItem(js["name"].toString());
    }
    ui->cbDatabases->setCurrentIndex(dbindex);
}

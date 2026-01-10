#include "c5login.h"
#include "ui_c5login.h"
#include "ndataprovider.h"
#include "c5user.h"
#include "c5servername.h"
#include "fileversion.h"
#include "c5config.h"
#include "c5database.h"
#include "appwebsocket.h"
#include "c5officewidget.h"
#include "c5connectiondialog.h"
#include <QSettings>

C5Login::C5Login(C5User *user) :
    C5OfficeDialog(user),
    ui(new Ui::C5Login)
{
    ui->setupUi(this);
    ui->leVersion->setText(FileVersion::getVersionString(qApp->applicationFilePath()));
#ifndef QT_DEBUG
    ui->leVersion->setText(FileVersion::getVersionString(qApp->applicationFilePath()));
    ui->leVersion->setVisible(false);
#endif
    readServers();
    adjustSize();
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
    if(!mUser) {
        mUser = new C5User();
        C5OfficeWidget::mUser = mUser;
    }

    NDataProvider::mFileVersion = ui->leVersion->text();
    mUser->authByUsernamePass(ui->leUsername->currentText(), ui->lePassword->text(), fHttp, [this](const QJsonObject & jo) {
        QJsonObject js;

        for(int i = 0; i < fServers.size(); i++) {
            js = fServers.at(i).toObject();
            js["lastusername"] = "";
            js["lastdb"] = "";
            fServers[i] = js;
        }

        QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
        s.setValue("lastdb", ui->cbDatabases->currentText());
        s.setValue("lastusername", ui->leUsername->currentText());
        s.setValue("lastversion", ui->leVersion->text());
        __c5config.fDBName = ui->cbDatabases->currentText();
        C5Database::fDbParams = {"", "", "", ""};
        js = fServers.at(ui->cbDatabases->currentIndex()).toObject();
        C5Database::fUrl = QString("%1://%2").arg(js["settings"].toString(),
                           js["database"].toString());
        __c5config.setValues(mUser->fSettings);
        __c5config.fMainJson = mUser->fConfig;
        AppWebSocket::reconnect(QString("%1://%2").arg(js["settings"].toString() == "http" ? "ws" : "wss",
                                js["database"].toString() + "/ws"),
                                js["fkey"].toString(),
                                ui->leUsername->currentText(), ui->lePassword->text());
        accept();
    });
}

void C5Login::on_cbDatabases_currentIndexChanged(int index)
{
    for(int i = 0; i < fServers.count(); i++) {
        QJsonObject js = fServers.at(i).toObject();
        js["current"] = i == index;
        fServers[i] = js;
    }

    const QJsonObject &js = fServers.at(index).toObject();
    NDataProvider::mHost = js["database"].toString();
    NDataProvider::mAppName = "officen";
    NDataProvider::mProtocol = js["settings"].toString();
    C5Config::fDBHost = js["settings"].toString();
    C5Config::fDBPath = js["database"].toString();
    C5Config::fDBUser = js["username"].toString();
    C5Config::fDBPassword = js["password"].toString();
    C5Config::fSettingsName = js["settings"].toString();
    C5Config::fLastUsername = js["lastusername"].toString();
    C5Config::fFullScreen = js["fullscreen"].toBool();
    C5Database::fDbParams = {"", "", "", ""};
    C5Database::fUrl = QString("%1://%2").arg(js["settings"].toString(), js["database"].toString());
    C5Database db(js);
    db.exec("select f_login from s_user where f_id in (select f_user from s_db_access where f_permit=1)");
    ui->leUsername->clear();

    while(db.nextRow()) {
        ui->leUsername->addItem(db.getString(0));
    }

    ui->leUsername->setCurrentIndex(-1);

    if(js["lastusername"].toString().length() > 0) {
        ui->leUsername->setCurrentText(js["lastusername"].toString());
        ui->lePassword->setFocus();
    }
}

void C5Login::readServers()
{
    fServers = C5ServerName::mServers;
    ui->cbDatabases->clear();

    for(int i = 0; i < fServers.count(); i++) {
        const QJsonObject &js = fServers.at(i).toObject();
        ui->cbDatabases->addItem(js["name"].toString());
    }

    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
#ifdef QT_DEBUG

    if(s.value("lastversion").toString().isEmpty() == false) {
        ui->leVersion->setText(s.value("lastversion").toString());
    }

#endif
    int dbindex = ui->cbDatabases->findText(s.value("lastdb").toString());
    ui->cbDatabases->setCurrentIndex(dbindex);

    if(dbindex > -1) {
        ui->leUsername->setCurrentText(s.value("lastusername").toString());
        ui->lePassword->setFocus();
    }
}

void C5Login::on_btnConfig_clicked()
{
    C5ConnectionDialog::showSettings(this);
}

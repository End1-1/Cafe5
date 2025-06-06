#include "dlgserverconnection.h"
#include "ui_dlgserverconnection.h"
#include "c5config.h"
#include "c5message.h"
#include "c5servername.h"
#include <QInputDialog>

DlgServerConnection::DlgServerConnection(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgServerConnection)
{
    ui->setupUi(this);
    ui->leAddress->setText(__c5config.getRegValue("ss_server_address").toString());
    ui->leServerKey->setText(__c5config.getRegValue("ss_server_key").toString());
    ui->leSettingsPassword->setText(__c5config.getRegValue("ss_settings_password").toString());
    ui->cbDatabases->setCurrentText(__c5config.getRegValue("ss_database").toString());
    ui->leSettingsName->setText(__c5config.getRegValue("ss_settings").toString());
}

DlgServerConnection::~DlgServerConnection()
{
    delete ui;
}

void DlgServerConnection::showSettings(QWidget *parent)
{
    auto *dlg = new DlgServerConnection(parent);
    if (!dlg->ui->leSettingsPassword->text().isEmpty()) {
        bool ok = false;
        while (!ok) {
            QString pass = QInputDialog::getText(parent, tr("Settings password"), "", QLineEdit::Password, "", &ok);
            if (!ok) {
                return;
            }
            if (pass != dlg->ui->leSettingsPassword->text()) {
                ok = false;
                C5Message::error(tr("Access denied"));
            } else {
                ok = true;
            }
        }
        if (!ok) {
            return;
        }
    }
    dlg->exec();
}

void DlgServerConnection::on_btnShowPassword_clicked(bool checked)
{
    if (checked) {
        ui->leSettingsPassword->setEchoMode(QLineEdit::Normal);
    } else {
        ui->leSettingsPassword->setEchoMode(QLineEdit::Password);
    }
}

void DlgServerConnection::on_btnSave_clicked()
{
    __c5config.setRegValue("ss_server_address", ui->leAddress->text());
    __c5config.setRegValue("ss_server_key", ui->leServerKey->text());
    __c5config.setRegValue("ss_settings_password", ui->leSettingsPassword->text());
    __c5config.setRegValue("ss_database", ui->cbDatabases->currentText());
    __c5config.setRegValue("ss_settings", ui->leSettingsName->text());
    C5Message::info(tr("Saved"));
}

void DlgServerConnection::on_btnCancel_clicked()
{
    reject();
}

void DlgServerConnection::on_btnGetDatabases_clicked()
{
    C5ServerName cn(ui->leAddress->text());
    if (cn.getServers()) {
        ui->cbDatabases->clear();
        for (int i = 0; i < cn.mServers.size(); i++) {
            const QJsonObject &jt = cn.mServers.at(i).toObject();
            ui->cbDatabases->addItem(jt["name"].toString());
        }
    } else {
        C5Message::error(cn.mErrorString);
    }
}

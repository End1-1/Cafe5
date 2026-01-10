#include "c5connectiondialog.h"
#include "ui_c5connectiondialog.h"
#include "c5message.h"
#include <QInputDialog>
#include <QSettings>

C5ConnectionDialog* C5ConnectionDialog::mInstance = nullptr;

C5ConnectionDialog::C5ConnectionDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::C5ConnectionDialog)
{
    ui->setupUi(this);
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    ui->leAddress->setText(s.value("ss_server_address").toString());
    ui->leServerKey->setText(s.value("ss_server_key").toString());
    ui->leSettingsPassword->setText(s.value("ss_settings_password").toString());
    ui->cbConnection->setCurrentIndex(s.value("ss_secure_connection").toInt());
    ui->leSettingsName->setText(s.value("ss_settings").toString());
    ui->leServerUsername->setText(s.value("ss_server_username").toString());
    ui->leServerPassword->setText(s.value("ss_server_password").toString());
}

C5ConnectionDialog::~C5ConnectionDialog() { delete ui; }

void C5ConnectionDialog::showSettings(QWidget *parent)
{
    if(!instance()->ui->leSettingsPassword->text().isEmpty()) {
        bool ok = false;

        while(!ok) {
            QString pass = QInputDialog::getText(parent, tr("Settings password"), "", QLineEdit::Password, "", &ok);

            if(!ok) {
                return;
            }

            if(pass != instance()->ui->leSettingsPassword->text()) {
                ok = false;
                C5Message::error(tr("Access denied"));
            } else {
                ok = true;
            }
        }

        if(!ok) {
            return;
        }
    }

    instance()->exec();
}

C5ConnectionDialog* C5ConnectionDialog::instance()
{
    if(!mInstance) {
        mInstance = new C5ConnectionDialog();
    }

    return mInstance;
}

QString C5ConnectionDialog::serverAddress()
{
    return ui->leAddress->text();
}

QString C5ConnectionDialog::serverKey()
{
    return ui->leServerKey->text();
}

QString C5ConnectionDialog::username()
{
    return ui->leServerUsername->text();
}

QString C5ConnectionDialog::password()
{
    return ui->leServerUsername->text();
}

int C5ConnectionDialog::connectionType()
{
    ConnectionType ct = ui->cbConnection->currentIndex() == 0 ? noneSecure : Secure;
    return ct;
}

void C5ConnectionDialog::on_btnSave_clicked()
{
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    s.setValue("ss_server_address", ui->leAddress->text());
    s.setValue("ss_server_key", ui->leServerKey->text());
    s.setValue("ss_settings_password", ui->leSettingsPassword->text());
    s.setValue("ss_secure_connection", ui->cbConnection->currentIndex());
    s.setValue("ss_settings", ui->leSettingsName->text());
    s.setValue("ss_server_username", ui->leServerUsername->text());
    s.setValue("ss_server_password", ui->leServerPassword->text());
    C5Message::info(tr("Saved"));
}

void C5ConnectionDialog::on_btnCancel_clicked()
{
    reject();
}

void C5ConnectionDialog::on_btnShowPassword_clicked(bool checked)
{
    if(checked) {
        ui->leSettingsPassword->setEchoMode(QLineEdit::Normal);
    } else {
        ui->leSettingsPassword->setEchoMode(QLineEdit::Password);
    }
}

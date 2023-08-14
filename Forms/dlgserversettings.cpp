#include "dlgserversettings.h"
#include "ui_dlgserversettings.h"
#include "serverconnection.h"
#include "socketconnection.h"
#include <QMessageBox>

DlgServerSettings::DlgServerSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgServerSettings)
{
    ui->setupUi(this);
    QString ip, username, password;
    int port;
    ServerConnection::getParams(ip, port, username, password);
    ui->leServerIP->setText(ip);
    ui->leServerPort->setText(QString::number(port));
    ui->leUsername->setText(username);
    ui->lePassword->setText(password);
    ui->lePassword2->setText(password);
}

DlgServerSettings::~DlgServerSettings()
{
    delete ui;
}

void DlgServerSettings::on_btnCancel_clicked()
{
    reject();
}

void DlgServerSettings::on_btnSave_clicked()
{
    if (ui->lePassword->text() != ui->lePassword2->text()) {
        QMessageBox::critical(this, windowTitle(), tr("Passwords did not match"));
        return;
    }
    ServerConnection::setParams(ui->leServerIP->text(), ui->leServerPort->text(), ui->leUsername->text(), ui->lePassword->text());
    SocketConnection::startConnection(ui->leServerIP->text(), ui->leServerPort->text().toInt(), ui->leUsername->text(), ui->lePassword->text());
    QMessageBox::information(this, windowTitle(), tr("Saved"));
    accept();
}

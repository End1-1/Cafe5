#include "c5connection.h"
#include "ui_c5connection.h"
#include "c5config.h"
#include <QSettings>
#include <QInputDialog>

#define db_ver 1

C5Connection::C5Connection(QWidget *parent) :
    C5Dialog(parent),
    ui(new Ui::C5Connection)
{
    ui->setupUi(this);

    QList<QByteArray> params;
    readParams(params);
    if (params.count() < 5) {
        return;
    }
    ui->leHost->setText(params.at(0));
    ui->leDatabase->setText(params.at(1));
    ui->leUsername->setText(params.at(2));
    ui->lePassword->setText(params.at(3));
    ui->cbSettings->setCurrentText(params.at(4));
}

C5Connection::~C5Connection()
{
    delete ui;
}

void C5Connection::readParams(QList<QByteArray> &params)
{
    QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    if (s.value("db_ver").toInt() == 0) {
        s.setValue("db_ver", db_ver);
    }
    QByteArray buf = s.value("db").toByteArray();
    for (int i = 0; i < buf.length(); i++) {
        buf[i] = buf[i] ^ ((i % 2) + (i % 3) + (i % 4) + (i % 5) + (i % 6) + (i % 7) + (i % 8) + (i % 9));
    }
    params = buf.split('\r');

}

bool C5Connection::preambule()
{
    bool ok = false;
    QString password = QInputDialog::getText(__c5config.fParentWidget, tr("Configuration settings"), tr("Password"), QLineEdit::Password, "", &ok);
    if (ok) {
        if (password != ui->lePassword->text()) {
            C5Message::error(tr("Access denied"));
            return false;
        }
    } else {
        return false;
    }
    return true;
}

void C5Connection::on_btnCancel_clicked()
{
    reject();
}

void C5Connection::on_btnTest_clicked()
{
    C5Database db(ui->leHost->text(), ui->leDatabase->text(), ui->leUsername->text(), ui->lePassword->text());
    if (!db.open()) {
        C5Message::error(tr("Cannot connect to database") + "<br>" + db.fLastError);
        return;
    }
    db.close();
    C5Message::info(tr("Connection successfull"));
}

void C5Connection::on_btnSave_clicked()
{
    QByteArray buf;
    buf.append(ui->leHost->text());
    buf.append('\r');
    buf.append(ui->leDatabase->text());
    buf.append('\r');
    buf.append(ui->leUsername->text());
    buf.append('\r');
    buf.append(ui->lePassword->text());
    buf.append('\r');
    buf.append(ui->cbSettings->currentText());
    buf.append('\r');

    for (int i = 0; i < buf.length(); i++) {
        buf[i] = buf[i] ^ ((i % 2) + (i % 3) + (i % 4) + (i % 5) + (i % 6) + (i % 7) + (i % 8) + (i % 9));
    }
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    s.setValue("db", buf);
    s.setValue("db_ver", db_ver);

    C5Database::fHost = ui->leHost->text();
    C5Database::fFile = ui->leDatabase->text();
    C5Database::fUser = ui->leUsername->text();
    C5Database::fPass = ui->lePassword->text();
    C5Config::fSettingsName = ui->cbSettings->currentText();
}

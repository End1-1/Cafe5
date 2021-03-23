#include "testerdialog.h"
#include "ui_testerdialog.h"
#include "test.h"
#include "testn1.h"
#include "testn2.h"
#include <QSettings>
#include <QFile>

TesterDialog::TesterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TesterDialog)
{
    ui->setupUi(this);
    QSettings s("NEWSTARSOFT", "SERVICE5TESTER");
    ui->leServerIP->setText(s.value("server_ip").toString());
    ui->leServerPort->setText(s.value("server_port").toString());
    ui->peHttpRequest->setPlainText(s.value("http_request").toString());
    fTextN1Count = 0;
    ui->peLog->appendPlainText(QString("SSL support: %1").arg(QSslSocket::supportsSsl() ? "YES" : "NO"));
    ui->peLog->appendPlainText(QSslSocket::sslLibraryBuildVersionString());
    ui->peLog->appendPlainText(QSslSocket::sslLibraryVersionString());
}

TesterDialog::~TesterDialog()
{
    QSettings s("NEWSTARSOFT", "SERVICE5TESTER");
    s.setValue("server_ip", ui->leServerIP->text());
    s.setValue("server_port", ui->leServerPort->text());
    s.setValue("http_request", ui->peHttpRequest->toPlainText());
    delete ui;
}

void TesterDialog::n1Finished()
{
    ui->peLog->appendPlainText(QString("N1 count: %1").arg(--fTextN1Count));
}

void TesterDialog::n1Error(int code, const QString &msg)
{
    ui->peLog->appendPlainText(QString("Last error #%2: %3").arg(code).arg(msg));
}

void TesterDialog::data(int code, const QVariant &d)
{
    ui->peLog->appendPlainText(QString("Data #%2: %3").arg(code).arg(d.toString()));
}


void TesterDialog::on_btnTestN1_clicked()
{
    ui->peLog->appendPlainText("Test N1. Open connection, write data, read data, close");
    qApp->processEvents();
    QMap<QString, QVariant> mp;
    mp["http_request"] = ui->peHttpRequest->toPlainText().replace("\n", "\r\n");
    auto *t = new Test<TestN1>(ui->leServerIP->text(), ui->leServerPort->text().toInt(), ui->leN1->text().toInt(), mp, this, SLOT(data(int,QVariant)), SLOT(n1Finished()), SLOT(n1Error(int,QString)));
    t->start();
    ui->peLog->appendPlainText(QString("N1 count: %1").arg(fTextN1Count += ui->leN1->text().toInt()));
}

void TesterDialog::on_btnTestN2_clicked()
{
    ui->peLog->appendPlainText("Test N1. Open connection.");
    qApp->processEvents();
    auto *t = new Test<TestN2>(ui->leServerIP->text(), ui->leServerPort->text().toInt(), ui->leN2->text().toInt(), ui->leN2Timeout->text().toInt(), this, SLOT(data(int,QVariant)), SLOT(n1Finished()), SLOT(n1Error(int,QString)));
    t->start();
    ui->peLog->appendPlainText(QString("N1 count: %1").arg(fTextN1Count += ui->leN2->text().toInt()));
}

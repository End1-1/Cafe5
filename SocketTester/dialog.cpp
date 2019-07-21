#include "dialog.h"
#include "ui_dialog.h"
#include <QSettings>

static QSettings s("TESTER", "TESTER");

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->lePort->setText(s.value("port").toString());
    ui->leAddress->setText(s.value("address").toString());
    connect(&fSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_btnConnect_clicked()
{
    fSocket.disconnectFromHost();
    fSocket.connectToHost(ui->leAddress->text(), ui->lePort->text().toInt());
    if(fSocket.waitForConnected()) {
        ui->teLog->append("Connected");
    } else {
        ui->teLog->append("Cannot connect " + fSocket.errorString());
    }
    datasize = 0;
}

void Dialog::on_teData_textChanged()
{
    ui->leDataSize->setText(QString::number(ui->teData->toPlainText().toUtf8().length()));
}

void Dialog::on_leAddress_textChanged(const QString &arg1)
{
    s.setValue("address", arg1);
}

void Dialog::on_lePort_textChanged(const QString &arg1)
{
    s.setValue("port", arg1);
}

void Dialog::on_btnSend_clicked()
{
    int datasize = ui->teData->toPlainText().toUtf8().length();
    fSocket.write(reinterpret_cast<const char*>(&datasize), sizeof(datasize));
    fSocket.write(ui->teData->toPlainText().toUtf8(), datasize);
    fSocket.flush();
}

void Dialog::readyRead()
{
    if (datasize == 0) {
        fSocket.read(reinterpret_cast<char *>(&datasize), sizeof(datasize));
        ui->teLog->append(QString::number(datasize));
    }
    ui->teLog->append(fSocket.readAll());
    datasize = 0;
}

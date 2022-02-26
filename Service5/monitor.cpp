#include "monitor.h"
#include "ui_monitor.h"
#include "serverthread.h"
#include "monitoringwindow.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>

Monitor::Monitor(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Monitor)
{
    ui->setupUi(this);
    MonitoringWindow::init();
    MonitoringWindow::connectReceiver(this);
}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::receiveData(int code, const QString &session, const QString &data1, const QVariant &data2)
{
    Q_UNUSED(data2);
    QString logstr = QString("[%1][%2][%3] %4").arg(QString::number(code), session, QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"), data1);
    ui->ptRawLog->appendPlainText(logstr);

    if (ui->ptRawLog->toPlainText().length() > 30000) {
        ui->ptRawLog->setPlainText(ui->ptRawLog->toPlainText().remove(0, 9000));
    }
}

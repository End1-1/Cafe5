#include "testerdialog.h"
#include "ui_testerdialog.h"
#include "test.h"
#include "socketconnection.h"
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

QList<SocketConnection*> fSockets;

TesterDialog::TesterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TesterDialog)
{
    ui->setupUi(this);
    QSettings s("NEWSTARSOFT", "SERVICE5TESTER");
    ui->leServerIP->setText(s.value("server_ip").toString());
    ui->leServerPort->setText(s.value("server_port").toString());
    ui->leClientsFile->setText(s.value("clientfile").toString());
}

TesterDialog::~TesterDialog()
{
    QSettings s("NEWSTARSOFT", "SERVICE5TESTER");
    s.setValue("server_ip", ui->leServerIP->text());
    s.setValue("server_port", ui->leServerPort->text());
    s.setValue("clientfile", ui->leClientsFile->text());
    delete ui;
}
void TesterDialog::on_btnSelectClientFile_clicked()
{
    QString folder = QFileDialog::getOpenFileName(this, "Client list", "", "*.txt");
    if (!folder.isEmpty()) {
        ui->leClientsFile->setText(folder);
    }
}

void TesterDialog::on_btnClientLogin_clicked()
{
    for (SocketConnection *s: fSockets) {
        s->deleteLater();
    }
    fSockets.clear();
    QFile f(ui->leClientsFile->text());
    if (f.open(QIODevice::ReadOnly)) {
        QByteArray b;
        do {
            //QThread::msleep(100);
            b = f.readLine();
            if (b.isEmpty()) {
                break;
            }
            QStringList d = QString(b.replace("\r\n", "")).split("\t");
            auto *t = new QThread();
            auto *s = new SocketConnection(d.at(0), d.at(1));
            fSockets.append(s);
            connect(t, &QThread::finished, t, &QThread::deleteLater);
            connect(t, &QThread::started, s, &SocketConnection::run);
            connect(this, &TesterDialog::driveRoute, s, &SocketConnection::driverRoute);
            s->moveToThread(t);
            t->start();
        } while (true);
        f.close();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Cannot open client file"));
        return;
    }
    qDebug() << "Total sockets:" << fSockets.count();
}

void TesterDialog::on_btnDriverRoute_clicked()
{
    QString dirName = QFileInfo(ui->leClientsFile->text()).absolutePath() + "/routes";
    QDir directory(dirName);
    QStringList routes = directory.entryList(QStringList() << "*.dat" ,QDir::Files);
    int i = 0;
    for (SocketConnection *s: fSockets) {
        int fileIndex = (i++) % routes.count();
        emit driveRoute(s, dirName + "/" + routes.at(fileIndex));
        QThread::sleep(i % 2 ? 1 : 2);
    }
}

void TesterDialog::on_btnClientsLogout_clicked()
{
    for (SocketConnection *s: fSockets) {
        s->deleteLater();
    }
    fSockets.clear();
}


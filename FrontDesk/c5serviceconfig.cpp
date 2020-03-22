#include "c5serviceconfig.h"
#include "ui_c5serviceconfig.h"
#include "c5message.h"
#include "servicecommands.h"
#include <QSslSocket>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

C5ServiceConfig::C5ServiceConfig(const QString &ip, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::C5ServiceConfig)
{
    ui->setupUi(this);
    ui->leIP->setText(ip);
    fSslSocket = nullptr;
}

C5ServiceConfig::~C5ServiceConfig()
{
    delete ui;
    if (fSslSocket) {
        delete fSslSocket;
    }
}

void C5ServiceConfig::connectToService(QByteArray &data, qint32 &datatype)
{
    QFile file(qApp->applicationDirPath() + "/" + "cert.pem");
    if (!file.open(QIODevice::ReadOnly)) {
        C5Message::error("Certificate file missing");
        return;
    }
    QSslCertificate cert = QSslCertificate(file.readAll());
    fSslSocket = new QSslSocket(this);
    fSslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    fSslSocket->addCaCertificate(cert);
    fSslSocket->connectToHostEncrypted(ui->leIP->text(), 8688);
    if (!fSslSocket->waitForEncrypted(3000)) {
        data = fSslSocket->errorString().toUtf8();
        fSslSocket->close();
        return;
    }
    qint32 datasize = 0;
    datasize = data.size();
    fSslSocket->write(reinterpret_cast<const char*>(&datasize), sizeof(datasize));
    fSslSocket->write(reinterpret_cast<const char*>(&datatype), sizeof(datatype));
    fSslSocket->write(data);
    fSslSocket->flush();
    data.clear();
    int dataread = 0;
    int buffersize = 8192;
    char *buffer = new char[buffersize];
    if (fSslSocket->waitForReadyRead()) {
        fSslSocket->read(reinterpret_cast<char *>(&datasize), sizeof(datasize));
        fSslSocket->read(reinterpret_cast<char *>(&datatype), sizeof(datatype));
        while (dataread < datasize) {
            int r = fSslSocket->read(buffer, buffersize);
            dataread += r;
            data.append(buffer, r);
        }
    }
    delete [] buffer;
    fSslSocket->close();
}

void C5ServiceConfig::on_btnConnect_clicked()
{
    QByteArray data;
    QJsonObject jo;
    jo["adminpass"] = ui->leAdminPass->text();
    QJsonDocument jdoc(jo);
    data.append(jdoc.toJson(QJsonDocument::Compact));
    qint32 response = dt_service_login;
    connectToService(data, response);
    if (response != dr_ok) {
        C5Message::error(data);
        return;
    }
    jdoc = QJsonDocument::fromJson(data);
    jo = jdoc.object();
    ui->leHost->setText(jo["dbhost"].toString());
    ui->leSchema->setText(jo["dbfile"].toString());
    ui->leUsername->setText(jo["dbuser"].toString());
}

void C5ServiceConfig::on_btnSave_clicked()
{
    QByteArray data;
    QJsonObject jo;
    jo["adminpass"] = ui->leAdminPass->text();
    jo["dbhost"] = ui->leHost->text();
    jo["dbfile"] = ui->leSchema->text();
    jo["dbuser"] = ui->leUsername->text();
    jo["dbpass"] = ui->lePassword->text();
    QJsonDocument jdoc(jo);
    data.append(jdoc.toJson(QJsonDocument::Compact));
    qint32 response = dt_service_config;
    connectToService(data, response);
    if (response == dr_ok) {
        C5Message::error(tr("Saved"));
        return;
    } else {
        C5Message::error(data);
    }
}

#include "monitor.h"
#include "ui_monitor.h"
#include "configini.h"
#include "database.h"
#include "dlglicenses.h"
#include "c5crypt.h"
#include "c5license.h"
#include "os.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QSslSocket>
#include <Windows.h>
#include <QThread>

Monitor::Monitor(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Monitor)
{
    ui->setupUi(this);
    ui->leMainHost->setText(ConfigIni::value("db/host"));
    ui->leMainSchema->setText(ConfigIni::value("db/schema"));
    ui->leMainusername->setText(ConfigIni::value("db/username"));
    ui->leMainPassword->setText(ConfigIni::value("db/password"));
    ui->leMariaDBInstallationPath->setText(ConfigIni::value("db/mariadbpath"));
    if (ui->leMainHost->text().isEmpty()) {
        ui->leMainHost->setText("127.0.0.1");
    }
    if (ui->leMainSchema->text().isEmpty()) {
        ui->leMainSchema->setText("server5");
    }
    if (ui->leMainusername->text().isEmpty()) {
        ui->leMainusername->setText("root");
    }
    if (ui->leMariaDBInstallationPath->text().isEmpty()) {
        QSettings s("HKEY_LOCAL_MACHINE\\SOFTWARE", QSettings::Registry64Format);
        for (const auto &c: s.childGroups()) {
            if (c.contains("MariaDB", Qt::CaseInsensitive)) {
                s.beginGroup(c);
                ui->leMariaDBInstallationPath->setText(s.value("INSTALLDIR").toString());
                s.endGroup();
                break;
            }
        }
    }

    ui->leServerPort->setText(ConfigIni::value("server/port"));
    if (ui->leServerPort->text().isEmpty()) {
        ui->leServerPort->setText("10002");
    }

    Database db;
    if (db.open(ui->leMainHost->text(), "", ui->leMainusername->text(), ui->leMainPassword->text())) {
        bool ok = false;
        if (db.exec("use server5")) {
            if (db.exec("show tables")) {
                if (db.rowCount() > 0) {
                    ok = true;
                }
            }
        }
        ui->lbServerDBStatus->setText(ok ? tr("Created") : tr("Need to create"));
        ok = false;
        if (db.exec("use airlog")) {
            if (db.exec("show tables")) {
                if (db.rowCount() > 0) {
                    ok = true;
                }
            }
        }
        ui->lbLogDBStatus->setText(ok ? tr("Created") : tr("Need to create"));
        ok = false;
        if (db.exec("use cafe5")) {
            if (db.exec("show tables")) {
                if (db.rowCount() > 0) {
                    ok = true;
                }
            }
        }
        ui->lbWorkingDBStatus->setText(ok ? tr("Created") : tr("Need to create"));
    }

    QString app = qApp->applicationFilePath();
    DWORD dwHandle;
    DWORD dwLen = GetFileVersionInfoSize(app.toStdWString().c_str(), &dwHandle);

    // GetFileVersionInfo
    QString v = "?.?.?.?";
    BYTE *lpData = new BYTE[dwLen];
    if(!GetFileVersionInfo(app.toStdWString().c_str(), dwHandle, dwLen, lpData)) {
        delete[] lpData;
    } else {
        // VerQueryValue
        VS_FIXEDFILEINFO *lpBuffer = nullptr;
        UINT uLen;
        if(VerQueryValue(lpData, QString("\\").toStdWString().c_str(), (LPVOID*)&lpBuffer, &uLen)) {
            v =
                QString::number((lpBuffer->dwFileVersionMS >> 16) & 0xffff) + "." +
                QString::number((lpBuffer->dwFileVersionMS) & 0xffff ) + "." +
                QString::number((lpBuffer->dwFileVersionLS >> 16 ) & 0xffff ) + "." +
                QString::number((lpBuffer->dwFileVersionLS) & 0xffff );
        }
        }
    ui->lbVersion->setText(v);

    readLicense();
}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::on_btnSave_clicked()
{
    ConfigIni::setValue("db/host", ui->leMainHost->text());
    ConfigIni::setValue("db/schema", ui->leMainSchema->text());
    ConfigIni::setValue("db/username", ui->leMainusername->text());
    ConfigIni::setValue("db/password", ui->leMainPassword->text());
    ConfigIni::setValue("db/mariadbpath", ui->leMariaDBInstallationPath->text());
    ConfigIni::setValue("server/port", ui->leServerPort->text());
}

void Monitor::on_btnCheckDBConnection_clicked()
{
    Database db;
    if (db.open(ui->leMainHost->text(), "", ui->leMainusername->text(), ui->leMainPassword->text())) {
        QMessageBox::information(this, tr("Connection"), tr("Connection successfull"));
    } else {
        QMessageBox::critical(this, tr("Connection"), tr("Connection fail"));
    }
}

void Monitor::on_btnCreateDatabases_clicked()
{
    if (ui->leMariaDBInstallationPath->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please, select the MariaDB installtion path"));
        return;
    }
    QString server5 = QString("%1/server5.sql").arg(QDir::tempPath());
    QString airlog = QString("%1/airlog.sql").arg(QDir::tempPath());
    QString cafe5 = QString("%1/cafe5.sql").arg(QDir::tempPath());
    QString log = QString("%1/info.sql").arg(QDir::tempPath());
    QDir d;
    d.remove(server5);
    d.remove(airlog);
    d.remove(cafe5);
    QFile::copy(":/res/server5.sql", server5);
    QFile::copy(":/res/airlog.sql", airlog);
    QFile::copy(":/res/cafe5.sql", cafe5);
    QFile::setPermissions(server5, QFile::ReadOther | QFile::WriteOther);
    QFile::setPermissions(airlog, QFile::ReadOther | QFile::WriteOther);
    QFile::setPermissions(cafe5, QFile::ReadOther | QFile::WriteOther);
    Database db;
    if (db.open(ui->leMainHost->text(), "", ui->leMainusername->text(), ui->leMainPassword->text())) {
        bool dbempty = true;
        if (db.execDirect("create database server5 default character set utf8 collate utf8_general_ci") == false) {
            db.execDirect("use server5");
            db.execDirect("show tables");
            if (db.rowCount() > 0) {
                dbempty = false;
            }
        }
        if (dbempty) {
            QString cmd = QString("\"%1/bin/mysql\" -uroot -p%2 server5")
                    .arg(ui->leMariaDBInstallationPath->text())
                    .arg(ui->leMainPassword->text());
            QProcess p;
            p.setStandardInputFile(server5);
            p.setStandardOutputFile(log);
            p.start(cmd);
            p.waitForFinished();
            QString out = p.readAllStandardError();
            if (out.isEmpty() == false) {
                QMessageBox::critical(this, tr("Error"), out);
                return;
            }
            ui->lbServerDBStatus->setText(tr("Created"));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Server database exists"));
        }

        dbempty = true;
        if (db.execDirect("create database airlog default character set utf8 collate utf8_general_ci") == false) {
            db.execDirect("use airlog");
            db.execDirect("show tables");
            if (db.rowCount() > 0) {
                dbempty = false;
            }
        }
        if (dbempty) {
            QString cmd = QString("\"%1/bin/mysql\" -uroot -p%2 airlog")
                    .arg(ui->leMariaDBInstallationPath->text())
                    .arg(ui->leMainPassword->text());
            QProcess p;
            p.setStandardInputFile(airlog);
            p.setStandardOutputFile(log);
            p.start(cmd);
            p.waitForFinished();
            QString out = p.readAllStandardError();
            if (out.isEmpty() == false) {
                QMessageBox::critical(this, tr("Error"), out);
                return;
            }
            ui->lbLogDBStatus->setText(tr("Created"));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Log database exists"));
        }
        dbempty = true;
        if (db.execDirect("create database cafe5 default character set utf8 collate utf8_general_ci") == false) {
            db.execDirect("use cafe5");
            db.execDirect("show tables");
            if (db.rowCount() > 0) {
                dbempty = false;
            }
        }
        if (dbempty) {
            QString cmd = QString("\"%1/bin/mysql\" -uroot -p%2 cafe5")
                    .arg(ui->leMariaDBInstallationPath->text())
                    .arg(ui->leMainPassword->text());
            QProcess p;
            p.setStandardInputFile(cafe5);
            p.setStandardOutputFile(log);
            p.start(cmd);
            p.waitForFinished();
            QString out = p.readAllStandardError();
            if (out.isEmpty() == false) {
                QMessageBox::critical(this, tr("Error"), out);
                return;
            }
            ui->lbWorkingDBStatus->setText(tr("Created"));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Working database exists"));
        }
    } else {
        QMessageBox::critical(this, tr("Error"), db.lastDbError());
        return;
    }
}

void Monitor::on_btnSetMariaDBInstallationPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("MariaDB"), "C:\\Program Files");
    if (path.isEmpty() == false) {
        if (QFile::exists(QString("%1\\bin\\mariadb.exe").arg(path))) {
            ui->leMariaDBInstallationPath->setText(path);
        } else {
            QMessageBox::critical(this, tr("MariaDB"), tr("The selected directory is not valid MariaDB path"));
        }
    }
}

void Monitor::on_btnRegister_clicked()
{
    C5Crypt c;
    QString cryptoKey = c.driveKey();
    if (cryptoKey.isEmpty()) {
        cryptoKey = c.machineUuid();
    }

#ifdef QT_DEBUG
    QString address = "10.1.0.2";
#else
    QString address = "breezedevs.ru";
#endif
    QUrl url(QString("https://%1/registerlicense.php").arg(address));
    QNetworkAccessManager manager;
    QUrlQuery postData;
    postData.addQueryItem("email", ui->leRegEmail->text());
    postData.addQueryItem("password", ui->leRegPassword->text());
    postData.addQueryItem("key", cryptoKey);
    postData.addQueryItem("r", "1");
    QSslConfiguration sslConfig;
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    QNetworkRequest request(url);
    request.setSslConfiguration(sslConfig);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *http = manager.post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    QEventLoop eventLoop;
    QObject::connect(http, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    if (http->error() == QNetworkReply::NoError) {
        QJsonParseError je;
        QByteArray replyData = http->readAll();
        qDebug() << replyData;
        QJsonDocument jd = QJsonDocument::fromJson(replyData, &je);
        if (je.error != QJsonParseError::NoError) {
            QMessageBox::critical(this, tr("Error"), je.errorString());
            return;
        }
        QJsonObject jo = jd.object();
        if (jo["reply"].toInt() == 0) {
            QMessageBox::critical(this, tr("Error"), jo["message"].toString());
            return;
        }
        QJsonArray ja = jo["licenses"].toArray();
        DlgLicenses d(this);
        for (int i = 0; i < ja.count(); i++) {
            QJsonObject jl = ja[i].toObject();
            d.addLicense(jl["id"].toInt(), jl["start"].toString(), jl["end"].toString(), jl["type"].toInt());
        }
        d.exec();
        if (d.result() > 0) {
            http->deleteLater();
            postData.removeQueryItem("r");
            postData.addQueryItem("r", "2");
            postData.addQueryItem("lid", QString::number(d.result()));
            http = manager.post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
            QEventLoop eventLoop;
            QObject::connect(http, SIGNAL(finished()), &eventLoop, SLOT(quit()));
            eventLoop.exec();
            if (http->error() == QNetworkReply::NoError) {
                replyData = http->readAll();
                qDebug() << replyData;
                jd = QJsonDocument::fromJson(replyData, &je);
                if (je.error != QJsonParseError::NoError) {
                    QMessageBox::critical(this, tr("Error"), je.errorString());
                    return;
                }
                jo = jd.object();
                if (jo["reply"].toInt() == 0) {
                    QMessageBox::critical(this, tr("Error"), jo["message"].toString());
                    return;
                }
                replyData = QByteArray::fromBase64(jo["license"].toString().toLatin1());
                C5License lic;
                if (lic.write(replyData) == false) {
                    QMessageBox::critical(this, tr("Error"), tr("Cannot open file to write license information"));
                    return;
                }
                readLicense();
            } else {
                QMessageBox::critical(this, tr("Error"), http->errorString());
            }
        }
    } else {
        QMessageBox::critical(this, tr("Error"), http->errorString());
    }
    http->deleteLater();
}

void Monitor::readLicense()
{
    ui->grLicense->setVisible(false);
    C5License l;
    QDate d;
    QString t;
    if (l.read(d, t)) {
        ui->grLicense->setVisible(true);
        ui->grRegistration->setVisible(false);
        ui->lbLicenseDate->setText(QString("%1: %2").arg(tr("Expiration"), d.toString("dd/MM/yyyy")));
        ui->lbLicenseType->setText(QString("%1: %2").arg(tr("License type"), t));
        if (!d.isValid() || d < QDate::currentDate()) {
            ui->lbLicenseDate->setStyleSheet("color: red");
        }
    }
}


void Monitor::on_btnStopServer_clicked()
{
    bool stopped = false;
    do {
        QString r = OS::p("sc stop Breeze 2:64:256");
        QThread::sleep(3);
        if (r.contains("STOP_PENDING")) {
            continue;
        }
        stopped = true;
    } while (!stopped);
    QMessageBox::information(this, tr("Server"), tr("The server was stopped"));
}

void Monitor::on_btnStartServer_clicked()
{
    QString r;
    bool started = false;
    do {
        r = OS::p("sc start Breeze");
        qDebug() << r;
        QThread::sleep(3);
        if (r.contains("START_PENDING")) {
            continue;
        }
        started = true;
    } while (!started);
    r = OS::p("sc query Breeze");
    qDebug() << r;
    if (r.contains("RUNNING")) {
        QMessageBox::information(this, tr("Server"), tr("The server was started"));
    } else {
        QMessageBox::information(this, tr("Server"), r);
    }
}

void Monitor::on_btnRestartServer_clicked()
{
    QString r;
    bool stopped = false;
    do {
        r = OS::p("sc stop Breeze 2:64:256");
        if (r.contains("STOP_PENDING")) {
            QThread::sleep(3);
            continue;
        }
        stopped = true;
    } while (!stopped);
    bool started = false;
    do {
        r = OS::p("sc start Breeze");
        qDebug() << r;
        if (r.contains("START_PENDING")) {
            QThread::sleep(3);
            continue;
        }
        started = true;
    } while (!started);
    r = OS::p("sc query Breeze");
    qDebug() << r;
    if (r.contains("RUNNING")) {
        QMessageBox::information(this, tr("Server"), tr("The server was started"));
    } else {
        QMessageBox::information(this, tr("Server"), r);
    }
}

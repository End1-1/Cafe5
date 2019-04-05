#include "updatethread.h"
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication>
#include <QFile>
#include <QDir>

UpdateThread::UpdateThread(QObject *parent) :
    QThread(parent)
{
    fUpdateVersion = 0;
    fUpdateMySelf = false;
    fNetManager = new QNetworkAccessManager(this);
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(go()), SLOT(goUpdate()));
}

UpdateThread::~UpdateThread()
{
    fNetManager->deleteLater();
    qDebug() << "EXIT THREAD";
}

void UpdateThread::run()
{
    emit message(tr("Check for update"));
    emit go();
    exec();
}

void UpdateThread::checkForUpdate()
{
    QString checkFile = fServerAddress + "/cafe5/updates/check.json";
    QNetworkRequest request(checkFile);
    QNetworkReply *r = fNetManager->get(request);
    connect(r, SIGNAL(finished()), SLOT(checkDownloadFinished()));
    connect(r, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(netError(QNetworkReply::NetworkError)));
}

void UpdateThread::checkFor7z()
{
    QFile sevenz("./7z.exe");
    if (sevenz.exists()) {
        downloadUpdatePackage();
        return;
    }
    emit message(tr("7-Zip missing, try download it from server"));
    emit progress(0);
    QString checkFile = fServerAddress + "/cafe5/updates/7z.lo";
    QNetworkRequest request(checkFile);
    QNetworkReply *r = fNetManager->get(request);
    connect(r, SIGNAL(finished()), SLOT(sevenzDownloadFinished()));
    connect(r, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(netError(QNetworkReply::NetworkError)));
    connect(r, SIGNAL(downloadProgress(qint64, qint64)), SLOT(downloadProgress(qint64, qint64)));
}

void UpdateThread::downloadUpdatePackage()
{
    progress(0);
    QString checkFile = fServerAddress + QString("/cafe5/updates/%1").arg(fUpdatePackage);
    QNetworkRequest request(checkFile);
    QNetworkReply *r = fNetManager->get(request);
    connect(r, SIGNAL(finished()), SLOT(downloadUpdatePackageFinished()));
    connect(r, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(netError(QNetworkReply::NetworkError)));
    connect(r, SIGNAL(downloadProgress(qint64, qint64)), SLOT(downloadProgress(qint64, qint64)));
}

void UpdateThread::extractData()
{
    QDir dir;
    fPathToExtract = dir.tempPath() + "/" + fUpdatePackage.remove(".7z", Qt::CaseInsensitive);
    dir.mkpath(fPathToExtract);
    QStringList args;
    args << "x";
    args << "-aoa";
    args << "-o" + fPathToExtract + "/";
    args << fFileToExtract;
    QProcess *proc = new QProcess(this);
    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(sevenzExtractFinished(int, QProcess::ExitStatus)));
    connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    proc->start("./7z.exe", args);
}

void UpdateThread::startUpdate()
{
    bool error = false;
    emit message(tr("Copying new files"));
    for (int i = 0; i < fFilesForCopy.count(); i++) {
        QString tmpFileName = fFilesForCopy.at(i).toString();
        QFile file(fFileToExtract + "\\" + tmpFileName);
        if (QFile::exists(fInstallPath + tmpFileName)) {
            QFile::remove(fInstallPath + tmpFileName);
        }
        if (!file.copy(fInstallPath + tmpFileName)) {
            if (tmpFileName == "updater.exe") {
                fUpdateMySelf = true;
                continue;
            }
            error = true;
            fUpdateMySelf = false;
            emit message(tr("Copy file failed. Please, close any program and try to update again"));
            emit allDone(tr("File was not copyed: ") + tmpFileName);
            break;
        }
    }
    cleanup(error);
}

void UpdateThread::cleanup(bool error)
{
    emit message(tr("Cleanup"));
    QFile bat(fPathToExtract + "/../clear.bat");
    bat.open(QIODevice::WriteOnly);
    if (fUpdateMySelf) {
        bat.write("taskkill /f /im updater.exe\r\n");
        bat.write("copy /Y ");
        bat.write(QString(fPathToExtract + "/updater.exe ").replace("/", "\\").toUtf8());
        bat.write(QString(qApp->applicationDirPath().toUtf8() + "/updater.exe").replace("/", "\\").toUtf8());
        bat.write("\r\n");
    }
    bat.write("rd /S /Q ");
    bat.write(fPathToExtract.toUtf8().replace("/", "\\"));
    bat.write("\r\n");
    bat.write("del ");
    bat.write(fFileToExtract.toUtf8().replace("/", "\\"));
    bat.write("\r\n");
    bat.write(QString("del " + fPathToExtract + "/../clear.bat\r\n").replace("/", "\\").toUtf8());
    bat.close();
    QStringList args;
    args << "/c";
    args << QString(fPathToExtract + "/../clear.bat").replace("/", "\\");
    QProcess *p = new QProcess();
    p->setProgram("cmd");
    p->setArguments(args);
    p->startDetached();
    allDone(error ? tr("Not updated") : tr("Updated!"));
}

void UpdateThread::downloadProgress(qint64 received, qint64 total)
{
    emit progress((received * 100) / total);
}

void UpdateThread::goUpdate()
{
    checkForUpdate();
}

void UpdateThread::checkDownloadFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonDocument jdoc = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();
    QJsonObject jobj = jdoc.object();
    qDebug() << jobj;
    emit message(QString("%1: %2, %3: %4").arg(tr("Installed version")).arg(fUpdateVersion).arg("Latest version").arg(jobj["version"].toInt()));
    if (fUpdateVersion == jobj["version"].toInt()) {
        emit allDone(tr("No updates"));
        return;
    }
    emit message(tr("Need to update"));
    fUpdatePackage = jobj["package"].toString();
    fFilesForCopy = jobj["files"].toArray();
    fDatabaseQueries = jobj["database"].toArray();
    checkFor7z();
}

void UpdateThread::sevenzDownloadFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QFile file("7z.exe");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(reply->readAll());
        file.close();
        downloadUpdatePackage();
    } else {
        allDone(tr("Filed to download 7-zip"));
    }
    reply->deleteLater();
}

void UpdateThread::downloadUpdatePackageFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QDir dir;
    fFileToExtract = dir.tempPath() + "/" + fUpdatePackage;
    QFile file(fFileToExtract);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(reply->readAll());
        file.close();
        extractData();
    } else {
        allDone("Update failed. Cannot save downloaded archive to the directory");
    }
    reply->deleteLater();
}

void UpdateThread::sevenzExtractFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    startUpdate();
}

void UpdateThread::netError(QNetworkReply::NetworkError code)
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    emit allDone(tr("Network error") + ": " + QString::number(code) + ", " + reply->errorString());
}

void UpdateThread::readyReadStandardOutput()
{
    QProcess *proc = static_cast<QProcess*>(sender());
    QByteArray ba = proc->readAllStandardOutput();
    emit message(ba);
}

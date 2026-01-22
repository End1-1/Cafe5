#include "updatemanger.h"
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QSaveFile>
#include <QMessageBox>
#include <QCoreApplication>

UpdateManager::UpdateManager(const QString &archiveUrl,
                             const QString &appDir,
                             const QString &appExe,
                             QObject *parent)
    : QObject(parent),
      m_archiveUrl(archiveUrl),
      m_appDir(QDir(appDir).absolutePath()),
      m_appExe(appExe),
      m_tmpDir()
{
}

void UpdateManager::start()
{
    setOverallProgress(0.0);
    downloadArchive();
}

void UpdateManager::downloadArchive()
{
    QNetworkRequest req((QUrl(m_archiveUrl)));
    auto *r = m_nam.get(req);
    QString archivePath = m_tmpDir.path() + "/files.7z";
    QFile *f = new QFile(archivePath);

    if(!f->open(QIODevice::WriteOnly)) {
        emit error("Cannot create temp archive");
        return;
    }

    connect(r, &QNetworkReply::readyRead, this, [r, f]() {
        f->write(r->readAll());
    });
    connect(r, &QNetworkReply::downloadProgress, this, [this](qint64 done, qint64 total) {
        setDownloadProgressBytes(done, total);
    });
    connect(r, &QNetworkReply::finished, this, [this, r, f, archivePath]() {
        f->close();
        f->deleteLater();

        if(r->error() != QNetworkReply::NoError) {
            emit error("Archive download failed");
            return;
        }

        extractArchive(archivePath);
    });
}

void UpdateManager::extractArchive(const QString &archivePath)
{
    QString sevenZip = QCoreApplication::applicationDirPath() + "/7z.exe";

    if(!QFile::exists(sevenZip)) {
        emit error("7z.exe not found");
        return;
    }

    QProcess p;
    p.start(sevenZip, {"x", archivePath, "-o" + m_tmpDir.path(), "-y"});

    if(!p.waitForFinished(-1) || p.exitCode() != 0) {
        emit error("Extraction failed");
        return;
    }

    setOverallProgress(0.80);
    promptAndTerminateRunning();
}

void UpdateManager::promptAndTerminateRunning()
{
    QStringList exeNames;
    exeNames << QFileInfo(m_appExe).fileName();
    const QString text =
        "Как только нажмете ДА, все файлы что не закрыты, будут принудительно закрыты и произведено обновление.\n\nПродолжить?";

    if(QMessageBox::question(nullptr, "Обновление", text,
                             QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        emit error("Update cancelled.");
        return;
    }

    const QStringList running = findProcessesHoldingOrMatching(exeNames);

    for(const QString &p : running) {
        QString err;

        if(!terminateProcessByName(p, &err)) {
            emit error(err);
            return;
        }
    }

    replaceFiles();
}

void UpdateManager::replaceFiles()
{
    QDir tmp(m_tmpDir.path());
    QStringList files = tmp.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    int n = files.size();

    for(int i = 0; i < n; i++) {
        QString rel = files[i];
        QString src = tmp.filePath(rel);
        QString dst = QDir(m_appDir).filePath(rel);
        QString err;

        if(!ensureDirForFile(dst, &err) || !copyFileAtomic(src, dst, &err)) {
            emit error(err);
            return;
        }

        setOverallProgress(0.80 + 0.20 * (double(i + 1) / double(n)));
    }

    launchUpdatedApp();
}

void UpdateManager::launchUpdatedApp()
{
    QString exePath = QDir(m_appDir).filePath(m_appExe);
    QProcess::startDetached(exePath, {}, m_appDir);
    emit finished();
}

void UpdateManager::setOverallProgress(double v01)
{
    if(!m_pb)
        return;

    if(v01 < 0)
        v01 = 0;

    if(v01 > 1)
        v01 = 1;

    m_pb->setValue(int(v01 * 100.0));
}

void UpdateManager::setDownloadProgressBytes(qint64 done, qint64 total)
{
    if(total <= 0)
        return;

    double v = double(done) / double(total);
    setOverallProgress(0.80 * v);
}

QStringList UpdateManager::findProcessesHoldingOrMatching(const QStringList &exeNames) const
{
#ifdef Q_OS_WIN
    QStringList running;
    QProcess p;
    p.start("tasklist", {"/fo", "csv", "/nh"});
    p.waitForFinished();
    QString out = QString::fromLocal8Bit(p.readAllStandardOutput());

    for(auto &name : exeNames)
        if(out.contains(name, Qt::CaseInsensitive))
            running << name;

    return running;
#else
    return {};
#endif
}

bool UpdateManager::terminateProcessByName(const QString &exeName, QString *err) const
{
#ifdef Q_OS_WIN
    QProcess p;
    p.start("taskkill", {"/IM", exeName, "/F"});
    p.waitForFinished();

    if(p.exitCode() != 0 && err)
        *err = "taskkill failed: " + exeName;

#endif
    return true;
}

bool UpdateManager::ensureDirForFile(const QString &absoluteFilePath, QString *err) const
{
    QDir d = QFileInfo(absoluteFilePath).dir();

    if(d.exists() || d.mkpath("."))
        return true;

    if(err)
        *err = "Cannot create dir " + d.absolutePath();

    return false;
}

bool UpdateManager::copyFileAtomic(const QString &src, const QString &dst, QString *err) const
{
    QFile in(src);

    if(!in.open(QIODevice::ReadOnly)) {
        if(err)
            *err = "Cannot read " + src;

        return false;
    }

    QSaveFile out(dst);

    if(!out.open(QIODevice::WriteOnly)) {
        if(err)
            *err = "Cannot write " + dst;

        return false;
    }

    out.write(in.readAll());

    if(!out.commit()) {
        if(err)
            *err = "Commit failed " + dst;

        return false;
    }

    return true;
}

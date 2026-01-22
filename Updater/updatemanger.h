#pragma once

#include <QObject>
#include <QTemporaryDir>
#include <QNetworkAccessManager>
#include <QProgressBar>

struct FileItem {
    QString path;
};

class UpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit UpdateManager(const QString &archiveUrl,
                           const QString &appDir,
                           const QString &appExe,
                           QObject *parent = nullptr);

    void setProgressBar(QProgressBar *pb) { m_pb = pb; }
    void start();

signals:
    void error(const QString &msg);
    void finished();

private:
    void downloadArchive();
    void extractArchive(const QString &archivePath);
    void promptAndTerminateRunning();
    void replaceFiles();
    void launchUpdatedApp();

    void setOverallProgress(double v01);
    void setDownloadProgressBytes(qint64 done, qint64 total);

    QStringList findProcessesHoldingOrMatching(const QStringList &exeNames) const;
    bool terminateProcessByName(const QString &exeName, QString *err) const;

    bool ensureDirForFile(const QString &absoluteFilePath, QString *err) const;
    bool copyFileAtomic(const QString &src, const QString &dst, QString *err) const;

private:
    QNetworkAccessManager m_nam;
    QString m_archiveUrl;
    QString m_appDir;
    QString m_appExe;

    QTemporaryDir m_tmpDir;
    QProgressBar* m_pb = nullptr;
};

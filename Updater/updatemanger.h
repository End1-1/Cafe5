#pragma once

#include <QObject>
#include <QProgressBar>
#include <QString>
#include <QStringList>
#include <QWidget>

class UpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit UpdateManager(const QString &setupUrl,
                           const QString &setupFileName,
                           const QString &appModule,
                           const QString &moduleInstallDir,
                           QObject *parent = nullptr);

    void setProgressBar(QProgressBar *pb) { m_pb = pb; }
    void setHostWindow(QWidget *w) { m_hostWindow = w; }
    void start();

signals:
    void statusChanged(const QString &text);
    void error(const QString &msg);
    void finished();
    void progress(qint64 done, qint64 total);
    /** Hide AlwaysOnTop UI so the UAC consent dialog is not covered. */
    void aboutToElevate();

private:
    void downloadSetup();
    void downloadFinishedOk(const QString &setupPath);
    void launchSetup(const QString &setupPath);
    void setProgress(qint64 done, qint64 total);
    void setBusyProgress();
    QStringList processesToKill() const;

    QString m_setupUrl;
    QString m_setupFileName;
    QString m_appModule;
    QString m_moduleInstallDir;
    QString m_downloadDir;
    QProgressBar *m_pb = nullptr;
    QWidget *m_hostWindow = nullptr;
};

#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H

#include <QThread>
#include <QNetworkReply>
#include <QProcess>
#include <QJsonArray>

class QNetworkAccessManager;

class UpdateThread : public QThread
{
    Q_OBJECT

public:
    UpdateThread(QObject *parent = nullptr);

    ~UpdateThread();

    int fUpdateVersion;

    QString fServerAddress;

    QString fUpdatePackage;

protected:
    virtual void run();

private:
    bool fUpdateMySelf;

    QString fInstallPath;

    QString fFileToExtract;

    QString fPathToExtract;

    QJsonArray fFilesForCopy;

    QJsonArray fDatabaseQueries;

    QNetworkAccessManager *fNetManager;

    void checkForUpdate();

    void checkFor7z();

    void downloadUpdatePackage();

    void extractData();

    void startUpdate();

    void cleanup(bool error);

private slots:
    void downloadProgress(qint64 recieved, qint64 total);

    void goUpdate();

    void checkDownloadFinished();

    void sevenzDownloadFinished();

    void downloadUpdatePackageFinished();

    void sevenzExtractFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void netError(QNetworkReply::NetworkError code);

    void readyReadStandardOutput();

signals:
    void message(const QString &str);

    void allDone(const QString &str);

    void go();

    void download(bool v);

    void progress(int p);

};

#endif // UPDATETHREAD_H

#ifndef C5DATASYNCTHREAD_H
#define C5DATASYNCTHREAD_H

#include <QThread>
#include <QDate>

class C5DataSyncThread : public QThread
{
    Q_OBJECT

public:
    C5DataSyncThread(const QStringList &dbParamsSrc, const QStringList &dbParamsDst, int infoRow, QObject *parent = 0);

    void runWithDate(const QDate &d1, const QDate &d2, bool removeFromSource);

    void runUploadData();

    ~C5DataSyncThread();

protected:
    virtual void run();

private:
    enum RunMode {rmNone = 0, rmDownloadSales, rmUploadChanges};

    RunMode fRunMode;

    QStringList fDbParamsSrc;

    QStringList fDbParamsDst;

    int fInfoRow;

    QDate fDate1;

    QDate fDate2;

    bool fRemoveFromSrc;

    void downloadData();

    void uploadData();

signals:
    void statusUpdated(const QString &name, int row, const QString &msg);

    void statusUploadUpdated(int row, int column, const QString &msg);

    void progressBarMax(int max);

    void progressBarValue(int value);

    void uploadLastChangeDate(const QString &date);

    void errLog(const QString &msg);
};

#endif // C5DATASYNCTHREAD_H

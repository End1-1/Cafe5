#ifndef C5REPLICATION_H
#define C5REPLICATION_H

#include "c5database.h"
#include <QObject>

class C5Replication : public QObject
{
    Q_OBJECT
public:
    explicit C5Replication();

    void start(const char *slot);

    bool ret(C5Database &db1, C5Database &db2);

    void downloadDataFromServer(const QStringList &src, const QStringList &dst);

    bool uploadDataToServer(const QStringList &src, const QStringList &dst);

    bool fIgnoreErrors;

signals:
    void finished();

    void haveChanges(bool);

    void progress(const QString&);

public slots:
    bool uploadToServer();

    void downloadFromServer();

private:
    void updateTable(C5Database &dr, C5Database &db, int &step, int steps, const QString &tableName);

};

#endif // C5REPLICATION_H

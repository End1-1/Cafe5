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

signals:
    void finished();

    void progress(const QString&);

public slots:
    bool uploadToServer();

    void downloadFromServer();

private:
    void updateTable(C5Database &dr, C5Database &db, int &step, int steps, const QString &tableName);

};

#endif // C5REPLICATION_H

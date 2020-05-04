#ifndef C5REPLICATION_H
#define C5REPLICATION_H

#include "c5database.h"
#include <QObject>

class C5Replication : public QObject
{
    Q_OBJECT
public:
    explicit C5Replication(QObject *parent = nullptr);

    void start();

    bool ret(C5Database &db1, C5Database &db2);

signals:
    void finished();

public slots:
    bool process();
};

#endif // C5REPLICATION_H

#ifndef PRINTRECEIPTGROUP_H
#define PRINTRECEIPTGROUP_H

#include "c5database.h"
#include <QObject>

class PrintReceiptGroup : public QObject
{
    Q_OBJECT
public:
    explicit PrintReceiptGroup(QObject *parent = nullptr);

    void print(const QString &id, C5Database &db, int rw);

    void print2(const QString &id, C5Database &db);

signals:

public slots:
};

#endif // PRINTRECEIPTGROUP_H

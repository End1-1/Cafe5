#ifndef PRINTRECEIPTGROUP_H
#define PRINTRECEIPTGROUP_H

#include "database.h"
#include <QObject>

class PrintReceiptGroup : public QObject
{
    Q_OBJECT
public:
    explicit PrintReceiptGroup(QObject *parent = nullptr);

    void print(const QString &id, Database &db, int rw);

    void print2(const QString &id, Database &db, const QString &printerName);

signals:

public slots:
};

#endif // PRINTRECEIPTGROUP_H

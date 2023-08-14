#ifndef PRINTRECEIPT_H
#define PRINTRECEIPT_H

#include "c5database.h"
#include <QObject>

class PrintReceipt : public QObject
{
    Q_OBJECT
public:
    explicit PrintReceipt(QObject *parent = nullptr);

    void print(const QString &id, C5Database &db);

signals:

public slots:
};

#endif // PRINTRECEIPT_H

#ifndef PRINTRECEIPT_H
#define PRINTRECEIPT_H

#include <QObject>

class PrintReceipt : public QObject
{
    Q_OBJECT
public:
    explicit PrintReceipt(QObject *parent = nullptr);

    void print(const QString &id);

signals:

public slots:
};

#endif // PRINTRECEIPT_H

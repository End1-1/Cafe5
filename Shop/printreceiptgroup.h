#ifndef PRINTRECEIPTGROUP_H
#define PRINTRECEIPTGROUP_H
#include <QObject>

class PrintReceiptGroup : public QObject
{
    Q_OBJECT
public:
    explicit PrintReceiptGroup(QObject *parent = nullptr);

    void print(const QString &id, int rw);

    void print2(const QString &id);

    void print3(const QString &id);

signals:

public slots:
};

#endif // PRINTRECEIPTGROUP_H

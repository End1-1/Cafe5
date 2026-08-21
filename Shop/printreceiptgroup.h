#ifndef PRINTRECEIPTGROUP_H
#define PRINTRECEIPTGROUP_H
#include <QObject>

class C5User;
class QJsonObject;

class PrintReceiptGroup : public QObject
{
    Q_OBJECT
public:
    explicit PrintReceiptGroup(QObject *parent = nullptr);

    void print(const QString &id, int rw);

    /** Load closed sale from server and print thermal receipt. */
    static void print2(const QString &id, C5User *user, QObject *context);

    /** Print from view-order/get JSON (header + goods). */
    static void printOrder(const QJsonObject &jo);

    void print3(const QString &id);

signals:

public slots:
};

#endif // PRINTRECEIPTGROUP_H

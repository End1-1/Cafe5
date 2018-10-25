#ifndef C5PRINTRECEIPTTHREAD_H
#define C5PRINTRECEIPTTHREAD_H

#include <QThread>
#include <QJsonArray>
#include <QJsonObject>

class C5PrintReceiptThread : public QThread
{
    Q_OBJECT

public:
    C5PrintReceiptThread(const QJsonObject &header, const QJsonArray &body, const QString &printer, QObject *parent = 0);

protected:
    virtual void run();

private:
    QJsonObject fHeader;

    QJsonArray fBody;

    QString fPrinter;
};

#endif // C5PRINTRECEIPTTHREAD_H

#ifndef C5PRINTRECEIPTTHREAD_H
#define C5PRINTRECEIPTTHREAD_H

#include <QThread>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

class C5PrintReceiptThread : public QThread
{
    Q_OBJECT

public:
    C5PrintReceiptThread(const QStringList &dbParams, const QJsonObject &header, const QJsonArray &body, const QString &printer, QObject *parent = nullptr);

    bool fBill;

protected:
    virtual void run();

private:
    QJsonObject fHeader;

    QJsonArray fBody;

    QString fPrinter;

    QStringList fDbParams;

private slots:
    void print();

signals:
    void startPrint();
};

#endif // C5PRINTRECEIPTTHREAD_H

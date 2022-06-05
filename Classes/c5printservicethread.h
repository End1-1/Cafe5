#ifndef C5PRINTSERVICETHREAD_H
#define C5PRINTSERVICETHREAD_H

#include <QObject>
#include <QMap>

class C5PrintServiceThread : public QObject
{
    Q_OBJECT

public:
    C5PrintServiceThread(const QString &header, QObject *parent = nullptr);

    ~C5PrintServiceThread();

    bool run();

    QString fReprintList;

    bool fUseAliases;

    bool fBooking;

private:
    QString fHeader;

    QMap<QString, QVariant> fHeaderData;

    QMap<QString, QVariant> fPreorderData;

    QList<QMap<QString, QVariant> > fBodyData;

    QMap<QString, QString> fPrinterAliases;

    void print(QString printer, const QString &side, bool reprint);
};

#endif // C5PRINTSERVICETHREAD_H

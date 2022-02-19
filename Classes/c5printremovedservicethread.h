#ifndef C5PRINTREMOVEDSERVICETHREAD_H
#define C5PRINTREMOVEDSERVICETHREAD_H

#include <QThread>
#include <QMap>
#include <QVariant>

class C5PrintRemovedServiceThread : public QThread
{
    Q_OBJECT

public:
    C5PrintRemovedServiceThread(const QString &id, QObject *parent = 0);

protected:
    virtual void run();

private:
    QString fId;

    QMap<QString, QVariant> fBody;

    QMap<QString, QVariant> fHeader;

    QMap<QString, QString> fPrinterAliases;

    void print(QString printName);
};

#endif // C5PRINTREMOVEDSERVICETHREAD_H

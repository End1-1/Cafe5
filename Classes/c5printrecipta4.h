#ifndef C5PRINTRECIPTA4_H
#define C5PRINTRECIPTA4_H

#include <QObject>

class C5PrintReciptA4 : public QObject
{
    Q_OBJECT
public:
    explicit C5PrintReciptA4(const QString &orderid, QObject *parent = nullptr);
    bool print(QString &err);

private:
    QString fOrderUUID;
signals:

};

#endif // C5PRINTRECIPTA4_H

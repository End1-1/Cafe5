#ifndef IDRAM_H
#define IDRAM_H

#include <QObject>

class Idram : public QObject
{
    Q_OBJECT
public:
    explicit Idram(QObject *parent = nullptr);
    static bool check(const QString &server, const QString &sessionid, const QString &orderid, double &amount, QByteArray &out);

signals:

};

#endif // IDRAM_H

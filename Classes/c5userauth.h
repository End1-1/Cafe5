#ifndef C5USERAUTH_H
#define C5USERAUTH_H

#include "c5database.h"
#include <QString>

class C5UserAuth : public QObject
{
    Q_OBJECT
public:
    C5UserAuth(C5Database &db, QObject *parent = nullptr);

    bool authByPinPass(const QString &pin, const QString &pass, int &id, int &group, QString &name);

    bool authByPass(const QString &pass, int &id, int &group, QString &name);

    bool enterWork(const QString &pass);

    bool leaveWork(const QString &pass);

    QString error() const;

private:
    C5Database &fDb;

    QString fError;
};

#endif // C5USERAUTH_H

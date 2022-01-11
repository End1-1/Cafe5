#ifndef DBOPREORDER_H
#define DBOPREORDER_H

#include "dbdata.h"

class DbOPreorder : public DbData
{
public:
    DbOPreorder();

    QMap<QString, QMap<QString, QVariant> > fOrderData;

    QString guestName(const QString &id);

    double amount(const QString &id);

    int table(const QString &id);

private:
    virtual void getFromDatabase() override;

    QVariant get(const QString &id, const QString &key);
};

#endif // DBOPREORDER_H

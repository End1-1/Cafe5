#ifndef DBOHEADER_H
#define DBOHEADER_H

#include "dbdata.h"
#include <QMap>

class DbOHeader : public DbData
{
public:
    DbOHeader();

    QMap<QString, QMap<QString, QVariant> > fOrderData;

    QMap<int, QString> fTableOrder;

    QMap<int, int> fStaffTable;

    QDate dateOpen(const QString &id);

    QTime timeOpen(const QString &id);

    int staff(const QString &id);

    int state(const QString &id);

    double amount(const QString &id);

    int precheck(const QString &id);

    int print(const QString &id);

    QString comment(const QString &id);

private:
    virtual void getFromDatabase() override;

    QVariant get(const QString &id, const QString &key);
};

#endif // DBOHEADER_H

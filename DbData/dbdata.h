#ifndef DBDATA_H
#define DBDATA_H

#include "c5database.h"

class DbData : public QObject
{
    Q_OBJECT
public:
    DbData(const QString &tableName, const QString &cond = "");

    static void setDBParams(const QStringList &dbParams);

    virtual QString name(int id);

    QList<int> list();

    virtual void refresh();

protected:
    C5Database fDb;

    QString fTable;

    QString fCondition;

    QMap<int, QMap<QString, QVariant> > fData;

    QVariant get(int id, const QString &key);

    virtual void getFromDatabase();

private:
    static QStringList fDbParams;
};

#endif // DBDATA_H

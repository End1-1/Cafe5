#ifndef DBDATA_H
#define DBDATA_H

#include <QObject>
#include <QMap>
#include <QVariant>

class DbData : public QObject
{
    Q_OBJECT
public:
    DbData(const QString &tableName, const QString &cond = "");

    DbData(int id);

    static void setDBParams(const QStringList &dbParams);

    virtual QString name(int id);

    int id();

    QList<int> list();

    virtual void refresh();

protected:
    int fId;

    static QStringList fDbParams;

    QString fTable;

    QString fCondition;

    QMap<int, QMap<QString, QVariant> > fData;

    QVariant get(int id, const QString &key);

    virtual void getFromDatabase();
};

#endif // DBDATA_H

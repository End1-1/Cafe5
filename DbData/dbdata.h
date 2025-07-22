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

    virtual QString name(int id);

    int id();

    QList<int> list();

    virtual void refresh();

    QVariant get(int id, const QString &key);

    void updateField(int id, const QString &field, const QVariant &value);

protected:
    int fId;

    static QStringList fDbParams;

    QString fTable;

    QString fCondition;

    QMap<int, QMap<QString, QVariant> > fData;

    virtual void getFromDatabase();
};

#endif // DBDATA_H

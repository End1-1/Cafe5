#ifndef DATAONLINE_H
#define DATAONLINE_H

#include <QObject>
#include <QHash>
#include <QVariant>

#define dbo(t, f, i) DataOnline::instance()->value(t, f, i)
#define dbo_int(t, f, i) dbo(t, f, i).toInteger()
#define dbo_str(t, f, i) dbo(t, f, i).toString()

class DataOnline : public QObject
{
    Q_OBJECT
public:
    explicit DataOnline();

    QVariant value(const QString &table, const QString &field, int id);

    QHash<QString, QString> fTableQueries;

    QHash<QString, QHash<QString, int> > fColumnNames;

    static DataOnline* instance();

private:
    QHash<QString, QHash<int, int> > fIdRow;

    QHash<QString, QVector<QJsonArray > > fValues;

    static QHash<QString, DataOnline*> fInstances;

signals:

};

#endif // DATAONLINE_H

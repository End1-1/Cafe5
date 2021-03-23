#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QMutex>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

class Database
{
public:
    explicit Database();
    ~Database();
    QSqlDatabase fSqlDatabase;
    bool open(const QString &host, const QString &schema, const QString &username, const QString &password);
    bool exec(const QString &query);
    bool insert(const QString &table);
    bool insert(const QString &table, int &id);
    bool update(const QString &table);
    inline int rowCount() {return fQuery->size();}
    inline bool next() {return fQuery->next();}
    inline QVariant value(int column) const {return fQuery->value(column);}
    void close();
    QSqlQuery *fQuery;
    QVariant &operator [](const QString &name);
    inline const QVariant operator ()(const QString &name) {return fQuery->value(fColumnsNames[name]); }
    const QString lastQuery();
    QString lastDbError() const;
    QString fDatabaseNumber;

private:
    static int fDatabaseCounter;
    QMap<QString, QVariant> fBindValues;
    QHash<QString, int> fColumnsNames;
    static QMutex fMutex;
};

#endif // DATABASE_H

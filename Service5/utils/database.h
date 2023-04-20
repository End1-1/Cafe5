#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QMutex>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDateTime>
#include <QJsonObject>

class Database
{
public:
    explicit Database();
    explicit Database(Database &other);
    explicit Database(const QString &driverName);
    ~Database();
    QSqlDatabase fSqlDatabase;
    bool open(const QString &configFile);
    bool open(const QString &host, const QString &schema, const QString &username, const QString &password);
    bool open(const QJsonObject &o);
    bool startTransaction();
    bool commit();
    bool rollback();
    bool exec(const QString &query);
    bool execDirect(const QString &query);
    bool insert(const QString &table);
    bool insert(const QString &table, int &id);
    bool update(const QString &table);
    bool update(const QString &table, const QString &field, const QVariant &value);
    bool deleteFromTable(const QString &table, const QString &field, const QVariant &value);
    inline int rowCount() {return fQuery->size();}
    inline int columnCount() {return fColumnsNames.count();}
    inline QString columnName(int index) {return fColumnsIndexes[index];}
    inline bool next() {return fQuery->next();}
    inline QVariant value(int column) const {return fQuery->value(column);}
    inline QVariant value(const QString &columnName) {return fQuery->value(fColumnsNames[columnName]);}
    inline QString string(const QString &columnName) {return value(columnName).toString(); }
    inline QString string(int colIndex) {return value(colIndex).toString(); }
    inline int integer(const QString &columnName) {return value(columnName).toInt(); }
    inline int integer(int colIndex) {return value(colIndex).toInt(); }
    inline double doubleValue(const QString &columnName) {return value(columnName).toDouble(); }
    inline double doubleValue(int colIndex) {return value(colIndex).toDouble(); }
    inline QDate date(const QString &columnName) {return value(columnName).toDate(); }
    inline QTime time(const QString &columnName) {return value(columnName).toTime(); }
    inline QDateTime dateTimeValue(const QString &columnName) {return value(columnName).toDateTime(); }
    QString uuid();
    void setBindValues(const QMap<QString, QVariant> &v);
    QMap<QString, QVariant> getBindValues();
    void close();
    QSqlQuery *fQuery;
    QVariant &operator [](const QString &name);
    inline const QVariant operator ()(const QString &name) {return fQuery->value(fColumnsNames[name]); }
    const QString lastQuery();
    QString lastDbError() const;
    QString fDatabaseNumber;

    int genFBID(const QString &name);
    void rowToMap(QMap<QString, QVariant> &map);
    QStringList params();

private:
    static int fDatabaseCounter;
    QMap<QString, QVariant> fBindValues;
    QHash<QString, int> fColumnsNames;
    QHash<int, QString> fColumnsIndexes;
    QString fDatabaseDriver;
    static QMutex fMutex;
};

#endif // DATABASE_H

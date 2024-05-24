#ifndef DOUBLEDATABASE_H
#define DOUBLEDATABASE_H

#include <QSqlDatabase>
#include <QMap>
#include <QVariant>
#include <QDate>

class QSqlQuery;

extern QString __dd1Host;
extern QString __dd1Database;
extern QString __dd1Username;
extern QString __dd1Password;

class DoubleDatabase : public QObject
{
    Q_OBJECT
public:
    DoubleDatabase();
    DoubleDatabase(DoubleDatabase &db);
    DoubleDatabase(bool openFirst, bool openSecond = false);
    DoubleDatabase(const QString &host, const QString &db, const QString &user, const QString &password);
    ~DoubleDatabase();
    void init();
    bool isOpened();
    bool isReady();
    static QString getDbNumber(const QString &prefix);
    QVariant &operator[](const QString &name);
    void setDatabase(const QString &host, const QString &db, const QString &user, const QString &password);
    bool open();
    bool startTransaction();
    bool commit();
    void rollback();
    void close(bool commit = true);
    bool exec(const QString &sqlQuery);
    bool exec(const QString &sqlQuery, QList<QList<QVariant> > &dbrows);
    bool exec(const QString &sqlQuery, QList<QList<QVariant> > &dbrows, QMap<QString, int> &columns);
    bool exec(const QString &sqlQuery, QMap<QString, QList<QVariant> > &dbrows, QMap<QString, int> &columns);
    QMap<QString, QVariant> fBindValues;
    QList<QList<QVariant> > fDbRows;
    QString fLastError;
    int rowCount();
    int columnCount();
    bool nextRow(QList<QVariant> &row);
    bool nextRow();
    bool update(const QString &tableName, const QString &whereClause);
    int insert(const QString &tableName, bool returnId = true);
    bool insertId(const QString &tableName, const QVariant &id);
    QVariant singleResult(const QString &sql);
    bool deleteTableEntry(const QString &table, const QVariant &id);
    inline QVariant getValue(int column)
    {
        return fDbRows.at(fCursorPos).at(column);
    }
    inline QVariant getValue(const QString &columnName)
    {
        return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]);
    }
    inline QVariant getValue(int row, int column)
    {
        return fDbRows.at(row).at(column);
    }
    inline QVariant getValue(int row, const QString &columnName)
    {
        return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]);
    }
    inline QString getString (int column)
    {
        return fDbRows.at(fCursorPos).at(column).toString();
    }
    inline QString getString (const QString &columnName)
    {
        return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toString();
    }
    inline QString getString(int row, int column)
    {
        return fDbRows.at(row).at(column).toString();
    }
    inline QString getString(int row, const QString &columnName)
    {
        return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toString();
    }
    inline int getInt(int row, const QString &columnName)
    {
        return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toInt();
    }
    inline int getInt(int column)
    {
        return fDbRows.at(fCursorPos).at(column).toInt();
    }
    inline int getInt(const QString &columnName)
    {
        return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toInt();
    }
    inline uint getUInt(const QString &columnName)
    {
        return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toUInt();
    }
    inline double getDouble(int column)
    {
        return fDbRows.at(fCursorPos).at(column).toDouble();
    }
    inline double getDouble(const QString &columnName)
    {
        return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toDouble();
    }
    inline QDate getDate(int row, const QString &columnName)
    {
        return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toDate();
    }
    inline QDate getDate(const QString &columnName)
    {
        return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toDate();
    }
    inline QDate getDate(int column)
    {
        return fDbRows.at(fCursorPos).at(column).toDate();
    }
    inline QTime getTime(int column)
    {
        return fDbRows.at(fCursorPos).at(column).toTime();
    }
    inline QTime getTime(const QString &columnName)
    {
        return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toTime();
    }
    inline QDateTime getDateTime(int row, int column)
    {
        return fDbRows.at(row).at(column).toDateTime();
    }
    inline QDateTime getDateTime(int column)
    {
        return fDbRows.at(fCursorPos).at(column).toDateTime();
    }
    inline QDateTime getDateTime(const QString &columnName)
    {
        return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toDateTime();
    }
    inline QList<QVariant> &row()
    {
        return fDbRows[fCursorPos];
    }
    void getBindValues(QMap<QString, QVariant> &b);
    void getBindValues(int row, QMap<QString, QVariant> &b);
    void setValue(int row, int column, const QVariant &value);
    void setValue(int row, const QString &columnName, const QVariant &value);
    void resetDoNotUse(bool v = false);
    void setNoSqlErrorLogMode(bool v = true);
    static void logEvent(const QString &event);
private:
    bool fIsReady;
    bool fNoSqlErrorLog;
    static bool fDoNotUse2;
    int fCursorPos;
    static int fCounter;
    bool fRecordFails;
    QString fDbName1;
    QString fDbName2;
    QSqlDatabase fDb1;
    void configureDatabase(QSqlDatabase &cn, const QString &host, const QString &db, const QString &user,
                           const QString &password);
    QString lastQuery(QSqlQuery *q);
    bool exec(QSqlQuery *q, const QString &sqlQuery, bool &isSelect);
    QMap<QString, int> fNameColumnMap;
signals:
    void secondDbError();
};

#endif // DOUBLEDATABASE_H

#ifndef C5Database_H
#define C5Database_H

#include "format_date.h"
#include <QSqlDatabase>
#include <QMap>
#include <QVariant>
#include <QDate>
#include <QElapsedTimer>
#include <QJsonObject>
#include <QJsonArray>

#define where_id(id) QString("where f_id='%1'").arg(id)

class QSqlQuery;

class C5Database : public QObject
{
    Q_OBJECT
public:
    C5Database();

    C5Database(C5Database &db);

    C5Database(const QJsonObject &params);

    C5Database(const QString &host, const QString &db, const QString &user, const QString &password);

    ~C5Database();

    QStringList dbParams();

    bool isOpened();

    bool isReady();

    static QString getDbNumber(const QString &prefix);

    QVariant& operator[](const QString &name);

    void setDatabase(const QStringList &dbParam);

    void setDatabase(const QString &host, const QString &db, const QString &user, const QString &password);

    QString execDry(const QString &sqlQuery);

    bool exec(const QString &sqlQuery);

    bool exec(const QString &sqlQuery, std::vector<QJsonArray >& dbrows);

    bool exec(const QString &sqlQuery, std::vector<QJsonArray >& dbrows, QHash<QString, int>& columns);

    bool execSqlList(const QStringList &sqlList);

    bool execNetwork(const QString &sqlQuery);

    static QString uuid();

    QByteArray uuid_getbin(QString u);

    QMap<QString, QVariant> fBindValues;

    std::vector<QJsonArray> fDbRows;

    QMap<int, QMetaType::Type> fColumnType;

    QString fLastError;

    int rowCount();

    int columnCount();

    int pos();

    bool first();

    bool nextRow(QJsonArray &row);

    bool nextRow();

    bool update(const QString &tableName, const QString &whereClause);

    QString updateDry(const QString tableName, const QString &field, const QVariant &value);

    bool update(const QString &tableName, const QString &field, const QVariant &value);

    bool deleteFromTable(const QString &tableName, const QVariant &id);

    bool deleteFromTable(const QString &tableName, const QString &field, const QVariant &value);

    int insert(const QString &tableName, bool returnId = true);

    bool insert(const QString &tableName, int& id);

    bool replaceInto(const QString &tableName);

    bool insertId(const QString &tableName, const QVariant &id);

    QVariant singleResult(const QString &sql);

    bool deleteTableEntry(const QString &table, const QVariant &id);

    inline QVariant getValue(int column)
    {
        QJsonValue v = fDbRows.at(fCursorPos).at(column);

        switch(fColumnType[column]) {
        case QMetaType::Int:
            return v.toInt();

        case QMetaType::Double:
            return v.toDouble();

        default:
            return v.toVariant();
        }
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

    inline QString getString(int column)
    {
        return fDbRows.at(fCursorPos).at(column).toString();
    }

    inline QString getString(const QString &columnName)
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

    inline double getDouble(int row, const QString &columnName)
    {
        return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toDouble();
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
        return QDate::fromString(fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toString(), FORMAT_DATE_TO_STR_MYSQL);
    }

    inline QDate getDate(const QString &columnName)
    {
        return QDate::fromString(fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toString(),
                                 FORMAT_DATE_TO_STR_MYSQL);
    }

    inline QDate getDate(int column)
    {
        return QDate::fromString(fDbRows.at(fCursorPos).at(column).toString(), FORMAT_DATE_TO_STR_MYSQL);
    }

    inline QTime getTime(int column)
    {
        return QTime::fromString(fDbRows.at(fCursorPos).at(column).toString());
    }

    inline QTime getTime(const QString &columnName)
    {
        return QTime::fromString(fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toString());
    }

    inline QDateTime getDateTime(int row, const QString &columnName)
    {
        return QDateTime::fromString(fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toString(),
                                     FORMAT_DATETIME_TO_STR_MYSQL);
    }

    inline QDateTime getDateTime(const QString &columnName)
    {
        return QDateTime::fromString(fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toString(),
                                     FORMAT_DATETIME_TO_STR_MYSQL);
    }

    inline QDateTime getDateTime(int column)
    {
        return QDateTime::fromString(fDbRows.at(fCursorPos).at(column).toString(), FORMAT_DATETIME_TO_STR_MYSQL);
    }

    QString columnName(int index);

    QMap<QString, QVariant> getBindValues();

    void rowToMap(QMap<QString, QVariant>& m);

    void getBindValues(QMap<QString, QVariant>& b);

    void getBindValues(int row, QMap<QString, QVariant>& b);

    void setBindValues(const QMap<QString, QVariant>& b);

    void removeBindValue(const QString &key);

    void setValue(int row, int column, const QJsonValue &value);

    void setValue(int row, const QString &columnName, const QJsonValue &value);

    void logEvent(const QString &event);

    inline QString host()
    {
        return fDb.hostName();
    }

    inline QString database()
    {
        return fDb.databaseName();
    }

    inline QString username()
    {
        return fDb.userName();
    }

    inline QString password()
    {
        return fDb.password();
    }

    QSqlDatabase fDb;

    QHash<QString, int> fNameColumnMap;

    static bool LOGGING;

    QSqlQuery* fQuery;

    static QStringList fDbParams;

private:
    bool fIsReady;

    int fCursorPos;

    static int fCounter;

    QString fDbName;

    QElapsedTimer fTimer;

    qint64 fTimerCount;

    void configureDatabase(QSqlDatabase &cn, const QString &host, const QString &db, const QString &user,
                           const QString &password);

    //QString lastQuery(QSqlQuery *q);

    bool exec(const QString &sqlQuery, bool &isSelect);

signals:
    void queryError(const QString&);

};

#endif // C5Database_H

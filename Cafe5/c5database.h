#ifndef C5Database_H
#define C5Database_H

#include <QSqlDatabase>
#include <QMap>
#include <QVariant>
#include <QDate>

#define where_id(id) QString("where f_id=%1").arg(id)

class QSqlQuery;

class C5Database : public QObject
{
    Q_OBJECT
public:
    C5Database();
    C5Database(C5Database &db);
    C5Database(bool openFirst);
    C5Database(const QString &host, const QString &db, const QString &user, const QString &password);
    ~C5Database();
    static QString fHost;
    static QString fFile;
    static QString fUser;
    static QString fPass;
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
    inline QVariant getValue(int column) { return fDbRows.at(fCursorPos).at(column); }
    inline QVariant getValue(const QString &columnName) { return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]); }
    inline QVariant getValue(int row, int column) {return fDbRows.at(row).at(column); }
    inline QVariant getValue(int row, const QString &columnName) {return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]); }
    inline QString getString (int column) {return fDbRows.at(fCursorPos).at(column).toString(); }
    inline QString getString (const QString &columnName) { return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toString(); }
    inline QString getString(int row, int column) {return fDbRows.at(row).at(column).toString(); }
    inline QString getString(int row, const QString &columnName) {return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toString(); }
    inline int getInt(int row, const QString &columnName) {return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toInt(); }
    inline int getInt(int column) {return fDbRows.at(fCursorPos).at(column).toInt(); }
    inline int getInt(const QString &columnName) {return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toInt(); }
    inline double getDouble(int column) {return fDbRows.at(fCursorPos).at(column).toDouble(); }
    inline double getDouble(const QString &columnName) {return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toDouble(); }
    inline QDate getDate(int row, const QString &columnName) { return fDbRows.at(row).at(fNameColumnMap[columnName.toLower()]).toDate(); }
    inline QDate getDate(const QString &columnName) { return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toDate(); }
    inline QDate getDate(int column) { return fDbRows.at(fCursorPos).at(column).toDate(); }
    inline QTime getTime(int column) { return fDbRows.at(fCursorPos).at(column).toTime(); }
    inline QTime getTime(const QString &columnName) { return fDbRows.at(fCursorPos).at(fNameColumnMap[columnName.toLower()]).toTime(); }
    inline QList<QVariant> &row() {return fDbRows[fCursorPos]; }
    QString columnName(int index);
    void getBindValues(QMap<QString, QVariant> &b);
    void getBindValues(int row, QMap<QString, QVariant> &b);
    void setValue(int row, int column, const QVariant &value);
    void setValue(int row, const QString &columnName, const QVariant &value);
    void logEvent(const QString &event);
private:
    bool fIsReady;
    int fCursorPos;
    static int fCounter;
    QString fDbName;
    QSqlDatabase fDb;
    void configureDatabase(QSqlDatabase &cn, const QString &host, const QString &db, const QString &user, const QString &password);
    QString lastQuery(QSqlQuery *q);
    bool exec(QSqlQuery *q, const QString &sqlQuery, bool &isSelect);
    QMap<QString, int> fNameColumnMap;
signals:
    void secondDbError();
};

#endif // C5Database_H

#include "doubledatabase.h"
#include <QMutexLocker>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>
#include <QHostInfo>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QSqlDriver>
#include <QSqlField>

int DoubleDatabase::fCounter = 0;
bool DoubleDatabase::fDoNotUse2 = false;
bool logEnabled = true;

static QMutex fMutex;
QString __dd1Host;
QString __dd1Username;
QString __dd1Database;
QString __dd1Password;

DoubleDatabase::DoubleDatabase() :
    QObject()
{
    init();
    configureDatabase(fDb1, __dd1Host, __dd1Database, __dd1Username, __dd1Password);
}

DoubleDatabase::DoubleDatabase(DoubleDatabase &db) :
    QObject()
{
    init();
    configureDatabase(fDb1,
                      db.fDb1.hostName(),
                      db.fDb1.databaseName(),
                      db.fDb1.userName(),
                      db.fDb1.password());
}

DoubleDatabase::DoubleDatabase(bool openFirst, bool openSecond) :
    QObject()
{
    init();
    configureDatabase(fDb1, __dd1Host, __dd1Database, __dd1Username, __dd1Password);
    open();
}

DoubleDatabase::DoubleDatabase(const QString &host, const QString &db, const QString &user, const QString &password) :
    QObject()
{
    init();
    configureDatabase(fDb1, host, db, user, password);
}

DoubleDatabase::~DoubleDatabase()
{
    fDb1 = QSqlDatabase::addDatabase(_DBDRIVER_);
    QSqlDatabase::removeDatabase(fDbName1);
}

QString DoubleDatabase::getDbNumber(const QString &prefix)
{
    return QString("%1-%2").arg(prefix).arg(fCounter);
}

QVariant &DoubleDatabase::operator[](const QString &name)
{
    return fBindValues[name];
}

void DoubleDatabase::setDatabase(const QString &host, const QString &db, const QString &user, const QString &password)
{
    configureDatabase(fDb1, host, db, user, password);
}

bool DoubleDatabase::open()
{
    fLastError = "";
    bool isOpened = true;
    if (!fDb1.isOpen()) {
        if (fDb1.open()) {
            //fDb1.transaction();
        } else {
            isOpened = false;
            fLastError += "#1: " + fDb1.lastError().databaseText()
                          + " database: " + fDb1.databaseName()
                          + " username: " + fDb1.userName()
                          + " password: " + fDb1.password()
                          + " drivers: " + fDb1.drivers().join(',');
            logEvent(fLastError);
        }
    }
    return isOpened;
}

bool DoubleDatabase::startTransaction()
{
    //    qDebug() << "Opened" << fDb1.isOpen();
    //    qDebug() << "Futures" << fDb1.driver()->hasFeature(QSqlDriver::Transactions);
    //    qDebug() << "TRansaction" << fDb1.transaction();
    //    if (fExecFlagSlave) {
    //        fDb2.transaction();
    //    }
    QSqlQuery *q = new QSqlQuery(fDb1);
    bool result = q->exec("start transaction");
    delete q;
    return result;
}

bool DoubleDatabase::commit()
{
    //    fDb1.commit();
    //    if (fExecFlagSlave) {
    //        fDb2.commit();
    //    }
    //    return true;
    QSqlQuery *q1 = new QSqlQuery(fDb1);
    bool result = q1->exec("commit");
    delete q1;
    return result;
}

void DoubleDatabase::rollback()
{
    QSqlQuery *q1 = new QSqlQuery(fDb1);
    q1->exec("rollback");
    delete q1;
}

void DoubleDatabase::close(bool commit)
{
    if (commit) {
        fDb1.commit();
    } else {
        fDb1.rollback();
    }
    fDb1.close();
}

bool DoubleDatabase::exec(const QString &sqlQuery)
{
    return exec(sqlQuery, fDbRows, fNameColumnMap);
}

bool DoubleDatabase::exec(const QString &sqlQuery, QList<QList<QVariant> > &dbrows)
{
    QMap<QString, int> cols;
    return exec(sqlQuery, dbrows, cols);
}

bool DoubleDatabase::exec(const QString &sqlQuery, QList<QList<QVariant> > &dbrows, QMap<QString, int> &columns)
{
    QSqlQuery *q1 = new QSqlQuery(fDb1);
    bool isSelect = true;
    bool result = true;
    if (!exec(q1, sqlQuery, isSelect)) {
        delete q1;
        fBindValues.clear();
        return false;
    }
    if (logEnabled) {
        logEvent("#1 " + lastQuery(q1));
    }
    fBindValues.clear();
    if (isSelect) {
        fCursorPos = -1;
        columns.clear();
        QSqlRecord r = q1->record();
        for (int i = 0; i < r.count(); i++) {
            columns[r.field(i).name().toLower()] = i;
        }
        int colCount = r.count();
        dbrows.clear();
        while (q1->next()) {
            QList<QVariant> row;
            for (int i = 0; i < colCount; i++) {
                row << q1->value(i);
            }
            dbrows << row;
        }
    }
    delete q1;
    return result;
}

bool DoubleDatabase::exec(const QString &sqlQuery, QMap<QString, QList<QVariant> > &dbrows, QMap<QString, int> &columns)
{
    QSqlQuery *q1 = new QSqlQuery(fDb1);
    bool isSelect = true;
    bool result = true;
    if (!exec(q1, sqlQuery, isSelect)) {
        delete q1;
        fBindValues.clear();
        return false;
    }
#ifdef QT_DEBUG
    logEvent("#1 " + lastQuery(q1));
#endif
    fBindValues.clear();
    if (isSelect) {
        fCursorPos = -1;
        columns.clear();
        QSqlRecord r = q1->record();
        for (int i = 0; i < r.count(); i++) {
            columns[r.field(i).name().toLower()] = i;
        }
        int colCount = r.count();
        dbrows.clear();
        while (q1->next()) {
            QList<QVariant> row;
            for (int i = 0; i < colCount; i++) {
                row << q1->value(i);
            }
            dbrows[q1->value(0).toString()] << row;
        }
    }
    return result;
}

int DoubleDatabase::rowCount()
{
    return fDbRows.count();
}

int DoubleDatabase::columnCount()
{
    return fNameColumnMap.count();
}

bool DoubleDatabase::nextRow(QList<QVariant> &row)
{
    bool result = nextRow();
    if (result) {
        row = fDbRows.at(fCursorPos);
    }
    return result;
}

bool DoubleDatabase::nextRow()
{
    if (fCursorPos < rowCount() - 1 && rowCount() > 0) {
        fCursorPos++;
        return true;
    }
    return false;
}

bool DoubleDatabase::update(const QString &tableName, const QString &whereClause)
{
    QString sql = "update " + tableName + " set ";
    bool first = true;
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.begin(); it != fBindValues.end(); it++) {
        if (first) {
            first = false;
        } else {
            sql += ",";
        }
        QString f = it.key();
        sql += f.remove(0, 1) + "=" + it.key();
    }
    if (!whereClause.isEmpty()) {
        sql += " " + whereClause;
    }
    return exec(sql);
}

int DoubleDatabase::insert(const QString &tableName, bool returnId)
{
    QString sql = "insert into " + tableName;
    QString k, v;
    bool first = true;
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.begin(); it != fBindValues.end(); it++) {
        if (first) {
            first = false;
        } else {
            k += ",";
            v += ",";
        }
        k += QString(it.key()).remove(0, 1);
        v += it.key();
    }
    sql += QString("(%1) values (%2)").arg(k).arg(v);
    if (!exec(sql)) {
        return 0;
    }
    if (returnId) {
        exec("select last_insert_id()");
        return getValue(0, 0).toInt();
    } else {
        return 1;
    }
}

bool DoubleDatabase::insertId(const QString &tableName, const QVariant &id)
{
    fBindValues[":f_id"] = id;
    return insert(tableName, false) != 0;
}

QVariant DoubleDatabase::singleResult(const QString &sql)
{
    exec(sql);
    if (nextRow()) {
        return getValue(0);
    }
    return QVariant();
}

bool DoubleDatabase::deleteTableEntry(const QString &table, const QVariant &id)
{
    QString query = "delete from " + table + " where f_id=:f_id";
    fBindValues[":f_id"] = id;
    return exec(query);
}

void DoubleDatabase::getBindValues(QMap<QString, QVariant> &b)
{
    getBindValues(fCursorPos, b);
}

void DoubleDatabase::getBindValues(int row, QMap<QString, QVariant> &b)
{
    b.clear();
    for (QMap<QString, int>::const_iterator it = fNameColumnMap.begin(); it != fNameColumnMap.end(); it++) {
        b[":" + it.key()] = getValue(row, it.key());
    }
}

void DoubleDatabase::setValue(int row, int column, const QVariant &value)
{
    fDbRows[row][column] = value;
}

void DoubleDatabase::setValue(int row, const QString &columnName, const QVariant &value)
{
    setValue(row, fNameColumnMap[columnName], value);
}

void DoubleDatabase::resetDoNotUse(bool v)
{
    fDoNotUse2 = v;
    fRecordFails = false;
}

void DoubleDatabase::setNoSqlErrorLogMode(bool v)
{
    fNoSqlErrorLog = v;
}

void DoubleDatabase::init()
{
    fNoSqlErrorLog = false;
    fRecordFails = false;
    fIsReady = false;
    if (QSqlDatabase::drivers().count() == 0) {
        return;
    }
    fIsReady = true;
    QMutexLocker ml( &fMutex);
    ++fCounter;
    fDbName1 = getDbNumber("DB1");
    fDbName2 = getDbNumber("DB2");
    fDb1 = QSqlDatabase::addDatabase(_DBDRIVER_, fDbName1);
}

bool DoubleDatabase::isOpened()
{
    return fDb1.isOpen();
}

bool DoubleDatabase::isReady()
{
    return fIsReady;
}

void DoubleDatabase::configureDatabase(QSqlDatabase &cn, const QString &host, const QString &db, const QString &user,
                                       const QString &password)
{
    cn.setHostName(host);
    cn.setDatabaseName(db);
    cn.setUserName(user);
    cn.setPassword(password);
}

void DoubleDatabase::logEvent(const QString &event)
{
#ifdef QT_DEBUG
    qDebug() << event;
#endif
    QDir d;
    if (!d.exists(d.homePath() + "/" + _APPLICATION_)) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_);
    }
    QFile  f(d.homePath() + "/" + _APPLICATION_ + "/log.txt");
    QString dt = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + " ";
    if (f.open(QIODevice::Append)) {
        f.write(dt.toUtf8());
        f.write(event.toUtf8());
        f.write("\r\n");
        f.close();
    }
}

QString DoubleDatabase::lastQuery(QSqlQuery *q)
{
    QString sql = q->lastQuery();
    QMapIterator<QString, QVariant> it(q->boundValues());
    while (it.hasNext()) {
        it.next();
        QVariant value = it.value();
        switch (it.value().type()) {
            case QVariant::String:
                value = QString("'%1'").arg(value.toString().replace("'", "''"));
                break;
            case QVariant::Date:
                if (value.toDate().isValid()) {
                    value = QString("'%1'").arg(value.toDate().toString("yyyy-MM-dd"));
                } else {
                    value = "null";
                }
                break;
            case QVariant::DateTime:
                if (value.toDateTime().isValid()) {
                    value = QString("'%1'").arg(value.toDateTime().toString("yyyy-MM-dd HH:mm:ss"));
                } else {
                    value = "null";
                }
                break;
            case QVariant::Double:
                value = QString("%1").arg(value.toDouble());
                break;
            case QVariant::Int:
                value = QString("%1").arg(value.toInt());
                break;
            case QVariant::Time:
                if (value.toTime().isValid()) {
                    value = QString("'%1'").arg(value.toTime().toString("HH:mm:ss"));
                } else {
                    value = "null";
                }
                break;
            default:
                break;
        }
        sql.replace(QRegExp(it.key() + "\\b"), value.toString());
    }
    return sql;
}

bool DoubleDatabase::exec(QSqlQuery *q, const QString &sqlQuery, bool &isSelect)
{
    if (!q->prepare(sqlQuery)) {
        fLastError = q->lastError().databaseText();
        if (!fNoSqlErrorLog) {
            logEvent(fLastError);
            logEvent(sqlQuery);
        }
        return false;
    }
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.begin(); it != fBindValues.end(); it++) {
        q->bindValue(it.key(), it.value());
    }
    if (!q->exec()) {
        fLastError = q->lastError().databaseText();
        if (!fNoSqlErrorLog) {
            logEvent(fLastError);
            logEvent(lastQuery(q));
        }
        return false;
    }
    isSelect = q->isSelect();
    if (!isSelect) {
        isSelect = sqlQuery.mid(0, 4).compare("call", Qt::CaseInsensitive) == 0;
    }
    if (!isSelect) {
        isSelect = sqlQuery.mid(0, 4).compare("drop", Qt::CaseInsensitive) == 0;
    }
    return true;
}

#include "C5Database.h"
#include <QMutexLocker>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>
#include <QHostInfo>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QSqlField>

int C5Database::fCounter = 0;

QMutex fMutex;
QString C5Database::fHost;
QString C5Database::fFile;
QString C5Database::fUser;
QString C5Database::fPass;

C5Database::C5Database() :
    QObject()
{
    init();
    configureDatabase(fDb, fHost, fFile, fUser, fPass);
}

C5Database::C5Database(C5Database &db) :
    QObject()
{
    init();
    configureDatabase(fDb,
                      db.fDb.hostName(),
                      db.fDb.databaseName(),
                      db.fDb.userName(),
                      db.fDb.password());
}

C5Database::C5Database(bool openFirst) :
    QObject()
{
    init();
    Q_UNUSED(openFirst)
    configureDatabase(fDb, fHost, fFile, fUser, fPass);
    fDb.open();
}

C5Database::C5Database(const QString &host, const QString &db, const QString &user, const QString &password) :
    QObject()
{
    init();
    configureDatabase(fDb, host, db, user, password);

}

C5Database::~C5Database()
{
    logEvent("Destructor" + fDb.connectionName());
    fDb = QSqlDatabase::addDatabase(_DBDRIVER_);
    QSqlDatabase::removeDatabase(fDbName);
}

QString C5Database::getDbNumber(const QString &prefix)
{
    return QString("%1-%2").arg(prefix).arg(fCounter);
}

QVariant &C5Database::operator[](const QString &name)
{
    return fBindValues[name];
}

void C5Database::setDatabase(const QString &host, const QString &db, const QString &user, const QString &password)
{
    configureDatabase(fDb, host, db, user, password);
}

bool C5Database::open()
{
    fLastError = "";
    bool isOpened = true;
    if (!fDb.isOpen()) {
        if (fDb.open()) {
            //fDb.transaction();
        } else {
            isOpened = false;
            fLastError += fDb.lastError().databaseText() + " database: " + fDb.databaseName() + " drivers: " + fDb.drivers().join(',');
            logEvent(fLastError);
        }
    }
    return isOpened;
}

bool C5Database::startTransaction()
{
    fDb.transaction();
    return true;
}

bool C5Database::commit()
{
    fDb.commit();
    return true;
}

void C5Database::rollback()
{
    fDb.rollback();
}

void C5Database::close(bool commit)
{
    if (commit) {
        fDb.commit();
    } else {
        fDb.rollback();
    }
    fDb.close();
}

bool C5Database::exec(const QString &sqlQuery)
{
    if (!fDb.open()) {
        if (!fDb.open()) {
            C5Message::error(fLastError);
            return false;
        }
    }
    return exec(sqlQuery, fDbRows, fNameColumnMap);
}

bool C5Database::exec(const QString &sqlQuery, QList<QList<QVariant> > &dbrows)
{
    QMap<QString, int> cols;
    return exec(sqlQuery, dbrows, cols);
}

bool C5Database::exec(const QString &sqlQuery, QList<QList<QVariant> > &dbrows, QMap<QString, int> &columns)
{
    QSqlQuery *q1 = new QSqlQuery(fDb);
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
            dbrows << row;
        }
    }
    return result;
}

bool C5Database::exec(const QString &sqlQuery, QMap<QString, QList<QVariant> > &dbrows, QMap<QString, int> &columns)
{
    QSqlQuery *q1 = new QSqlQuery(fDb);
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

int C5Database::rowCount()
{
    return fDbRows.count();
}

int C5Database::columnCount()
{
    return fNameColumnMap.count();
}

bool C5Database::nextRow(QList<QVariant> &row)
{
    bool result = nextRow();
    if (result) {
        row = fDbRows.at(fCursorPos);
    }
    return result;
}

bool C5Database::nextRow()
{
    if (fCursorPos < rowCount() - 1 && rowCount() > 0) {
        fCursorPos++;
        return true;
    }
    return false;
}

bool C5Database::update(const QString &tableName, const QString &whereClause)
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

int C5Database::insert(const QString &tableName, bool returnId)
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
        QSqlQuery q(fDb);
        q.exec("select last_insert_id()");
        q.next();
        return q.value(0).toInt();
    } else {
        return 1;
    }
}

bool C5Database::insertId(const QString &tableName, const QVariant &id)
{
    fBindValues[":f_id"] = id;
    return insert(tableName, false) != 0;
}

QVariant C5Database::singleResult(const QString &sql)
{
    exec(sql);
    if (nextRow()) {
        return getValue(0);
    }
    return QVariant();
}

bool C5Database::deleteTableEntry(const QString &table, const QVariant &id)
{
    QString query = "delete from " + table + " where f_id=:f_id";
    fBindValues[":f_id"] = id;
    return exec(query);
}

QString C5Database::columnName(int index)
{
    return fNameColumnMap.key(index);
}

void C5Database::getBindValues(QMap<QString, QVariant> &b)
{
    getBindValues(fCursorPos, b);
}

void C5Database::getBindValues(int row, QMap<QString, QVariant> &b)
{
    for (QMap<QString, int>::const_iterator it = fNameColumnMap.begin(); it != fNameColumnMap.end(); it++) {
        b[":" + it.key()] = getValue(row, it.key());
    }
}

void C5Database::setValue(int row, int column, const QVariant &value)
{
    fDbRows[row][column] = value;
}

void C5Database::setValue(int row, const QString &columnName, const QVariant &value)
{
    setValue(row, fNameColumnMap[columnName], value);
}

void C5Database::init()
{
    fIsReady = false;
    if (QSqlDatabase::drivers().count() == 0) {
        return;
    }
    fIsReady = true;

    QMutexLocker ml(&fMutex);
    ++fCounter;
    fDbName = getDbNumber("DB1");
    fDb = QSqlDatabase::addDatabase(_DBDRIVER_, fDbName);
}

bool C5Database::isOpened()
{
    return fDb.isOpen();
}

bool C5Database::isReady()
{
    return fIsReady;
}

void C5Database::configureDatabase(QSqlDatabase &cn, const QString &host, const QString &db, const QString &user, const QString &password)
{
    cn.setHostName(host);
    cn.setDatabaseName(db);
    cn.setUserName(user);
    cn.setPassword(password);
}

void C5Database::logEvent(const QString &event)
{
    qDebug() << event;
    QDir d;
    if (!d.exists(d.homePath() + "/" + _APPLICATION_)) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_);
    }
    QFile  f(d.homePath() + "/" + _APPLICATION_ + "/log.txt");
    QString dt = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + " ";
    f.open(QIODevice::Append);
    f.write(dt.toUtf8());
    f.write(event.toUtf8());
    f.write("\r\n");
    f.close();
}

QString C5Database::lastQuery(QSqlQuery *q)
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
            value = QString("'%1'").arg(value.toDate().toString("yyyy-MM-dd"));
            break;
        case QVariant::DateTime:
            value = QString("'%1'").arg(value.toDateTime().toString("yyyy-MM-dd HH:mm:ss"));
            break;
        case QVariant::Double:
            value = QString("%1").arg(value.toDouble());
            break;
        case QVariant::Int:
            value = QString("%1").arg(value.toInt());
            break;
        case QVariant::Time:
            value = QString("'%1'").arg(value.toTime().toString("HH:mm:ss"));
            break;
        default:
            break;
        }
        sql.replace(QRegExp(it.key() + "\\b"), value.toString());
    }
    return sql;
}

bool C5Database::exec(QSqlQuery *q, const QString &sqlQuery, bool &isSelect)
{
    if (!fDb.isOpen()) {
        if (!fDb.open()) {
            C5Message::error(fLastError);
            return false;
        }
    }
    if (!q->prepare(sqlQuery)) {
        fLastError = q->lastError().databaseText();
        logEvent(fLastError);
        logEvent(sqlQuery);
        return false;
    }
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.begin(); it != fBindValues.end(); it++) {
        q->bindValue(it.key(), it.value());
    }
    if (!q->exec()) {
        fLastError = q->lastError().databaseText();
        logEvent(fLastError);
        logEvent(lastQuery(q));
        return false;
    }
    isSelect = q->isSelect();
    if (!isSelect) {
        isSelect = sqlQuery.mid(0, 4).compare("call", Qt::CaseInsensitive) == 0;
    }
    return true;
}

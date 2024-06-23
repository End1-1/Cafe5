#include "C5Database.h"
#include "c5config.h"
#include "c5utils.h"
#include <QMutexLocker>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>
#include <QHostInfo>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QUuid>
#include <QSqlField>
#include <QException>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include <QApplication>

#ifndef _NOAPP_
    #include <QMessageBox>
#endif

int C5Database::fCounter = 0;
bool C5Database::LOGGING = false;
QStringList C5Database::fDbParamsForUuid;

static QMutex fMutex;

class SqlException : public QException
{
public:
    SqlException(const QString &w) throw ()
    {
        fWhat = w;
    }
    virtual const char *what() const noexcept override
    {
        return fWhat.toUtf8().data();
    }
private:
    QString fWhat;
};

C5Database::C5Database() :
    QObject()
{
    fQuery = nullptr;
    init();
}

C5Database::C5Database(const QString &dbdriver)
{
    fIsReady = false;
    if (QSqlDatabase::drivers().count() == 0) {
        return;
    }
    fIsReady = true;
    fQuery = nullptr;
    QMutexLocker ml( &fMutex);
    ++fCounter;
    fDbName = getDbNumber("DB1");
    fDb = QSqlDatabase::addDatabase(dbdriver, fDbName);
}

C5Database::C5Database(const QStringList &dbParams) :
    C5Database()
{
    fDbParamsForUuid = dbParams;
    init();
    configureDatabase(fDb, dbParams[0], dbParams[1], dbParams[2], dbParams[3]);
}

C5Database::C5Database(C5Database &db) :
    C5Database()
{
    init();
    configureDatabase(fDb,
                      db.fDb.hostName(),
                      db.fDb.databaseName(),
                      db.fDb.userName(),
                      db.fDb.password());
}

C5Database::C5Database(const QJsonObject &params) :
    C5Database()
{
    fDbParamsForUuid.clear();
    fDbParamsForUuid.append(params["host"].toString());
    fDbParamsForUuid.append(params["database"].toString());
    configureDatabase(fDb, params["host"].toString(), params["database"].toString(), params["username"].toString(),
                      params["password"].toString());
}

C5Database::C5Database(const QString &host, const QString &db, const QString &user, const QString &password) :
    C5Database()
{
    fDbParamsForUuid.clear();
    fDbParamsForUuid.append(host);
    fDbParamsForUuid.append(db);
    fDbParamsForUuid.append(user);
    fDbParamsForUuid.append(password);
    init();
    configureDatabase(fDb, host, db, user, password);
}

C5Database::~C5Database()
{
#ifdef NETWORKDB
    return;
#endif
    if (fQuery) {
        fQuery->finish();
        delete fQuery;
    }
    fDb.close();
    fDb = QSqlDatabase::addDatabase(_DBDRIVER_);
    QSqlDatabase::removeDatabase(fDbName);
}

QStringList C5Database::dbParams()
{
    QStringList params;
    params << fDb.hostName()
           << fDb.databaseName()
           << fDb.userName()
           << fDb.password();
    return params;
}

QString C5Database::getDbNumber(const QString &prefix)
{
    return QString("%1-%2").arg(prefix).arg(fCounter);
}

QVariant &C5Database::operator[](const QString &name)
{
    return fBindValues[name];
}

void C5Database::setDatabase(const QStringList &dbParam)
{
    setDatabase(dbParam.at(0), dbParam.at(1), dbParam.at(2), dbParam.at(3));
}

void C5Database::setDatabase(const QString &host, const QString &db, const QString &user, const QString &password)
{
    configureDatabase(fDb, host, db, user, password);
}

bool C5Database::open()
{
#ifdef NETWORKDB
    return true;
#endif
    fLastError = "";
    bool isOpened = true;
    if (!fDb.isOpen()) {
        if (fDb.open()) {
            if (fQuery) {
                delete fQuery;
            }
            fQuery = new QSqlQuery(fDb);
        } else {
            isOpened = false;
            fLastError += fDb.lastError().databaseText() + " database: " + fDb.databaseName() + " drivers: " +
                          fDb.drivers().join(',');
            logEvent(fLastError);
        }
    }
    return isOpened;
}

bool C5Database::startTransaction()
{
#ifdef NETWORKDB
    return true;
#endif
    if (!open()) {
        return false;
    }
    return fQuery->exec("start transaction");
}

bool C5Database::commit()
{
#ifdef NETWORKDB
    return true;
#endif
    return fQuery->exec("commit");
}

void C5Database::rollback()
{
#ifdef NETWORKDB
    return;
#endif
    fQuery->exec("rollback");
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
    if (!open()) {
#ifdef _NOAPP_
#else
        QMessageBox::critical(0, "DB error", fLastError);
#endif
        return false;
    }
    return exec(sqlQuery, fDbRows, fNameColumnMap);
}

bool C5Database::exec(const QString &sqlQuery, QList<QList<QVariant> > &dbrows)
{
    QHash<QString, int> cols;
    return exec(sqlQuery, dbrows, cols);
}

bool C5Database::exec(const QString &sqlQuery, QList<QList<QVariant> > &dbrows, QHash<QString, int> &columns)
{
    bool isSelect = true;
    bool result = true;
#ifdef NETWORKDB
    if(execNetwork(sqlQuery)) {
        dbrows = fDbRows;
        columns = fNameColumnMap;
        return true;
    } else {
#ifdef QT_DEBUG
        logEvent(fDb.hostName() + ":" + fDb.databaseName() + " " + fLastError + sqlQuery);
#else
        logEvent(fLastError + sqlQuery);
#endif
        return false;
    }
#endif
    if (!exec(sqlQuery, isSelect)) {
        fBindValues.clear();
        return false;
    }
    fBindValues.clear();
    if (isSelect) {
        fCursorPos = -1;
        columns.clear();
        QSqlRecord r = fQuery->record();
        for (int i = 0; i < r.count(); i++) {
            columns[r.field(i).name().toLower()] = i;
        }
        int colCount = r.count();
        dbrows.clear();
        while (fQuery->next()) {
            QList<QVariant> row;
            for (int i = 0; i < colCount; i++) {
                row << fQuery->value(i);
            }
            dbrows << row;
        }
    }
    return result;
}

bool C5Database::exec(const QString &sqlQuery, QMap<QString, QList<QVariant> > &dbrows, QMap<QString, int> &columns)
{
    bool isSelect = true;
    bool result = true;
#ifdef NETWORKDB
    return execNetwork(sqlQuery);
#endif
    if (!exec(sqlQuery, isSelect)) {
        fBindValues.clear();
        return false;
    }
    if (LOGGING) {
#ifdef QT_DEBUG
        logEvent(fDb.hostName() + ":" + fDb.databaseName() + " " + lastQuery(fQuery));
#else
        logEvent(lastQuery(fQuery));
#endif
    }
    fBindValues.clear();
    if (isSelect) {
        fCursorPos = -1;
        columns.clear();
        QSqlRecord r = fQuery->record();
        for (int i = 0; i < r.count(); i++) {
            columns[r.field(i).name().toLower()] = i;
        }
        int colCount = r.count();
        dbrows.clear();
        while (fQuery->next()) {
            QList<QVariant> row;
            for (int i = 0; i < colCount; i++) {
                row << fQuery->value(i);
            }
            dbrows[fQuery->value(0).toString()] << row;
        }
    }
    return result;
}

bool C5Database::execDirect(const QString &sqlQuery)
{
#ifdef NETWORKDB
    return execNetwork(sqlQuery);
#endif
    if (!open())  {
        return false;
    }
    if (!fQuery->exec(sqlQuery)) {
        fLastError = fQuery->lastError().databaseText();
        logEvent(fLastError);
        logEvent(sqlQuery);
        return false;
    }
#ifdef QT_DEBUG
    logEvent(fDb.hostName() + ":" + fDb.databaseName() + " " + lastQuery(fQuery));
#else
    if (LOGGING) {
        logEvent(fDb.hostName() + ":" + fDb.databaseName() + " " + lastQuery(fQuery));
    }
#endif
    return true;
}

bool C5Database::execNetwork(const QString &sqlQuery)
{
    if (fDbParamsForUuid.isEmpty()) {
        fLastError = "Database not configured";
        return false;
    }
    QElapsedTimer t;
    t.start();
    QString sql = sqlQuery;
    QMapIterator<QString, QVariant> it(fBindValues);
    while (it.hasNext()) {
        it.next();
        QVariant value = it.value();
        if(!it.value().isValid()) {
            value = "null";
        } else {
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
                    value = QString("%1").arg(QString::number(value.toDouble(), 'f', 4));
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
                case QVariant::ByteArray:
                    value = QString("'%1'").arg(QString(value.toByteArray().toHex()));
                    break;
                default:
                    break;
            }
        }
        sql.replace(QRegExp(it.key() + "\\b"), value.toString());
    }
    fBindValues.clear();
    QNetworkAccessManager m;
    QString host = "https://" + fDbParamsForUuid.at(0);
    QNetworkRequest rq(host);
    m.setTransferTimeout(60000);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration sslConf = rq.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    rq.setSslConfiguration(sslConf);
    QJsonObject jo;
    jo["query"] = 3;
    jo["call"] = "sql";
    jo["sql"] = sql;
    jo["sk"] = "5cfafe13-a886-11ee-ac3e-1078d2d2b808";
    auto *r = m.post(rq, QJsonDocument(jo).toJson());
    while (!r->isFinished()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        QThread::msleep(10);
    }
    if (r->error() != QNetworkReply::NoError) {
        fLastError = r->errorString();
        return false;
    }
    QByteArray ba = r->readAll();
    quint64 elapsed = t.elapsed();
    jo = QJsonDocument::fromJson(ba).object();
    if (jo["status"].toInt() == 0) {
        fLastError = ba;
        logEvent(ba);
        return false;
    }
    if (sql.mid(0, 6).compare("insert", Qt::CaseInsensitive) == 0
            || sql.mid(0, 6).compare("delete", Qt::CaseInsensitive) == 0
            || sql.mid(0, 6).compare("update", Qt::CaseInsensitive) == 0) {
        if (sql.mid(0, 6).compare("insert", Qt::CaseInsensitive) == 0) {
            fCursorPos = jo["data"].toString().toInt();
        }
#ifdef QT_DEBUG
        logEvent(fDbParamsForUuid.at(0) + " (" + QString::number(elapsed) + "-" + QString::number(
                     t.elapsed()) + " ms):" + " " + sql);
#else
        if (__c5config.getValue(param_debuge_mode).toInt() > 0) {
            logEvent("(" + QString::number(elapsed) + "-" + QString::number(t.elapsed()) + " ms):" + " " + sql);
        }
#endif
        return true;
    }
    jo = jo["data"].toObject();
    QJsonArray ja = jo["columns"].toObject()["column_index_name"].toArray();
    fNameColumnMap.clear();
    for (int i = 0; i < ja.size(); i++) {
        const QJsonObject &jc = ja[i].toObject();
        fNameColumnMap[jc["name"].toString()] = jc["value"].toInt();
    }
    ja = jo["columns"].toObject()["column_name_index"].toArray();
    QJsonArray jtype = jo["types"].toArray();
    // fNameColumnMap.clear();
    // for (int i = 0; i < ja.size(); i++) {
    //     const QJsonObject &jc = ja[i].toObject();
    //     fNameColumnMap[jc["name"].toString()] = jc["value"].toInt();
    // }
    ja = jo["data"].toArray();
    fDbRows.clear();
    for (int i = 0; i < ja.size(); i++) {
        QList<QVariant> r;
        QJsonArray jar = ja[i].toArray();
        for (int j = 0; j < jar.size(); j++) {
            switch (jtype[j].toInt()) {
                case 3:
                    r.append(jar[j].toInt());
                    break;
                case 246:
                    r.append(jar[j].toDouble());
                    break;
                case 10:
                    r.append(QDate::fromString(jar[j].toString(), FORMAT_DATE_TO_STR_MYSQL));
                    break;
                case 11:
                    r.append(QTime::fromString(jar[j].toString()));
                    break;
                case 7:
                    r.append(QDateTime::fromString(jar[j].toString()));
                    break;
                default:
                    r.append(jar[j].toString());
                    break;
            }
        }
        fDbRows.append(r);
    }
    fCursorPos = -1;
#ifdef QT_DEBUG
    logEvent(fDbParamsForUuid.at(0) + " (" + QString::number(elapsed) + "-" + QString::number(
                 t.elapsed()) + " ms):" + " " + sql);
#else
    if (__c5config.getValue(param_debuge_mode).toInt() > 0) {
        logEvent("(" + QString::number(elapsed) + "-" + QString::number(t.elapsed()) + " ms):" + " " + sql);
    }
#endif
    return true;
}

QString C5Database::uuid()
{
    return QUuid::createUuid().toString().replace("{", "").replace("}", "");
}

QByteArray C5Database::uuid_bin()
{
    exec("select unhex(replace(uuid(),'-',''))") ;
    if (nextRow()) {
        return getValue(0).toByteArray();
    }
    return QByteArray();
}

QByteArray C5Database::uuid_getbin(QString u)
{
    QByteArray b;
    u.replace("-", "");
    bool ok = true;
    for (int i = 0; i < u.length(); i += 2) {
        QString t = u.mid(i, 2);
        b.append(t.toUInt( &ok, 16));
    }
    return b;
}

int C5Database::rowCount()
{
    return fDbRows.count();
}

int C5Database::columnCount()
{
    return fNameColumnMap.count();
}

int C5Database::pos()
{
    return fCursorPos;
}

bool C5Database::first()
{
    fCursorPos = -1;
    return rowCount() > 0;
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
    if ((fCursorPos < rowCount() - 1) && (rowCount() > 0)) {
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

bool C5Database::update(const QString &tableName, const QString &field, const QVariant &value)
{
    QString sql = "update " + tableName + " set ";
    bool first = true;
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.constBegin(); it != fBindValues.constEnd(); it++) {
        if (first) {
            first = false;
        } else {
            sql += ",";
        }
        QString f = it.key();
        sql += f.remove(0, 1) + "=" + it.key();
    }
    sql += QString(" where %1=:%1").arg(field);
    fBindValues[":" + field] = value;
    return exec(sql);
}

bool C5Database::deleteFromTable(const QString &tableName, const QString &field, const QVariant &value)
{
    fBindValues[":" + field] = value;
    QString sql = QString("delete from %1 where %2=:%2").arg(tableName).arg(field);
    return exec(sql);
}

bool C5Database::deleteFromTable(const QString &tableName, const QVariant &id)
{
    return deleteFromTable(tableName, "f_id", id);
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
    sql += QString("(%1) values (%2)").arg(k, v);
    if (!exec(sql)) {
        return 0;
    }
    if (returnId) {
#ifdef NETWORKDB
        return fCursorPos;
#endif
        fQuery->exec("select last_insert_id()");
        fQuery->next();
        return fQuery->value(0).toInt();
    } else {
        return 1;
    }
}

bool C5Database::insert(const QString &tableName, int &id)
{
    id = insert(tableName, true);
    return id > 0;
}

bool C5Database::replaceInto(const QString &tableName)
{
    QString sql = "replace into " + tableName + " values ( ";
    bool first = true;
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.begin(); it != fBindValues.end(); it++) {
        if (first) {
            first = false;
        } else {
            sql += ",";
        }
        sql += it.key();
    }
    sql += ")";
    return exec(sql);
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

QMap<QString, QVariant> C5Database::getBindValues()
{
    QMap<QString, QVariant> b;
    for (QHash<QString, int>::const_iterator it = fNameColumnMap.constBegin(); it != fNameColumnMap.constEnd(); it++) {
        b[":" + it.key()] = getValue(fCursorPos, it.key());
    }
    return b;
}

void C5Database::rowToMap(QMap<QString, QVariant> &m)
{
    for (int i = 0; i < columnCount(); i++) {
        m[columnName(i)] = getValue(i);
    }
}

void C5Database::getBindValues(QMap<QString, QVariant> &b)
{
    getBindValues(fCursorPos, b);
}

void C5Database::getBindValues(int row, QMap<QString, QVariant> &b)
{
    for (QHash<QString, int>::const_iterator it = fNameColumnMap.begin(); it != fNameColumnMap.end(); it++) {
        b[":" + it.key()] = getValue(row, it.key());
    }
}

void C5Database::setBindValues(const QMap<QString, QVariant> &b)
{
    fBindValues = b;
}

void C5Database::removeBindValue(const QString &key)
{
    fBindValues.remove(key);
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
#ifdef NETWORKDB
    return;
#endif
    fIsReady = false;
    if (QSqlDatabase::drivers().count() == 0) {
        return;
    }
    fIsReady = true;
    QMutexLocker ml( &fMutex);
    ++fCounter;
    fDbName = getDbNumber("DB1");
    fDb = QSqlDatabase::addDatabase(_DBDRIVER_, fDbName);
    fDb.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=5");
}

bool C5Database::isOpened()
{
    return fDb.isOpen();
}

bool C5Database::isReady()
{
    return fIsReady;
}

void C5Database::configureDatabase(QSqlDatabase &cn, const QString &host, const QString &db, const QString &user,
                                   const QString &password)
{
    QString h = host;
    int port = 0;
    if (cn.driverName() == "QIBASE") {
        port = 3050;
    } else if (cn.driverName() == "QMARIADB") {
        port = 3306;
    }
    if (h.contains(":")) {
        QString portstr = h.mid(h.indexOf(":") + 1, h.length() - h.indexOf(":"));
        port = portstr.toInt();
        h = h.mid(0, h.indexOf(":"));
    }
    if (port > 0) {
        cn.setPort(port);
    }
    cn.setHostName(h);
    cn.setDatabaseName(db);
    cn.setUserName(user);
    cn.setPassword(password);
}

void C5Database::logEvent(const QString &event)
{
    qDebug() << event.left(1000);
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
        if(!it.value().isValid()) {
            value = "null";
        } else {
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
                case QVariant::ByteArray:
                    value = QString("'%1'").arg(QString(value.toByteArray().toHex()));
                    break;
                default:
                    break;
            }
        }
        sql.replace(QRegExp(it.key() + "\\b"), value.toString());
    }
    return sql;
}

bool C5Database::exec(const QString &sqlQuery, bool &isSelect)
{
    QString sql = sqlQuery;
    sql = sql.replace("\r\n", " ");
    sql = sql.trimmed();
    if (sql.isEmpty()) {
        return true;
    }
    fTimer.restart();
    if (!open()) {
        return false;
    }
    if (!fQuery->prepare(sql)) {
        fLastError = fQuery->lastError().databaseText();
        logEvent(fLastError);
        logEvent(sql);
        throw SqlException(fLastError);
        return false;
    }
    for (QMap<QString, QVariant>::iterator it = fBindValues.begin(); it != fBindValues.end(); it++) {
        //        QVariant v = it.value();
        //        if (v.type() == QVariant::Date) {
        //            if (!v.toDate().isValid()) {
        //                v = QDate::fromString("01/01/1990", "dd/MM/yyyy");
        //            }
        //        }
        fQuery->bindValue(it.key(), it.value());
    }
    if (!fQuery->exec()) {
        fLastError = fQuery->lastError().databaseText();
        logEvent(fLastError);
        logEvent(lastQuery(fQuery));
        throw SqlException(fLastError);
        return false;
    }
    fTimerCount = fTimer.elapsed();
#ifdef QT_DEBUG
    logEvent(fDb.hostName() + " (" + QString::number(fTimerCount) + " ms):" + fDb.databaseName() + " " + lastQuery(fQuery));
#else
    if (__c5config.getValue(param_debuge_mode).toInt() > 0) {
        logEvent(" (" + QString::number(fTimerCount) + " ms):" + " " + lastQuery(fQuery));
    }
#endif
    isSelect = fQuery->isSelect();
    QString call = "call";
    if (!isSelect) {
        isSelect = sql.midRef(0, 4).compare(QStringRef( &call), Qt::CaseInsensitive) == 0;
    }
    return true;
}

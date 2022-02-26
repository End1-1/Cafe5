#include "database.h"
#include "logwriter.h"
#include <QDateTime>
#include <QSqlRecord>
#include <QUuid>

QMutex Database::fMutex;
int Database::fDatabaseCounter = 0;

Database::Database() :
    Database("QMYSQL")
{
    QMutexLocker ml(&fMutex);
    fDatabaseNumber = QString::number(++fDatabaseCounter);
}

Database::Database(Database &other) :
    Database("QMYSQL")
{
    open(other.fSqlDatabase.hostName(), other.fSqlDatabase.databaseName(), other.fSqlDatabase.userName(), other.fSqlDatabase.password());
}

Database::Database(const QString &driverName)
{
    fQuery = nullptr;
    fDatabaseDriver = driverName;
    QMutexLocker ml(&fMutex);
    fDatabaseNumber = QString::number(++fDatabaseCounter);
}

Database::~Database()
{
    if (fQuery) {
        fQuery->finish();
        delete fQuery;
    }
    fSqlDatabase = QSqlDatabase::addDatabase(fDatabaseDriver);
    QSqlDatabase::removeDatabase(fDatabaseNumber);
}

bool Database::open(const QString &host, const QString &schema, const QString &username, const QString &password)
{
    fSqlDatabase = QSqlDatabase::addDatabase(fDatabaseDriver, fDatabaseNumber);
    fSqlDatabase.setHostName(host);
    fSqlDatabase.setDatabaseName(schema);
    fSqlDatabase.setUserName(username);
    fSqlDatabase.setPassword(password);
    if (!fSqlDatabase.open()) {
        return false;
    }
    fQuery = new QSqlQuery(fSqlDatabase);
    return true;
}

bool Database::startTransaction()
{
    return fSqlDatabase.transaction();
}

bool Database::commit()
{
    return fSqlDatabase.commit();
}

bool Database::rollback()
{
    return fSqlDatabase.rollback();
}

bool Database::exec(const QString &query)
{
    if (!fQuery->prepare(query)) {
        LogWriter::write(LogWriterLevel::errors, "", fQuery->lastError().databaseText());
        LogWriter::write(LogWriterLevel::errors, "", lastQuery());
        return false;
    }
    QStringList keys = fBindValues.keys();
    for (const QString &key: qAsConst(keys)) {
        fQuery->bindValue(key, fBindValues[key]);
    }
    fBindValues.clear();
    if (!fQuery->exec()) {
        LogWriter::write(LogWriterLevel::errors, "", fQuery->lastError().databaseText());
        LogWriter::write(LogWriterLevel::errors, "", lastQuery());
    }
    LogWriter::write(LogWriterLevel::verbose, "", lastQuery());
    bool isSelect = fQuery->isSelect();
    if (isSelect) {
        //fQuery->first();
        fColumnsNames.clear();
        QSqlRecord rec = fQuery->record();
        for (int i = 0; i < rec.count(); i++) {
            fColumnsNames[rec.fieldName(i).toLower()] = i;
            fColumnsIndexes[i] = rec.fieldName(i).toLower();
        }
    }
    return true;
}

bool Database::insert(const QString &table)
{
    QString sql = "insert into " + table;
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
    return exec(sql);
}

bool Database::insert(const QString &table, int &id)
{
    id = 0;
    if (insert(table)) {
        fQuery->exec("select last_insert_id()");
        if (fQuery->next()) {
            id = fQuery->value(0).toInt();
        }
    }
    return id > 0;
}

bool Database::update(const QString &table)
{
    return update(table, "fid", fBindValues[":fid"]);
}

bool Database::update(const QString &table, const QString &field, const QVariant &value)
{
    fBindValues[":" + field] = value;
    QString sql = "update " + table + " set ";
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
    sql += QString(" where %1=:%1").arg(field);
    return exec(sql);
}

QString Database::uuid()
{
    return QUuid::createUuid().toString().replace("{", "").replace("}", "");
}

void Database::setBindValues(const QMap<QString, QVariant> &v)
{
    fBindValues = v;
}

QMap<QString, QVariant> Database::getBindValues()
{
    QMap<QString, QVariant> b;
    for (QHash<QString, int>::const_iterator it = fColumnsNames.begin(); it != fColumnsNames.end(); it++) {
        b[":" + it.key()] = fQuery->value(fColumnsNames[it.key()]);
    }
    return b;
}

void Database::close()
{
    if (fQuery) {
        fQuery->clear();
    }
    fSqlDatabase.close();
}

QVariant &Database::operator[](const QString &name)
{
    return fBindValues[name];
}

const QString Database::lastQuery()
{
    QString sql = fQuery->lastQuery();
    QMapIterator<QString, QVariant> it(fQuery->boundValues());
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
        case QVariant::ByteArray:
            value = QString("'%1'").arg(QString(value.toByteArray().toHex()));
            break;
        default:
            break;
        }
        sql.replace(QRegExp(it.key() + "\\b"), value.toString());
    }
    return sql;
}

QString Database::lastDbError() const
{
    return fSqlDatabase.lastError().databaseText();
}

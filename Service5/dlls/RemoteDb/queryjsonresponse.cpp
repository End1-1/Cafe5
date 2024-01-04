#include "queryjsonresponse.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlRecord>

QueryJsonResponse::QueryJsonResponse(Database &db, const QJsonObject &ji, QJsonObject &jo) :
    fDb(db),
    fJsonIn(ji),
    fJsonOut(jo)
{

}

void QueryJsonResponse::getResponse()
{
    fJsonOut["kStatus"] = 1;
    fJsonOut["kData"] = "";

    if (fJsonIn.contains("sqlselect")) {
        if (!fDb.exec(fJsonIn["sqlselect"].toString())) {
            fJsonOut["kStatus"] = 4;
            fJsonOut["kData"] = fDb.lastDbError();
            fDb.rollback();
            return;
        }
        QSqlRecord r = fDb.fQuery->record();
        QJsonObject jFields;
        QJsonArray jData;
        for (int i = 0; i < r.count(); i++) {
            jFields[r.fieldName(i)] = i;
            jFields[QString::number(i)] = r.fieldName(i);
        }
        while (fDb.next()) {
            QJsonArray jrow;
            for (int i = 0; i < fDb.columnCount(); i++) {
                switch (fDb.value(i).type()) {
                case QVariant::Double:
                    jrow.append(fDb.value(i).toDouble());
                    break;
                case QVariant::Int:
                    jrow.append(fDb.value(i).toInt());
                    break;
                case QVariant::Date:
                    jrow.append(fDb.value(i).toDate().toString("dd/MM/yyyy"));
                    break;
                case QVariant::Time:
                    jrow.append(fDb.value(i).toTime().toString("HH:mm:ss"));
                    break;
                case QVariant::DateTime:
                    jrow.append(fDb.value(i).toDateTime().toString("dd/MM/yyyy HH:mm:ss"));
                    break;
                default:
                    jrow.append(fDb.value(i).toString());
                    break;
                }
            }
            jData.append(jrow);
        }
        QJsonObject jt;
        jt["table"] = jFields;
        jt["data"] = jData;
        fJsonOut["kData"] = jt;
        return;
    }

    if (fJsonIn.contains("sqllist")) {
        QStringList sqlList = fJsonIn["sql"].toString().split(";", Qt::SkipEmptyParts);
        fDb.startTransaction();
        for (const QString &s: qAsConst(sqlList)) {
            if (!fDb.exec(s)) {
                fJsonOut["kStatus"] = 4;
                fJsonOut["kData"] = fDb.lastDbError();
                fDb.rollback();
                return;
            }
        }
        fDb.commit();
        return;
    }

    if (fJsonIn["call"].toString().isEmpty()) {
        fJsonOut["kStatus"] = 4;
        fJsonOut["kData"] = "Unknown query";
        return;
    }

    fDb.startTransaction();
    if (fJsonIn["format"].toInt() == 1 || fJsonIn["format"].toInt() == 2) {
        if (!fDb.exec(QString("call %1('%2')").arg(fJsonIn["call"].toString(),
                                                 QJsonDocument(fJsonIn["params"].toObject()).toJson(QJsonDocument::Compact)))) {
            fJsonOut["kStatus"] = 4;
            fJsonOut["kData"] = fDb.lastDbError();
            fDb.rollback();
            return;
        }
        if (!fDb.exec("select * from ret")) {
            fJsonOut["kStatus"] = 4;
            fJsonOut["kData"] = fDb.lastDbError();
            fDb.rollback();
            return;
        }
        if (fJsonIn["format"].toInt() == 1) {
            if (!fDb.next()) {
                fJsonOut["kStatus"] = 4;
                fJsonOut["kData"] = "Missing database records";
                fDb.rollback();
                return;
            }
            fJsonOut = QJsonDocument::fromJson(fDb.string(0).toUtf8()).object();
        } else {
            QJsonArray ja;
            dbToArray(ja);
            fJsonOut["kData"] = ja;
        }
    } else if (fJsonIn["format"].toInt() == 3) {
        if (!fDb.exec(QString("select %1('%2')").arg(fJsonIn["call"].toString(),
                                                   QJsonDocument(fJsonIn["params"].toObject()).toJson(QJsonDocument::Compact)))) {
            fJsonOut["kStatus"] = 4;
            fJsonOut["kData"] = fDb.lastDbError();
            fDb.rollback();
            return;
        }
        if (!fDb.next()) {
            fJsonOut["kStatus"] = 4;
            fJsonOut["kData"] = "Missing database records";
            fDb.rollback();
            return;
        }
        qDebug() << fDb.string(0);
        fJsonOut = QJsonDocument::fromJson(fDb.string(0).toUtf8()).object();
    }
    fDb.commit();
}

void QueryJsonResponse::dbToArray(QJsonArray &ja)
{
    while (fDb.next()) {
        QJsonObject jo;
        for (int i = 0; i < fDb.columnCount(); i++){
            switch (fDb.value(i).type()) {
            case QVariant::Int:
            case QVariant::Char:
            case QVariant::LongLong:
            case QVariant::ULongLong:
                jo[fDb.columnName(i)] = fDb.integer(i);
                break;
            case QVariant::Double:
                jo[fDb.columnName(i)] = fDb.doubleValue(i);
                break;
            default:
                jo[fDb.columnName(i)] = fDb.string(i);
                break;
            }
        }
        ja.append(jo);
    }
}

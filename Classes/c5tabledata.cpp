#include "c5tabledata.h"
#include "logwriter.h"

C5TableData* C5TableData::mInstance = nullptr;

C5TableData::C5TableData(QObject *parent)
    : QObject{parent}
{}

QJsonObject C5TableData::jsonObject(const QString &table, const QString &id) const
{
#ifdef QT_DEBUG
    Q_ASSERT(data.contains(table));
#endif
    QJsonObject o = data[table].toObject();
#ifdef QT_DEBUG
    Q_ASSERT(o.contains(id));
#endif

    if(!o.contains(id)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("TableData %1 not containe value with %2").arg(table, id));
        LogWriter::write(LogWriterLevel::errors, "Keys", data[table].toObject().keys().join(","));
    }

    return o[id].toObject();
}

QVariant C5TableData::variant(const QString &table, const QString &field, int id)
{
    QJsonObject o = jsonObject(table, QString::number(id));
    return o[field].toVariant();
}

QJsonArray C5TableData::objects(const QString &table)
{
#ifdef QT_DEBUG
    Q_ASSERT(data.contains(table));
#endif
    QStringList keys = data[table].toObject().keys();
    QJsonArray o;

    for(const QString &k : qAsConst(keys)) {
        o.append(data[table].toObject()[k].toObject());
    }

    return o;
}

C5TableData* C5TableData::instance()
{
    if(!mInstance) {
        mInstance = new C5TableData();
    }

    return mInstance;
}

int C5TableData::version()
{
    return data["version"].toInt();
}

QStringList C5TableData::ids(const QString &table)
{
#ifdef QT_DEBUG
    Q_ASSERT(data.contains(table));
#endif
    return data[table].toObject().keys();
}

QMap<QString, int> C5TableData::cashNames()
{
    QMap<QString, int> l;
    QStringList keys = data["e_cash_names"].toObject().keys();

    for(const QString &k : keys) {
        l[data["e_cash_names"].toObject()[k].toObject()["f_name"].toString()] = k.toInt();
    }

    return l;
}

void C5TableData::setStopList(const QJsonArray &ja)
{
    mStopList.clear();

    for(int i = 0; i < ja.size(); i++) {
        const QJsonObject &j  = ja.at(i).toObject();
        mStopList[j["f_dish"].toInt()] = j["f_qty"].toDouble();
    }
}

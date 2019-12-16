#include "c5cafecommon.h"
#include "c5config.h"
#include "c5utils.h"

QList<CreditCards> C5CafeCommon::fCreditCards;
QStringList C5CafeCommon::fDishRemoveReason;
QStringList C5CafeCommon::fDishComments;
QJsonArray C5CafeCommon::fHalls;
QJsonArray C5CafeCommon::fTables;
QJsonArray C5CafeCommon::fShifts;
QMap<int, QMap<int, QString> > C5CafeCommon::fHallConfigs;

C5CafeCommon::C5CafeCommon()
{

}

QString C5CafeCommon::creditCardName(int id)
{
    foreach (CreditCards cc, fCreditCards) {
        if (cc.id == id) {
            return cc.name;
        }
    }
    return "UNKNOWN";
}

QJsonObject C5CafeCommon::table(const QString &id)
{
    for (int i = 0; i < fTables.count(); i++) {
        QJsonObject t = fTables.at(i).toObject();
        if (t["f_id"].toString() == id) {
            return t;
        }
    }
    return QJsonObject();
}

QJsonObject C5CafeCommon::table(int id)
{
    return table(QString::number(id));
}

QJsonObject C5CafeCommon::hall(const QString &id)
{
    for (int i = 0; i < fHalls.count(); i++) {
        QJsonObject h = fHalls.at(i).toObject();
        if (h["f_id"].toString() == id) {
            return h;
        }
    }
    return QJsonObject();
}

QString C5CafeCommon::serviceMode(const QString &hall)
{
    if (fHallConfigs[hall.toInt()][param_cafe_service_mode].toInt() == 0) {
        return QString::number(SERVICE_AMOUNT_MODE_INCREASE_PRICE);
    }
    return fHallConfigs[hall.toInt()][param_cafe_service_mode];
}

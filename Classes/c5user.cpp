#include "c5user.h"
#include <QJsonArray>

C5User::C5User()
{
    fId = 0;
}

void C5User::fromJson(const QJsonObject &obj)
{
    QJsonArray ju = obj["user"].toArray();
    if (ju.count() > 0) {
        QJsonObject u = ju.at(0).toObject();
        fId = u["f_id"].toString().toInt();
        fGroup = u["f_group"].toString();
        fFirst = u["f_first"].toString();
        fLast = u["f_last"].toString();
        fFull = fLast + " " + fFirst;
    }
}

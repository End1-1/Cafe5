#include "c5cafecommon.h"

QList<CreditCards> C5CafeCommon::fCreditCards;
QStringList C5CafeCommon::fDishRemoveReason;
QJsonArray C5CafeCommon::fHalls;
QJsonArray C5CafeCommon::fTables;

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

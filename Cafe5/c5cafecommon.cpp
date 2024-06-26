#include "c5cafecommon.h"
#include "c5config.h"
#include "c5utils.h"

QList<CreditCards> C5CafeCommon::fCreditCards;
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

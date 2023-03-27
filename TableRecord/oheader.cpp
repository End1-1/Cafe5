#include "oheader.h"
#include "c5utils.h"

OHeader::OHeader()
{
    id = "";
    hallId = 0;
    prefix = "";
    state = 0;
    hall = 0;
    table = 0;
    timeOpen = QTime::currentTime();
    timeClose = QTime::currentTime();
    dateOpen = QDate::currentDate();
    dateClose = QDate::currentDate();
    dateCash = QDate::currentDate();
    cashier = 0;
    staff = 0,
    comment = "";
    print = 0;
    amountTotal = 0;
    amountCash = 0;
    amountCard = 0;
    amountIdram = 0;
    amountTelcell = 0;
    amountPrepaid = 0;
    amountBank = 0;
    amountDebt = 0;
    amountHotel = 0;
    amountPayX = 0.0;
    amountOther = 0;
    amountCashIn = 0;
    amountChange = 0;
    amountService= 0;
    amountDiscount = 0;
    serviceFactor = 0;
    discountFactor = 0;
    bankCard = 0;
    shift = 0;
    source = 1;
    saleType = 0;
    currentStaff = 0;
    partner = 0;
    guests = 0;
    precheck = 0;
    currency = 0;
}

bool OHeader::write(C5Database &db, QString &err)
{
    bool u = true;
    if (id.isEmpty()) {
        u = false;
        id = db.uuid();
    }
    db[":f_id"] = id;
    db[":f_hallid"] = hallId;
    db[":f_prefix"] = prefix;
    db[":f_state"] = state;
    db[":f_hall"] = hall;
    db[":f_table"] = table;
    db[":f_dateopen"] = dateOpen;
    db[":f_dateclose"] = dateClose;
    db[":f_datecash"] = dateCash;
    db[":f_timeopen"] = timeOpen;
    db[":f_timeclose"] = timeClose;
    db[":f_cashier"] = cashier;
    db[":f_staff"] = staff;
    db[":f_comment"] = comment;
    db[":f_print"] = print;
    db[":f_amounttotal"] = amountTotal;
    db[":f_amountcash"] = amountCash;
    db[":f_amountcard"] = amountCard;
    db[":f_amountprepaid"] = amountPrepaid;
    db[":f_amountbank"] = amountBank;
    db[":f_amountother"] = amountOther;
    db[":f_amountidram"] = amountIdram;
    db[":f_amountservice"] = amountService;
    db[":f_amountdiscount"] = amountDiscount;
    db[":f_servicefactor"] = serviceFactor;
    db[":f_discountfactor"] = discountFactor;
    db[":f_amountdebt"] = amountDebt;
    db[":f_creditcardid"] = bankCard;
    db[":f_otherid"] = 0;
    db[":f_shift"] = shift;
    db[":f_source"] = source;
    db[":f_saletype"] = saleType;
    db[":f_partner"] = partner;
    db[":f_currentStaff"] = currentStaff;
    db[":f_guests"] = guests;
    db[":f_precheck"] = precheck;
    db[":f_currency"] = currency;
    if (u) {
        return getWriteResult(db, db.update("o_header", where_id(id)), err);
    } else {
        bool b = getWriteResult(db, db.insert("o_header", false), err);
        if (b) {
            db[":f_id"] = id;
            db[":f_1"] = 0;
            db[":f_2"] = 0;
            db[":f_3"] = 0;
            db[":f_4"] = 0;
            db[":f_5"] = 0;
            b = b && getWriteResult(db, db.insert("o_header_flags", false), err);
            db[":f_id"] = id;
            b = b && getWriteResult(db, db.insert("o_header_options", false), err);
        }
        return b;
    }
    return true;
}

QString OHeader::humanId() {
    return QString("%1%2").arg(prefix).arg(hallId);
}

bool OHeader::hasIdram()
{
    return amountIdram > 0.001 || amountTelcell > 0.001;
}

void OHeader::countAmount(QVector<OGoods> &goods, BHistory &bhistory)
{
    amountTotal = 0;
    amountDiscount = 0;
    for (int i = 0; i < goods.count(); i++) {
        OGoods &g = goods[i];
        g.total = g.qty  * g.price;
        amountTotal += g.total;
        switch (g.discountMode) {
        case CARD_TYPE_DISCOUNT:
            g.discountAmount = g.total * g.discountFactor;
            g.total -= g.discountAmount;
            amountTotal -= g.discountAmount;
            amountDiscount += g.discountAmount;
            break;
        case CARD_TYPE_ACCUMULATIVE:
            g.accumulateAmount = g.total * g.discountFactor;
            break;
        case CARD_TYPE_COUNT_ORDER:
            break;
        case CARD_TYPE_MANUAL:
            amountTotal -= g.discountAmount;
            amountDiscount += g.discountAmount;
            g.total -= g.discountAmount;
            break;
        default:
            break;
        }
    }
}

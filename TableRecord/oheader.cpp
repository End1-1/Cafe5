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
    amountCredit = 0;
    amountDebt = 0;
    amountHotel = 0;
    amountPayX = 0.0;
    amountOther = 0;
    amountCashIn = 0;
    amountChange = 0;
    amountService = 0;
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

void OHeader::bind(C5Database& db)
{
    db[":f_id"] = _id();
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
    db[":f_amountcredit"] = amountCredit;
    db[":f_amountother"] = amountOther;
    db[":f_amountidram"] = amountIdram;
    db[":f_amounttelcell"] = amountTelcell;
    db[":f_amountpayx"] = amountPayX;
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
    db[":f_cash"] = amountCashIn;
    db[":f_change"] = amountChange;
    db[":f_taxpayertin"] = taxpayerTin;
}

bool OHeader::getRecord(C5Database& db)
{
    if (!db.nextRow()) {
        return false;
    }

    id = db.getValue("f_id");
    hallId = db.getInt("f_hallid");
    prefix = db.getString("f_prefix");
    state = db.getInt("f_state");
    hall = db.getInt("f_hall");
    table = db.getInt("f_table");
    dateOpen = db.getDate("f_dateopen");
    dateClose = db.getDate("f_dateclose");
    timeOpen = db.getTime("f_timeopen");
    timeClose = db.getTime("f_timeclose");
    dateCash = db.getDate("f_datecash");
    cashier = db.getInt("f_cashier");
    staff = db.getInt("f_staff");
    comment = db.getString("f_comment");
    print = db.getInt("f_print");
    amountTotal = db.getDouble("f_amounttotal");
    amountCash = db.getDouble("f_amountcash");
    amountCard = db.getDouble("f_amountcard");
    amountBank = db.getDouble("f_amountbank");
    amountCredit = db.getDouble("f_amountcredit");
    amountCashIn = db.getDouble("f_cash");
    amountChange = db.getDouble("f_change");
    amountOther = db.getDouble("f_amountother");
    amountIdram = db.getDouble("f_amountidram");
    amountTelcell = db.getDouble("f_amounttelcell");
    amountPayX = db.getDouble("f_amountpayx");
    amountPrepaid = db.getDouble("f_amountprepaid");
    amountDebt = db.getDouble("f_amountdebt");
    amountService = db.getDouble("f_amountservice");
    amountDiscount = db.getDouble("f_amountdiscount");
    serviceFactor = db.getDouble("f_servicefactor");
    discountFactor = db.getDouble("f_discountfactor");
    bankCard = db.getInt("f_creditcardid");
    shift = db.getInt("f_shift");
    source = db.getInt("f_source");
    saleType = db.getInt("f_saletype");
    partner = db.getInt("f_partner");
    currentStaff = db.getInt("f_currentstaff");
    guests = db.getInt("f_guests");
    precheck = db.getInt("f_precheck");
    currency = db.getInt("f_currency");
    taxpayerTin = db.getString("f_taxpayertin");
    return true;
}

bool OHeader::write(C5Database& db, QString& err)
{
    bool u = true;

    if (id.toString().isEmpty()) {
        u = false;
        id = db.uuid();
    }

    bind(db);

    if (u) {
        return update(db, "o_header", err);
    }
    else {
        bool b = insert(db, "o_header", err);

        if (b) {
            db[":f_id"] = _id();
            db[":f_1"] = 0;
            db[":f_2"] = 0;
            db[":f_3"] = 0;
            db[":f_4"] = 0;
            db[":f_5"] = 0;
            b = b && getWriteResult(db, db.insert("o_header_flags", false), err);
            db[":f_id"] = _id();
            b = b && getWriteResult(db, db.insert("o_header_options", false), err);
        }

        return b;
    }

    return true;
}

QString OHeader::humanId()
{
    return QString("%1%2").arg(prefix).arg(hallId);
}

bool OHeader::hasIdram()
{
    return amountIdram > 0.001 || amountTelcell > 0.001;
}

void OHeader::countAmount(QVector<OGoods>& goods, BHistory& bhistory)
{
    Q_UNUSED(bhistory);
    amountTotal = 0;
    amountDiscount = 0;

    for (int i = 0; i < goods.count(); i++) {
        OGoods& g = goods[i];
        g.row = i;
        g.total = g.qty * g.price;
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
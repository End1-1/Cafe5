#include "c5waiterorderdoc.h"
#include "c5utils.h"
#include "c5cafecommon.h"
#include "c5config.h"
#include "c5sockethandler.h"
#include "c5sockethandler.h"
#include <QHostInfo>

C5WaiterOrderDoc::C5WaiterOrderDoc() {
    fSaved = true;
}

C5WaiterOrderDoc::C5WaiterOrderDoc(const QString &id, C5Database &db) :
    fDb(db)
{
    fHeader["f_id"] = id;
    fSaved = true;
    open();
}

C5WaiterOrderDoc::C5WaiterOrderDoc(QJsonObject &jh, QJsonArray &jb, C5Database &db) :
    fDb(db)
{
    fHeader = jh;
    fItems = jb;
    fSaved = true;
    getTaxInfo();
}

C5WaiterOrderDoc::~C5WaiterOrderDoc()
{

}

int C5WaiterOrderDoc::itemsCount()
{
    return fItems.count();
}

QJsonObject C5WaiterOrderDoc::item(int index)
{
    return fItems[index].toObject();
}

void C5WaiterOrderDoc::addItem(const QJsonObject &obj)
{
    fItems.append(obj);
    fSaved = false;
}

void C5WaiterOrderDoc::sendToServer(C5SocketHandler *sh)
{
    sh->bind("cmd", sm_saveorder);
    QJsonObject o;
    o["header"] = fHeader;
    o["body"] = fItems;
    sh->send(o);
}

bool C5WaiterOrderDoc::transferToHotel(C5Database &fDD, QString &err)
{
    int settings = 0;
    int item = 0;
    QString itemName;
    fDb[":f_id"] = fHeader["f_hall"].toString().toInt();
    fDb.exec("select f_settings from h_halls where f_id=:f_id");
    if (fDb.nextRow()) {
        settings = fDb.getInt(0);
    }
    if (settings == 0) {
        err = "Cannot get settings for hall";
        return false;
    }
    fDb[":f_settings"] = settings;
    fDb[":f_key"] = param_item_code_for_hotel;
    fDb.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
    if (fDb.nextRow()) {
        item = fDb.getString(0).toInt();
    }
    if (item == 0) {
        //err = "Cannot retrieve invoice item for hotel";
        return true;
    }

    QString result = fHeader["f_prefix"].toString() + fHeader["f_hallid"].toString();
    correctHotelID(result, fDD, err);
    if (!err.isEmpty()) {
        return false;
    }
    int paymentMode = 1;
    QString dc = "DEBET";
    int sign = -1;

    QString room, res, inv, clcode, clname, guest;
    if (fHeader["f_otherid"].toString().toInt() == PAYOTHER_TRANSFER_TO_ROOM) {
        room = fHeader["f_other_room"].toString();
        inv = fHeader["f_other_inv"].toString();
        res = fHeader["f_other_res"].toString();
        guest = fHeader["f_other_guest"].toString();
        paymentMode = 5;
        dc = "CREDIT";
        sign = 1;
    } else if (hInt("f_otherid") == PAYOTHER_COMPLIMENTARY) {
        paymentMode = 6;
    } else if (hInt("f_otherid") == PAYOTHER_SELFCOST) {
        paymentMode = 14;
    }

    if (fHeader["f_otherid"].toString().toInt() == PAYOTHER_CL) {
        clcode = fHeader["f_other_clcode"].toString();
        clname = fHeader["f_other_clname"].toString();
        paymentMode = 4;
        dc = "DEBIT";
        sign = -1;
    }

    if (fHeader["f_amountcard"].toString().toDouble() > 0.001) {
        paymentMode = 2;
    }

    if (fHeader["f_amountbank"].toString().toDouble() > 0.001) {
        paymentMode = 3;
    }

    fDD.open();
    fDD.startTransaction();
    fDD[":f_id"] = item;
    fDD.exec("select f_en from f_invoice_item where f_id=:f_id");
    if (fDD.nextRow()) {
        itemName = fDD.getString(0);
    } else {
        err = "Cannot retrieve invoice item for hotel";
        fDD.rollback();
        return false;
    }
    QString payComment = "CASH";
    if (hDouble("f_amountcard") > 0.001) {
        payComment = C5CafeCommon::creditCardName(hInt("f_creditcardid"));
    }
    if (!clcode.isEmpty()) {
        payComment = clname;
    }
    fDD[":f_id"] = result;
    fDD[":f_source"] = "PS";
    fDD[":f_res"] = res;
    fDD[":f_wdate"] = QDate::fromString(hString("f_datecash"), ("dd/MM/yyyy"));
    fDD[":f_rdate"] = QDate::currentDate();
    fDD[":f_time"] = QTime::currentTime();
    fDD[":f_user"] = 1;
    fDD[":f_room"] = hInt("f_otherid") == PAYOTHER_TRANSFER_TO_ROOM ? room : clcode;
    fDD[":f_guest"] = hInt("f_otherid") == PAYOTHER_TRANSFER_TO_ROOM ? guest : clname + ", " + hString("f_prefix") + hString("f_hallid");
    fDD[":f_itemCode"] = item;
    fDD[":f_finalName"] = itemName + " " + hString("f_prefix") + hString("f_hallid");
    fDD[":f_amountAmd"] = hDouble("f_amounttotal");
    fDD[":f_usedPrepaid"] = 0;
    fDD[":f_amountVat"] = hDouble("f_amounttotal") - (hDouble("f_amounttotal") / 1.2);
    fDD[":f_amountUsd"] = 0;
    fDD[":f_fiscal"] = fTax["f_receiptnumber"].toString().toInt();
    fDD[":f_paymentMode"] = paymentMode;
    fDD[":f_creditCard"] = hInt("f_creditcardid");
    fDD[":f_cityLedger"] = clcode.toInt();
    fDD[":f_paymentComment"] = payComment;
    fDD[":f_dc"] = dc;
    fDD[":f_sign"] = sign;
    fDD[":f_doc"] = "";
    fDD[":f_rec"] = "";
    fDD[":f_inv"] = inv;
    fDD[":f_vatmode"] = 1;
    fDD[":f_finance"] = 1;
    fDD[":f_remarks"] = "";
    fDD[":f_canceled"] = 0;
    fDD[":f_cancelReason"] = "";
    fDD[":f_cancelDate"] = 0;
    fDD[":f_cancelUser"] = 0;
    fDD[":f_side"] = 0;
    if (!fDD.insert("m_register", false)) {
        err = "Cannot insert into m_register<br>" + fDD.fLastError;
        fDD.rollback();
        return false;
    }

    fDD[":f_id"] = result;
    fDD[":f_state"] = 2;
    fDD[":f_hall"] = hInt("f_hall");
    fDD[":f_table"] = hInt("f_table");
    fDD[":f_staff"] = 1;
    fDD[":f_dateopen"] = QDateTime::fromString(hString("f_dateopen") + " " + hString("f_timeopen"), "dd/MM/yyyy HH:mm:ss");
    fDD[":f_dateclose"] = QDateTime::fromString(hString("f_dateclose") + " " + hString("f_timeclose"), "dd/MM/yyyy HH:mm:ss");
    fDD[":f_datecash"] = QDate::fromString(hString("f_datecash"), ("dd/MM/yyyy"));
    fDD[":f_comment"] = "";
    fDD[":f_paymentModeComment"] = hInt("f_otherid") == PAYOTHER_TRANSFER_TO_ROOM ? QString("%1, %2").arg(room).arg(guest) : "";
    fDD[":f_paymentMode"] = paymentMode;
    fDD[":f_cityLedger"] = clcode.toInt();
    fDD[":f_reservation"] = res;
    fDD[":f_complex"] = 0;
    fDD[":f_print"] = hInt("f_print");
    fDD[":f_tax"] = fTax["f_receiptnumber"].toString().toInt();
    fDD[":f_roomComment"] = "";
    fDD[":f_total"] = hDouble("f_total");
    if (!fDD.insert("o_header", false)) {
        err = "Cannot insert into o_header<br>" + fDD.fLastError;
        fDD.rollback();
        return false;
    }

    for (int i = 0; i < fItems.count(); i++) {
        QJsonObject o = fItems.at(i).toObject();
        if (o["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        fDD[":f_id"] = getHotelID(fDD, "DR", err);
        fDD[":f_state"] = 1;
        fDD[":f_header"] = result;
        fDD[":f_dish"] = o["f_dish"].toString().toInt();
        fDD[":f_qty"] = o["f_qty1"].toString().toDouble();
        fDD[":f_qtyprint"] = o["f_qty2"].toString().toDouble();
        fDD[":f_price"] = o["f_price"].toString().toDouble();
        fDD[":f_svcvalue"] = 0;
        fDD[":f_svcamount"] = 0;
        fDD[":f_dctvalue"] = 0;
        fDD[":f_dctamount"] = 0;
        fDD[":f_total"] = o["f_total"].toString().toDouble();
        fDD[":f_totalusd"] = 0;
        fDD[":f_print1"] = "";
        fDD[":f_print2"] = "";
        fDD[":f_store"] = o["f_store"].toString().toInt();
        fDD[":f_comment"] = "";
        fDD[":f_staff"] = 1;
        fDD[":f_complex"] = 0;
        fDD[":f_complexid"] = 0;
        fDD[":f_adgt"] = o["f_adgcode"].toString();
        fDD[":f_complexRec"] = 0;
        fDD[":f_canceluser"] = 0;
        if (!fDD.insert("o_dish", false)) {
            err = "Cannot insert into o_dish<br>" + fDD.fLastError;
            fDD.rollback();
            return false;
        }
    }
    if (hDouble("f_servicefactor") > 0.001) {
        if (hInt("f_servicemode") == SERVICE_AMOUNT_MODE_SEPARATE) {
            fDD[":f_header"] = result;
            fDD[":inc"] = hDouble("f_servicefactor");
            fDD.exec("update o_dish set f_price=f_price + (f_price * :inc) where f_header=:f_header");
            fDD[":f_header"] = result;
            fDD.exec("update o_dish set f_total=f_qty * f_price where f_header=:f_header");
        }
    }
    fDD[":f_comp"] = QHostInfo::localHostName().toUpper();
    fDD[":f_date"] = QDate::currentDate();
    fDD[":f_time"] = QTime::currentTime();
    fDD[":f_user"] = 1;
    fDD[":f_type"] = 23;
    fDD[":f_rec"] = hString("f_prefix") + hString("f_hallid");
    fDD[":f_invoice"] = inv;
    fDD[":f_reservation"] = res;
    fDD[":f_action"] = "IMPORT FROM WAITER";
    fDD[":f_value1"] = hString("f_prefix") + hString("f_hallid");
    fDD[":f_value2"] = hString("f_amounttotal");
    fDD.insert("airlog.log", false);
    fDD.commit();
    return true;
}

int C5WaiterOrderDoc::hInt(const QString &name)
{
    return fHeader[name].toString().toInt();
}

double C5WaiterOrderDoc::hDouble(const QString &name)
{
    return fHeader[name].toString().toDouble();
}

QString C5WaiterOrderDoc::hString(const QString &name)
{
    return fHeader[name].toString();
}

void C5WaiterOrderDoc::hSetString(const QString &name, const QString &value)
{
    fHeader[name] = value;
}

void C5WaiterOrderDoc::hSetInt(const QString &name, int value)
{
    hSetString(name, QString::number(value));
}

void C5WaiterOrderDoc::hSetDouble(const QString &name, double value)
{
    hSetString(name, QString::number(value, 'f', 2));
}

int C5WaiterOrderDoc::iInt(const QString &name, int index)
{
    return fItems[index].toObject()[name].toString().toInt();
}

double C5WaiterOrderDoc::iDouble(const QString &name, int index)
{
    return fItems[index].toObject()[name].toString().toDouble();
}

QString C5WaiterOrderDoc::iString(const QString &name, int index)
{
    return fItems[index].toObject()[name].toString();
}

void C5WaiterOrderDoc::iSetString(const QString &name, const QString &value, int index)
{
    QJsonObject o = fItems[index].toObject();
    o[name] = value;
    fItems[index] = o;
}

void C5WaiterOrderDoc::iSetInt(const QString &name, int value, int index)
{
    iSetString(name, QString::number(value), index);
}

void C5WaiterOrderDoc::iSetDouble(const QString &name, double value, int index)
{
    iSetString(name, QString::number(value, 'f', 2), index);
}

QString C5WaiterOrderDoc::prepayment()
{
    switch (hInt("f_servicemode")) {
    case SERVICE_AMOUNT_MODE_INCREASE_PRICE:
        return float_str(countPreTotalV1(), 2);
    case SERVICE_AMOUNT_MODE_SEPARATE:
        return float_str(countPreTotalV2(), 2);
    default:
        return float_str(countPreTotalV1(), 2);
    }
}

void C5WaiterOrderDoc::countTotal()
{
    switch (hInt("f_servicemode")) {
    case SERVICE_AMOUNT_MODE_INCREASE_PRICE:
        countTotalV1();
        break;
    case SERVICE_AMOUNT_MODE_SEPARATE:
        countTotalV2();
        break;
    default:
        countTotalV1();
        break;
    }
}

void C5WaiterOrderDoc::countTotalV1()
{
    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;
    for (int i = 0, count = fItems.count(); i < count; i++) {
        iSetString("f_discount", hString("f_discountfactor"), i);
        iSetString("f_service", hString("f_servicefactor"), i);
        if (iInt("f_state", i) != DISH_STATE_OK) {
            continue;
        }
        double price = iDouble("f_price", i);
        double service = iDouble("f_service", i);
        double discount = iDouble("f_discount", i);
        double itemTotal = price * iDouble("f_qty2", i);
        double serviceAmount = (itemTotal * service);
        itemTotal += serviceAmount;
        double discountAmount = (itemTotal * discount);
        itemTotal -= discountAmount;
        total += itemTotal;
        totalService += serviceAmount;
        totalDiscount += discountAmount;
        iSetDouble("f_total", itemTotal, i);
    }
    hSetDouble("f_amounttotal", total);
    hSetDouble("f_amountservice", totalService);
    hSetDouble("f_amountdiscount", totalDiscount);
    double acash = hDouble("f_amountcash"),
            acard = hDouble("f_amountcard"),
            abank = hDouble("f_amountbank"),
            aother = hDouble("f_amountother");
    double adiff = total - (acash + acard + abank + aother);
    if (adiff < 0.001) {
        hSetDouble("f_amountother", 0);
        hSetDouble("f_amountbank", 0);
        hSetDouble("f_amountcard", 0);
        hSetDouble("f_amountcash", hDouble("f_amounttotal"));
    }
}

void C5WaiterOrderDoc::countTotalV2()
{
    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;
    for (int i = 0, count = fItems.count(); i < count; i++) {
        iSetString("f_discount", hString("f_discountfactor"), i);
        iSetString("f_service", hString("f_servicefactor"), i);
        if (iInt("f_state", i) != DISH_STATE_OK) {
            continue;
        }
        double price = iDouble("f_price", i);
        double itemTotal = price * iDouble("f_qty2", i);
        iSetDouble("f_total", itemTotal, i);
        total += itemTotal;
    }
    totalService = total * hDouble("f_servicefactor");
    total += totalService;
    totalDiscount = total * hDouble("f_discountfactor");
    total -= totalDiscount;
    hSetDouble("f_amounttotal", total);
    hSetDouble("f_amountservice", totalService);
    hSetDouble("f_amountdiscount", totalDiscount);
    double acash = hDouble("f_amountcash"),
            acard = hDouble("f_amountcard"),
            abank = hDouble("f_amountbank"),
            aother = hDouble("f_amountother");
    double adiff = total - (acash + acard + abank + aother);
    if (adiff < 0.001) {
        hSetDouble("f_amountother", 0);
        hSetDouble("f_amountbank", 0);
        hSetDouble("f_amountcard", 0);
        hSetDouble("f_amountcash", hDouble("f_amounttotal"));
    }
}

double C5WaiterOrderDoc::countPreTotalV1()
{
    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;
    for (int i = 0, count = fItems.count(); i < count; i++) {
        iSetString("f_discount", hString("f_discountfactor"), i);
        iSetString("f_service", hString("f_servicefactor"), i);
        if (iInt("f_state", i) != DISH_STATE_OK) {
            continue;
        }
        double price = iDouble("f_price", i);
        double service = iDouble("f_service", i);
        double discount = iDouble("f_discount", i);
        double itemTotal = price * iDouble("f_qty1", i);
        double serviceAmount = (itemTotal * service);
        itemTotal += serviceAmount;
        double discountAmount = (itemTotal * discount);
        itemTotal -= discountAmount;
        total += itemTotal;
        totalService += serviceAmount;
        totalDiscount += discountAmount;
    }
    return total;
}

double C5WaiterOrderDoc::countPreTotalV2()
{
    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;
    for (int i = 0, count = fItems.count(); i < count; i++) {
        iSetString("f_discount", hString("f_discountfactor"), i);
        iSetString("f_service", hString("f_servicefactor"), i);
        if (iInt("f_state", i) != DISH_STATE_OK) {
            continue;
        }
        double price = iDouble("f_price", i);
        double itemTotal = price * iDouble("f_qty1", i);
        iSetDouble("f_total", itemTotal, i);
        total += itemTotal;
    }
    totalService = total * hDouble("f_servicefactor");
    total += totalService;
    totalDiscount = total * hDouble("f_discountfactor");
    total -= totalDiscount;
    return total;
}

void C5WaiterOrderDoc::open()
{
    fDb[":f_id"] = hString("f_id");
    fDb.exec("select h.f_name as f_hallname, t.f_name as f_tableName, concat(s.f_last, ' ', s.f_first) as f_staffname, \
        o.* \
        from o_header o \
        left join h_tables t on t.f_id=o.f_table \
        left join h_halls h on h.f_id=t.f_hall \
        left join s_user s on s.f_id=o.f_staff \
        where o.f_id=:f_id \
        order by o.f_id ");
    if (fDb.nextRow()) {
        for (int i = 0, count = fDb.columnCount(); i < count; i++) {
            QVariant v = fDb.getValue(i);
            switch (v.type()) {
            case QVariant::Date:
                fHeader[fDb.columnName(i)] = fDb.getDate(i).toString(FORMAT_DATE_TO_STR);
                break;
            case QVariant::DateTime:
                fHeader[fDb.columnName(i)] = fDb.getDateTime(i).toString(FORMAT_DATETIME_TO_STR);
                break;
            default:
                fHeader[fDb.columnName(i)] = fDb.getString(i);
                break;
            }
        }
    }
    getTaxInfo();
    fDb[":f_header"] = fHeader["f_id"].toString();
    fDb.exec("select ob.f_id, ob.f_header, ob.f_state, dp1.f_name as part1, dp2.f_name as part2, ob.f_adgcode, d.f_name as f_name, \
             ob.f_qty1, ob.f_qty2, ob.f_price, ob.f_service, ob.f_discount, ob.f_total, \
             ob.f_store, ob.f_print1, ob.f_print2, ob.f_comment, ob.f_remind, ob.f_dish, \
             s.f_name as f_storename, ob.f_removereason, ob.f_timeorder \
             from o_body ob \
             left join d_dish d on d.f_id=ob.f_dish \
             left join d_part2 dp2 on dp2.f_id=d.f_part \
             left join d_part1 dp1 on dp1.f_id=dp2.f_part \
             left join c_storages s on s.f_id=ob.f_store \
             where ob.f_header=:f_header");
    while (fDb.nextRow()) {
        QJsonObject o;
        for (int i = 0; i < fDb.columnCount(); i++) {
            o[fDb.columnName(i)] = fDb.getString(i);
        }
        fItems.append(o);
    }
    // Discount
    QJsonArray jda;
    fDb[":f_order"] = fHeader["f_id"].toString();
    fDb.exec("select f_id, f_type, f_value from b_history where f_order=:f_order");
    if (fDb.nextRow()) {
        fHeader["f_bonusid"] = fDb.getString("f_id");
        fHeader["f_bonustype"] = fDb.getString("f_type");
        fHeader["f_discountfactor"] = QString::number(fDb.getDouble("f_value") / 100, 'f', 3);
    }
    fDb[":f_id"] = fHeader["f_id"].toString();
    fDb.exec("select * from o_pay_room where f_id=:f_id");
    if (fDb.nextRow()) {
        fHeader["f_other_res"] = fDb.getString("f_res");
        fHeader["f_other_room"] = fDb.getString("f_room");
        fHeader["f_other_guest"] = fDb.getString("f_guest");
        fHeader["f_other_inv"] = fDb.getString("f_inv");
    }
    fDb[":f_id"] = fHeader["f_id"].toString();
    fDb.exec("select * from o_pay_cl where f_id=:f_id");
    if (fDb.nextRow()) {
        fHeader["f_other_clcode"] = fDb.getString("f_code");
        fHeader["f_other_clname"] = fDb.getString("f_name");
    }
}

void C5WaiterOrderDoc::getTaxInfo()
{
    fDb[":f_id"] = hString("f_id");
    fDb.exec("select * from o_tax where f_id=:f_id");
    if (fDb.nextRow()) {
        for (int i = 0; i < fDb.columnCount(); i++) {
            fTax[fDb.columnName(i)] = fDb.getString(i);
        }
    }
}

bool C5WaiterOrderDoc::correctHotelID(QString &id, C5Database &dba, QString &err) {
    dba.open();
    dba.startTransaction();
    int totaltrynum = 0;
    bool success = false;
    QString tempId = id;
    do {
         success = dba.exec(QString("insert into airwick.f_id (f_value, f_try, f_comp, f_user, f_date, f_time, f_db) values ('%1-%2', %3, '%4', '%5', '%6', '%7', database())")
                                         .arg("PS")
                                         .arg(tempId).arg(totaltrynum)
                                         .arg(QHostInfo::localHostName().toUpper())
                                         .arg(1) //user id
                                         .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                                         .arg(QTime::currentTime().toString("HH:mm:ss")));
        if (dba.fLastError.toLower().contains("duplicate entry")) {
            totaltrynum++;
            tempId = QString("%1-%2").arg(id).arg(totaltrynum);
        } else {
            success = true;
        }
    } while (!success);
    if (tempId != id) {
        dba[":f_id1"] = tempId;
        dba[":f_id2"] = id;
        if (!dba.exec("update m_register set f_id=:f_id1, f_canceled=1, f_canceldate=current_date(), f_cancelReason='TRANSFER FROM WAITER', f_canceluser=1 where f_id=:f_id2")) {
            err = "Cannot update m_register 'correctHotelID'<br>" + dba.fLastError;
            dba.rollback();
            return false;
        }
        dba[":f_id1"] = tempId;
        dba[":f_id2"] = id;
        if (!dba.exec("update o_header set f_id=:f_id1, f_state=3 where f_id=:f_id2")) {
            err = "Cannot update o_header 'correctHotelID'<br>" + dba.fLastError;
            dba.rollback();
            return false;
        }
        dba[":f_id1"] = tempId;
        dba[":f_id2"] = id;
        if (!dba.exec("update o_dish set f_header=:f_id1, f_state=3 where f_header=:f_id2")) {
            err = "Cannot update o_dish 'correctHotelID'<br>" + dba.fLastError;
            dba.rollback();
            return false;
        }
    }
    dba.commit();
    return true;
}

QString C5WaiterOrderDoc::getHotelID(C5Database &db, const QString &source, QString &err) {
    C5Database dba(db.dbParams().at(0), "airwick", db.dbParams().at(2), db.dbParams().at(3));
    dba.open();
    int totaltrynum = 0;
    bool done = false;
    QString result;
    do {
        QString query = QString ("select f_max, f_zero from serv_id_counter where f_id='%1' for update").arg(source);
        if (!dba.exec(query)) {
            err = "<H1><font color=\"red\">ID ERROR. COUNTER ID GENERATOR FAIL</font></h1><br>" + dba.fLastError;
            exit(0);
        }
        if (dba.nextRow()) {
            int max = dba.getInt(0) + 1;
            int zero = dba.getInt(1);
            dba[":f_max"] = max;
            dba[":f_id"] = source;
            query = "update serv_id_counter set f_max=:f_max where f_id=:f_id";
            dba.exec(query);
            result = QString("%1").arg(max, zero, 10, QChar('0'));
        } else {
            query = "insert into serv_id_counter (f_id, f_max, f_zero) values ('" + source + "', 0, 6)";
            dba.exec(query);
            totaltrynum++;
            continue;
        }
        dba.exec(QString("insert into airwick.f_id (f_value, f_try, f_comp, f_user, f_date, f_time, f_db) values ('%1-%2', %3, '%4', '%5', '%6', '%7', database())")
             .arg(source)
             .arg(result).arg(totaltrynum)
             .arg(QHostInfo::localHostName().toUpper())
             .arg(1) //user id
             .arg(QDate::currentDate().toString("yyyy-MM-dd"))
             .arg(QTime::currentTime().toString("HH:mm:ss")));
        if (dba.fLastError.toLower().contains("duplicate entry")) {
            totaltrynum++;
        } else {
            done = true;
        }
        if (totaltrynum > 20) {
            err = "<H1><font color=\"red\">ID ERROR, TRYNUM>20. COUNTER ID GENERATOR FAIL, GIVE UP</font></h1>";
            exit(0);
        }
    } while (!done);
    result = source + result;
    return result;
}

void C5WaiterOrderDoc::calculateSelfCost()
{
    QMap<int, QMap<int, double> > goodsQty;
    QMap<int, double> price;
    fDb[":f_id"] = fHeader["f_id"].toString();
    fDb.exec("select b.f_dish, r.f_goods, r.f_qty "
            "from d_recipes r "
            "left join o_body b on b.f_dish=r.f_dish "
            "left join o_header h on h.f_id=b.f_header "
            "where b.f_state=1 and h.f_id=:f_id ");
    while (fDb.nextRow()) {
        goodsQty[fDb.getInt(0)][fDb.getInt(1)] = fDb.getDouble(2);
    }
    fDb.exec("select s.f_goods, sum(s.f_total)/sum(f_qty) "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_state=1 and h.f_type=1 and s.f_type=1 "
            "group by 1");
    while (fDb.nextRow()) {
        price[fDb.getInt(0)] = fDb.getDouble(1);
    }
    double total = 0;
    for (int i = 0; i < fItems.count(); i++) {
        QJsonObject o = fItems.at(i).toObject();
        if (o["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        if (!goodsQty.contains(o["f_dish"].toString().toInt())) {
            total += o["f_total"].toString().toDouble();
            continue;
        }
        double selfcost = 0;
        QMap<int, double> recipe = goodsQty[o["f_dish"].toString().toInt()];
        for (QMap<int, double>::const_iterator it = recipe.begin(); it != recipe.end(); it++) {
            selfcost += price[it.key()] * it.value();
        }
        o["f_price"] = QString::number(selfcost, 'f', 2);
        o["f_service"] = "0";
        o["f_total"] = QString::number(selfcost * o["f_qty1"].toString().toDouble(), 'f', 2);
        fDb[":f_id"] = o["f_id"].toString();
        fDb[":f_price"] = selfcost;
        fDb[":f_total"] = selfcost * o["f_qty1"].toString().toDouble();
        fDb.exec("update o_body set f_price=:f_price, f_total=:f_total where f_id=:f_id");
        total += selfcost * o["f_qty1"].toString().toDouble();
        fItems[i] = o;
    }
    fHeader["f_amountother"] = QString::number(total, 'f', 2);
    fHeader["f_amounttotal"] = QString::number(total, 'f', 2);
    fHeader["f_servicefactor"] = "0";
    fHeader["f_amountservice"] = 0;
    fDb[":f_amountother"] = total;
    fDb[":f_amounttotal"] = total;
    fDb[":f_servicefactor"] = 0;
    fDb[":f_amountservice"] = 0;
    fDb.update("o_header", where_id(fHeader["f_id"].toString()));
}

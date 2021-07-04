#include "c5waiterorderdoc.h"
#include "c5utils.h"
#include "c5cafecommon.h"
#include "c5config.h"
#include "c5sockethandler.h"
#include "c5storedoc.h"
#include "c5storedraftwriter.h"
#include <QHostInfo>

struct tmpg {
    QString recId;
    int goodsId;
    double qtyDraft;
    double qtyGoods;
    int store;
    double price;
    tmpg(){}
    tmpg(const QString &n, double dr, double goods, double pr){
        recId = n;
        qtyDraft = dr;
        qtyGoods = goods;
        price = pr;
    }
};

C5WaiterOrderDoc::C5WaiterOrderDoc():
    QObject()
{
    fSaved = true;
}

C5WaiterOrderDoc::C5WaiterOrderDoc(const QString &id, C5Database &db) :
    C5WaiterOrderDoc()
{
    fHeader["f_id"] = id;
    open(db);
}

C5WaiterOrderDoc::C5WaiterOrderDoc(C5Database &db, QJsonObject &jh, QJsonArray &jb) :
    C5WaiterOrderDoc()
{
    fHeader = jh;
    fItems = jb;
    getTaxInfo(db);
}

C5WaiterOrderDoc::~C5WaiterOrderDoc()
{

}

bool C5WaiterOrderDoc::isEmpty()
{
    for (int i = 0, c = itemsCount(); i < c; i++) {
        if (iInt("f_state", i) == DISH_STATE_OK) {
            return false;
        }
    }
    return true;
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

bool C5WaiterOrderDoc::transferToHotel(C5Database &db, C5Database &fDD, QString &err)
{
    int settings = 0;
    int item = 0;
    QString itemName;
    db[":f_id"] = fHeader["f_hall"].toString().toInt();
    db.exec("select f_settings from h_halls where f_id=:f_id");
    if (db.nextRow()) {
        settings = db.getInt(0);
    }
    if (settings == 0) {
        err = "Cannot get settings for hall";
        return false;
    }
    db[":f_settings"] = settings;
    db[":f_key"] = param_item_code_for_hotel;
    db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
    if (db.nextRow()) {
        item = db.getString(0).toInt();
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

bool C5WaiterOrderDoc::makeOutputOfStore(C5Database &db, QString &err, int storedocstate)
{
    //Remove old docs
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select distinct(ad.f_document) from o_goods g "
            "left join a_store_draft ad on ad.f_id=g.f_storerec "
            "where g.f_header=:f_id");
    while (db.nextRow()) {
        C5StoreDoc::removeDoc(db.dbParams(), db.getString(0), false);
    }

    //Check for store doc
    db[":f_header"] = fHeader["f_id"].toString();
    db.exec("select h.f_state, d.f_id, d.f_qty as f_dqty, g.f_qty as f_gqty, g.f_lastinputprice as f_gprice from o_goods g "
             "inner join a_store_draft d on d.f_id=g.f_storerec "
             "inner join a_header h on h.f_id=d.f_document "
             "where g.f_header=:f_header");

    QList<tmpg> goods;
    while (db.nextRow()) {
        if (db.getInt("f_state") == DOC_STATE_SAVED) {
            db.rollback();
            err += tr("Document saved") + "<br>";
            return false;
        }
        goods.append(tmpg(db.getString("f_id"), db.getDouble("f_dqty"), db.getDouble("f_gqty"), db.getDouble("f_gprice")));
    }
    foreach (const tmpg &t, goods) {
        db[":f_id"] = t.recId;
        db[":f_qty"] = t.qtyGoods;
        db.exec("update a_store_draft set f_qty=f_qty-:f_qty where f_id=:f_id");
    }
    db[":f_header"] = fHeader["f_id"].toString();
    db.exec("delete from o_goods where f_header=:f_header");
    db[":f_header"] = fHeader["f_id"].toString();
    db[":f_state1"] = DISH_STATE_OK;
    db[":f_state2"] = DISH_STATE_VOID;
    db.exec("select b.f_id, r.f_goods, r.f_qty*b.f_qty1 as f_totalqty, r.f_qty, "
             "g.f_lastinputprice, b.f_store "
             "from o_body b "
             "inner join d_recipes r on r.f_dish=b.f_dish "
             "inner join c_goods g on g.f_id=r.f_goods "
             "where b.f_header=:f_header and (b.f_state=:f_state1 or b.f_state=:f_state2) ");
    goods.clear();
    while (db.nextRow()) {
        tmpg t;
        t.recId = db.getString("f_id");
        t.goodsId = db.getInt("f_goods");
        t.qtyGoods = db.getDouble("f_totalqty");
        t.store = db.getInt("f_store");
        t.price = db.getDouble("f_lastinputprice");
        goods.append(t);
    }
    int row = 1;
    foreach (const tmpg &t, goods) {
        db[":f_id"] = db.uuid();
        db[":f_header"] = fHeader["f_id"].toString();
        db[":f_body"] = t.recId;
        db[":f_goods"] = t.goodsId;
        db[":f_qty"] = t.qtyGoods;
        db[":f_store"] = t.store;
        db[":f_price"] = t.price;
        db[":f_total"] = t.price * t.qtyGoods;
        db[":f_row"] = row++;
        db[":f_tax"] = 0;
        db[":f_taxdept"] = "";
        db.insert("o_goods", false);
    }
    C5StoreDraftWriter dw(db);
    if (!dw.writeFromShopOutput(fHeader["f_id"].toString(), storedocstate, err)) {
        err += dw.fErrorMsg + "<br>";
        return false;
    }
    return true;
}

bool C5WaiterOrderDoc::clearStoreOutput(C5Database &db, const QDate &d1, const QDate &d2)
{
    db[":f_date1"] = d1;
    db[":f_date2"] = d2;
    db.exec("select oh.f_id, ah.f_id, ad.f_id, ast.f_id,og.f_id "
            "from a_header_store ah "
            "left join a_header a on a.f_id=ah.f_id "
            "left join a_store_draft ad on ad.f_document=ah.f_id "
            "left join o_goods og on og.f_storerec=ad.f_id "
            "left join o_body ob on ob.f_id=og.f_body "
            "left join a_store ast on ast.f_document=ah.f_id "
            "left join o_header oh on oh.f_id=ob.f_header "
            " where ah.f_baseonsale=1 and a.f_date between :f_date1 and :f_date2 ");
    QSet<QString> oh;
    QSet<QString> ah;
    QSet<QString> as;
    QSet<QString> ad;
    QSet<QString> og;
    while (db.nextRow()) {
        oh.insert(db.getString(0));
        ah.insert(db.getString(1));
        ad.insert(db.getString(2));
        as.insert(db.getString(3));
        og.insert(db.getString(4));
    }
    for (const QString &id: as) {
        db[":f_id"] = id;
        db.exec("delete from a_store where f_id=:f_id");
    }
    for (const QString &id: ad) {
        db[":f_id"] = id;
        db.exec("delete from a_store_draft where f_id=:f_id");
    }
    for (const QString &id: og) {
        db[":f_id"] = id;
        db.exec("update o_goods set f_storerec=null where f_id=:f_id");
    }
    for (const QString &id: ah) {
        db[":f_id"] = id;
        db.exec("delete from a_header where f_id=:f_id");
        db[":f_id"] = id;
        db.exec("delete from a_header_store where f_id=:f_id");
    }
    return true;
}

void C5WaiterOrderDoc::removeDocument(C5Database &db, const QString &id)
{
    db[":f_state"] = DISH_STATE_MISTAKE;
    db[":f_state_normal"] = DISH_STATE_OK;
    db[":f_header"] = id;
    db.exec("update o_body set f_state=:f_state where f_header=:f_header and f_state=:f_state_normal");
    db[":f_id"] = id;
    db[":f_state"] = ORDER_STATE_VOID;
    db.exec("update o_header set f_state=:f_state where f_id=:f_id");
    db.deleteFromTable("b_clients_debts", "f_order", id);
    db.deleteFromTable("b_history", "f_id", id);
    db[":f_oheader"] = id;
    db.exec("select * from a_header_cash where f_oheader=:f_oheader");
    QString cashid;
    if (db.nextRow()) {
        cashid = db.getString("f_id");
        db.deleteFromTable("e_cash", "f_header", cashid);
        db.deleteFromTable("a_header_cash", cashid);
        db.deleteFromTable("a_header", cashid);
    }
    db[":f_header"] = id;
    db.exec("select ah.f_id as f_header, og.f_id as f_goods "
            "from a_header ah "
            "inner join a_store_draft ad on ad.f_document=ah.f_id "
            "inner join o_goods og on og.f_storerec=ad.f_id "
            "inner join o_body ob on ob.f_id=og.f_body "
            "where ob.f_header=:f_header");
    QSet<QString> aheader;
    QSet<QString> ogoods;
    while (db.nextRow()) {
        aheader.insert(db.getString("f_header"));
        ogoods.insert(db.getString("f_goods"));
    }
    foreach (const QString &s, ogoods) {
        db.deleteFromTable("o_goods", s);
    }
    foreach (const QString &s, aheader) {
        db.deleteFromTable("a_store", "f_document", s);
        db.deleteFromTable("a_store_draft", "f_document", s);
        db.deleteFromTable("a_header_store", s);
        db.deleteFromTable("a_header", s);
    }
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
    return str_float(fItems[index].toObject()[name].toString());
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

void C5WaiterOrderDoc::open(C5Database &db)
{
//    db[":f_id"] = hString("f_id");
//    db.exec("select h.f_name as f_hallname, t.f_name as f_tableName, concat(s.f_last, ' ', s.f_first) as f_staffname, \
//        o.*, oo.f_guests, oo.f_splitted \
//        from o_header o \
//        left join h_tables t on t.f_id=o.f_table \
//        left join h_halls h on h.f_id=t.f_hall \
//        left join s_user s on s.f_id=o.f_staff \
//        left join o_header_options oo on oo.f_id=o.f_id \
//        where o.f_id=:f_id \
//        order by o.f_id ");
//    if (db.nextRow()) {
//        for (int i = 0, count = db.columnCount(); i < count; i++) {
//            QVariant v = db.getValue(i);
//            switch (v.type()) {
//            case QVariant::Date:
//                fHeader[db.columnName(i)] = db.getDate(i).toString(FORMAT_DATE_TO_STR);
//                break;
//            case QVariant::DateTime:

//                fHeader[db.columnName(i)] = db.getDateTime(i).toString(FORMAT_DATETIME_TO_STR);
//                break;
//            default:
//                fHeader[db.columnName(i)] = db.getString(i);
//                break;
//            }
//        }
//    }
//    db[":f_order"] = fHeader["f_id"].toString();
//    db.exec("select f_car from b_car_orders where f_order=:f_order");
//    if (db.nextRow()) {
//        fHeader["car"] = db.getString(0);
//    }
//    getTaxInfo(db);
    db[":f_header"] = fHeader["f_id"].toString();
    db.exec("select ob.f_id, ob.f_header, ob.f_state, dp1.f_name as part1, dp2.f_name as part2, ob.f_adgcode, d.f_name as f_name, \
             ob.f_qty1, ob.f_qty2, ob.f_price, ob.f_service, ob.f_discount, ob.f_total, \
             ob.f_store, ob.f_print1, ob.f_print2, ob.f_comment, ob.f_remind, ob.f_dish, \
             s.f_name as f_storename, ob.f_removereason, ob.f_timeorder, ob.f_package, d.f_hourlypayment, \
             ob.f_canservice, ob.f_candiscount, ob.f_guest \
             from o_body ob \
             left join d_dish d on d.f_id=ob.f_dish \
             left join d_part2 dp2 on dp2.f_id=d.f_part \
             left join d_part1 dp1 on dp1.f_id=dp2.f_part \
             left join c_storages s on s.f_id=ob.f_store \
             where ob.f_header=:f_header");
    while (db.nextRow()) {
        QJsonObject o;
        for (int i = 0; i < db.columnCount(); i++) {
            o[db.columnName(i)] = db.getString(i);
        }
        fItems.append(o);
    }
    // Discount
    QJsonArray jda;
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select c.f_id, c.f_value, c.f_mode, cn.f_name, p.f_contact "
                            "from b_history h "
                            "left join b_cards_discount c on c.f_id=h.f_card "
                            "left join c_partners p on p.f_id=c.f_client "
                            "left join b_card_types cn on cn.f_id=c.f_mode "
                            "where h.f_id=:f_id");
    if (db.nextRow()) {
        fHeader["f_bonusid"] = db.getString("f_id");
        fHeader["f_bonustype"] = db.getString("f_mode");
        fHeader["f_bonusname"] = db.getString("f_name");
        fHeader["f_bonusvalue"] = db.getString("f_value");
        fHeader["f_bonusholder"] = db.getString("f_contact");
        fHeader["f_discountfactor"] = QString::number(db.getDouble("f_value") / 100, 'f', 3);
    }
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select * from o_pay_room where f_id=:f_id");
    if (db.nextRow()) {
        fHeader["f_other_res"] = db.getString("f_res");
        fHeader["f_other_room"] = db.getString("f_room");
        fHeader["f_other_guest"] = db.getString("f_guest");
        fHeader["f_other_inv"] = db.getString("f_inv");
    }
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select * from o_pay_cl where f_id=:f_id");
    if (db.nextRow()) {
        fHeader["f_other_clcode"] = db.getString("f_code");
        fHeader["f_other_clname"] = db.getString("f_name");
    }
}

void C5WaiterOrderDoc::getTaxInfo(C5Database &db)
{
    db[":f_id"] = hString("f_id");
    db.exec("select * from o_tax where f_id=:f_id");
    if (db.nextRow()) {
        for (int i = 0; i < db.columnCount(); i++) {
            fTax[db.columnName(i)] = db.getString(i);
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

void C5WaiterOrderDoc::calculateSelfCost(C5Database &db)
{
    /* this method used for assigning price from selfcost in metropol */
    QMap<int, QMap<int, double> > goodsQty;
    QMap<int, double> price;
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select b.f_dish, r.f_goods, r.f_qty "
            "from d_recipes r "
            "left join o_body b on b.f_dish=r.f_dish "
            "left join o_header h on h.f_id=b.f_header "
            "where b.f_state=1 and h.f_id=:f_id ");
    while (db.nextRow()) {
        goodsQty[db.getInt(0)][db.getInt(1)] = db.getDouble(2);
    }
    db.exec("select s.f_goods, sum(s.f_total)/sum(f_qty) "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_state=1 and h.f_type=1 and s.f_type=1 "
            "group by 1");
    while (db.nextRow()) {
        price[db.getInt(0)] = db.getDouble(1);
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
        db[":f_id"] = o["f_id"].toString();
        db[":f_price"] = selfcost;
        db[":f_total"] = selfcost * o["f_qty1"].toString().toDouble();
        db.exec("update o_body set f_price=:f_price, f_total=:f_total where f_id=:f_id");
        total += selfcost * o["f_qty1"].toString().toDouble();
        fItems[i] = o;
    }
    fHeader["f_amountother"] = QString::number(total, 'f', 2);
    fHeader["f_amounttotal"] = QString::number(total, 'f', 2);
    fHeader["f_servicefactor"] = "0";
    fHeader["f_amountservice"] = 0;
    db[":f_amountother"] = total;
    db[":f_amounttotal"] = total;
    db[":f_servicefactor"] = 0;
    db[":f_amountservice"] = 0;
    db.update("o_header", where_id(fHeader["f_id"].toString()));
}

#include "c5orderdriver.h"
#include "c5utils.h"
#include "c5cafecommon.h"
#include "datadriver.h"
#include "c5config.h"
#include "c5waiterorder.h"
#include "c5waiterorderdoc.h"
#include <QJsonObject>

C5OrderDriver::C5OrderDriver(const QStringList &dbParams) :
    fDbParams(dbParams)
{

}

bool C5OrderDriver::newOrder(int userid, QString &id, int tableId)
{
    fTable = tableId;
    C5Database db(fDbParams);
    db.startTransaction();

    fCurrentOrderId = db.uuid();
    db[":f_id"] = fCurrentOrderId;
    db.insert("o_header", false);
    db[":f_id"] = fCurrentOrderId;
    db.insert("o_header_options", false);
    db[":f_id"] = fCurrentOrderId;
    db.insert("o_tax", false);
    db[":f_id"] = fCurrentOrderId;
    db.insert("o_pay_cl", false);
    db[":f_id"] = fCurrentOrderId;
    db.insert("o_pay_room", false);
    db[":f_id"] = fCurrentOrderId;
    db.insert("o_header_flags", false);
    db[":f_id"] = fCurrentOrderId;
    db.insert("o_payment", false);
    db[":f_id"] = fCurrentOrderId;
    db.insert("o_preorder", false);
    fHeader["f_id"] = fCurrentOrderId;

    db[":f_id"] = dbtable->hall(fTable);
    db.exec("select f_id, f_prefix, f_counter + 1 as f_counter from h_halls where f_id=(select f_counterhall from h_halls where f_id=:f_id limit 1) for update");
    if (!db.nextRow()) {
        db.rollback();
        fLastError = QString("NO COUNTER FOR THIS HALL (%1)").arg(dbtable->hall(fTable));
        return false;
    }
    int hallid = db.getInt("f_counter");
    QString prefix = db.getString("f_prefix");
    db[":f_counter"] = hallid;
    db.update("h_halls", "f_id", db.getInt("f_id"));
    db.commit();

    setHeader("f_staff", userid);
    setHeader("f_table", fTable);
    setHeader("f_prefix", prefix);
    setHeader("f_hallid", hallid);
    setHeader("f_dateopen", QDate::currentDate());
    setHeader("f_timeopen", QTime::currentTime());
    setHeader("f_currentstaff", userid);
    setHeader("f_state", ORDER_STATE_OPEN);
    setHeader("f_precheck", 0);
    setHeader("f_print", 0);
    setHeader("f_guests", 1);
    setHeader("f_comment", "");
    setHeader("f_hall", dbtable->hall(fTable));
    setHeader("f_amounttotal", 0);
    setHeader("f_amountcash", 0);
    setHeader("f_amountcard", 0);
    setHeader("f_amountbank", 0);
    setHeader("f_amountother", 0);
    setHeader("f_amountservice", 0);
    setHeader("f_amountdiscount", 0);
    setHeader("f_servicefactor", __c5config.serviceFactor());
    setHeader("f_discountfactor", 0);
    if (!save()) {
        return false;
    }
    id = fCurrentOrderId;
    loadData(fCurrentOrderId);
    return true;
}

bool C5OrderDriver::closeOrder()
{
    setCloseHeader();
    if (!save()) {
        return false;
    }
    if (headerValue("f_state").toInt() == ORDER_STATE_CLOSE) {
        QString err;
        C5Database db(__c5config.dbParams());
        C5WaiterOrderDoc doc(currentOrderId(), db);
        doc.makeOutputOfStore(db, err, DOC_STATE_SAVED);
        if (__c5config.hotelDatabase().length() > 0) {
            if (!doc.transferToHotel(db, err)) {
                fLastError = err;
                return false;
            }
        }
    }
    clearOrder();
    return true;
}

bool C5OrderDriver::loadData(const QString id)
{
    clearOrder();

    if (id.isEmpty()) {
        fLastError = tr("Empty order id");
        return false;
    }

    fCurrentOrderId = id;
    if (!fetchTableData("select * from o_header where f_id='" + id + "' ", fHeader)) {
        return false;
    }
    if (!fetchTableData("select * from o_header_options where f_id='" + id + "'", fHeaderOptions)) {
        return false;
    }

    if (!fetchTableData("select * from o_tax where f_id='" + id + "'", fTaxInfo)) {
        return false;
    }

    if (!fetchTableData("select * from o_pay_room where f_id='" + id + "'", fPayRoom)) {
        return false;
    }

    if (!fetchTableData("select * from o_pay_cl where f_id='" + id + "'", fPayCL)) {
        return false;
    }

    if (!fetchDishesData(id, "")) {
        return false;
    }

    fTable = headerValue("f_table").toInt();
    return true;
}

bool C5OrderDriver::reloadOrder()
{
    return loadData(fCurrentOrderId);
}

bool C5OrderDriver::setRoom(const QString &res, const QString &inv, const QString &room, const QString &guest)
{
    //TODO: insert with new order
    //TODO: update insternal data
    C5Database db(fDbParams);
    db[":f_res"] = res;
    db[":f_inv"] = inv;
    db[":f_room"] = room;
    db[":f_guest"] = guest;
    if (!db.update("o_pay_room", "f_id", fCurrentOrderId)) {
        fLastError = db.fLastError;
        return false;
    }
    fPayRoom["f_guest"] = guest;
    fPayRoom["f_res"] = res;
    fPayRoom["f_inv"] = inv;
    fPayRoom["f_room"] = room;
    setHeader("f_otherid", PAYOTHER_TRANSFER_TO_ROOM);
    return true;
}

bool C5OrderDriver::setCL(const QString &code, const QString &name)
{
    //TODO: insert with new order
    //TODO: update insternal data
    C5Database db(fDbParams);
    db[":f_code"] = code;
    db[":f_name"] = name;
    if (!db.update("o_pay_cl", "f_id", fCurrentOrderId)) {
        fLastError = db.fLastError;
        return false;
    }
    fPayCL["f_code"] = code;
    fPayCL["f_name"] = name;
    return true;
}

bool C5OrderDriver::save()
{
    C5Database db(fDbParams);
    db.startTransaction();
    if (headerValue("f_state").toInt() == ORDER_STATE_EMPTY) {
        for (int i = 0; i < fDishes.count(); i++) {
            if (dishesValue("f_state", i).toInt() == DISH_STATE_OK) {
                setHeader("f_state", ORDER_STATE_OPEN);
                break;
            }
        }
    }
    for (const QString &t: fTableData.keys()) {
        for (QMap<QString, QVariant>::const_iterator it = fTableData[t].begin(); it != fTableData[t].end(); it++) {
            db[":" + it.key()] = it.value();
        }
        if (!db.update(t, "f_id", fCurrentOrderId)) {
            fLastError = db.fLastError;
            db.rollback();
            return false;
        }
    }
    for (const QString &d: fDishesTableData.keys()) {
        for (QMap<QString, QVariant>::const_iterator it = fDishesTableData[d].begin(); it != fDishesTableData[d].end(); it++) {
            db[":" + it.key()] = it.value();
        }
        if (!db.update("o_body", "f_id", d)) {
            fLastError = db.fLastError;
            db.rollback();
            return false;
        }
    }
    db.commit();
    fTableData.clear();
    fDishesTableData.clear();
    return true;
}

QString C5OrderDriver::error() const
{
    return fLastError;
}

int C5OrderDriver::ordersCount()
{
    return fHeader.count();
}

int C5OrderDriver::dishesCount()
{
    return fDishes.count();
}

bool C5OrderDriver::isEmpty()
{
    if (fHeader.count() == 0) {
        return true;
    }
    if (headerValue("f_state").toInt() == ORDER_STATE_PREORDER_1 ||
            headerValue("f_state").toInt() == ORDER_STATE_PREORDER_2) {
        return false;
    }
    for (int i = 0; i < fDishes.count(); i++) {
        if (dishesValue("f_state", i).toInt() == DISH_STATE_OK) {
            return false;
        }
    }
    return true;
}

double C5OrderDriver::amountTotal()
{
    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;
    for (int i = 0, count = dishesCount(); i < count; i++) {
        setDishesValue("f_discount", dishesValue("f_candiscount", i) == 0 ? 0 : headerValue("f_discountfactor"), i);
        setDishesValue("f_service",  dishesValue("f_canservice", i) == 0 ? 0 : headerValue("f_servicefactor"), i);
        if (dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
            continue;
        }
        if (dbdish->isHourlyPayment(dishesValue("f_dish", i).toInt())) {
            total += dishesValue("f_total", i).toDouble();
            continue;
        }
        double price = dishesValue("f_price", i).toDouble();
        double service = dishesValue("f_service", i).toDouble();
        double discount = dishesValue("f_discount", i).toDouble();
        double itemTotal = price * dishesValue("f_qty2", i).toDouble();
        if (dishesValue("f_canservice", i).toInt() > 0) {
            double serviceAmount = (itemTotal * service);
            itemTotal += serviceAmount;
            totalService += serviceAmount;
        }
        if (dishesValue("f_candiscount", i).toInt() > 0) {
            double discountAmount = (itemTotal * discount);
            itemTotal -= discountAmount;
            totalDiscount += discountAmount;
        }
        setDishesValue("f_total", price * dishesValue("f_qty2", i).toDouble(), i);
        total += itemTotal;
    }
    setHeader("f_amounttotal", total);
    setHeader("f_amountservice", totalService);
    setHeader("f_amountdiscount", totalDiscount);
    double acash = headerValue("f_amountcash").toDouble(),
            acard = headerValue("f_amountcard").toDouble(),
            abank = headerValue("f_amountbank").toDouble(),
            aother = headerValue("f_amountother").toDouble();
    double adiff = total - (acash + acard + abank + aother);
    if (adiff < 0.001) {
        setHeader("f_amountother", 0);
        setHeader("f_amountbank", 0);
        setHeader("f_amountcard", 0);
        setHeader("f_amountcash", headerValue("f_amounttotal"));
    }
    return total;
}

double C5OrderDriver::prepayment()
{
    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;
    for (int i = 0, count = dishesCount(); i < count; i++) {
        if (dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
            continue;
        }
        if (dishesValue("f_hourlypayment", i) > 0) {
            total += dishesValue("f_total", i).toDouble();
            continue;
        }
        double price = dishesValue("f_price", i).toDouble();
        double service = dishesValue("f_service", i).toDouble();
        double discount = dishesValue("f_discount", i).toDouble();
        double itemTotal = price * dishesValue("f_qty1", i).toDouble();
        if (dishesValue("f_canservice", i).toInt() > 0) {
            double serviceAmount = (itemTotal * service);
            itemTotal += serviceAmount;
            totalService += serviceAmount;
        }
        if (dishesValue("f_candiscount", i).toInt() > 0) {
            double discountAmount = (itemTotal * discount);
            itemTotal -= discountAmount;
            totalDiscount += discountAmount;
        }
        total += itemTotal;
    }
    return total;
}

C5OrderDriver &C5OrderDriver::setCurrentOrderID(const QString &id)
{
    fCurrentOrderId = id;
    loadData(id);
    return *this;
}

QString C5OrderDriver::currentOrderId()
{
    return fCurrentOrderId;
}

C5OrderDriver &C5OrderDriver::setHeader(const QString &key, const QVariant &value)
{
    //Q_ASSERT(fHeader.contains(key));
    fHeader[key] = value;
    fTableData["o_header"][key] = value;
    return *this;
}

QVariant C5OrderDriver::headerValue(const QString &key)
{
    Q_ASSERT(fHeader.contains(key));
    return fHeader[key];
}

QVariant C5OrderDriver::taxValue(const QString &key)
{
    Q_ASSERT(fTaxInfo.contains(key));
    return fTaxInfo[key];
}

C5OrderDriver &C5OrderDriver::setHeaderOption(const QString &key, const QVariant &value)
{
    fHeaderOptions[key] = value;
    fTableData["o_header_options"][key] = value;
    return *this;
}

QVariant C5OrderDriver::headerOptionsValue(const QString &key)
{
    return fHeaderOptions[key];
}

C5OrderDriver &C5OrderDriver::setPreorder(const QString &key, const QVariant &value)
{
    fHeaderPreorder[key] = value;
    fTableData["o_preorder"][key] = value;
    return *this;
}

QVariant C5OrderDriver::preorder(const QString &key)
{
    Q_ASSERT(fHeaderPreorder.contains(key));
    return fHeaderPreorder[key];
}

QVariant C5OrderDriver::payRoomValue(const QString &key)
{
    //TODO: UINSERT VALUE IF NEW ORDER
    Q_ASSERT(fPayRoom.contains(key));
    return fPayRoom[key];
}

QVariant C5OrderDriver::clValue(const QString &key)
{
    //TODO: UINSERT VALUE IF NEW ORDER
    Q_ASSERT(fPayCL.contains(key));
    return fPayCL[key];
}

const QMap<QString, QVariant> &C5OrderDriver::dish(int index) const
{
    return fDishes.at(index);
}

bool C5OrderDriver::addDish(int menuid, const QString &comment, double price)
{
    if (fCurrentOrderId.isEmpty()) {
        save();
    }
    QString id = C5Database::uuid();
    C5Database db(fDbParams);
    db[":f_id"] = id;
    db[":f_header"] = fCurrentOrderId;
    db[":f_state"] = DISH_STATE_OK;
    db[":f_dish"] = dbmenu->dishid(menuid);
    db[":f_qty1"] = 1;
    db[":f_qty2"] = 0;
    db[":f_price"] = price < 0.01 ? dbmenu->price(menuid) : price;
    db[":f_service"] = dbdish->canService(dbmenu->dishid(menuid)) ? headerValue("f_servicefactor").toDouble() : 0;
    db[":f_discount"] = dbdish->canDiscount(dbmenu->dishid(menuid)) ? headerValue("f_discountfactor").toDouble() : 0;
    db[":f_total"] = 0;
    db[":f_store"] = dbmenu->store(menuid);
    db[":f_print1"] = dbmenu->print1(menuid);
    db[":f_print2"] = dbmenu->print2(menuid);
    db[":f_comment"] = "";
    db[":f_comment2"] = comment;
    db[":f_remind"] = 0;
    db[":f_adgcode"] = dbdishpart2->adgcode(dbdish->group(dbmenu->dishid(menuid)));
    db[":f_removereason"] = "";
    db[":f_timeorder"] = 1;
    db[":f_package"] = 0;
    db[":f_candiscount"] = dbdish->canDiscount(dbmenu->dishid(menuid));
    db[":f_canservice"] = dbdish->canService(dbmenu->dishid(menuid));
    db[":f_guest"] = 1;
    db[":f_row"] = dishesCount();
    db[":f_appendtime"] = QDateTime::currentDateTime();
    if (!db.insert("o_body", false)) {
        fLastError = db.fLastError;
        return false;
    }
    return fetchDishesData("", id);
}

bool C5OrderDriver::addDish(QMap<QString, QVariant> o)
{
    if (fCurrentOrderId.isEmpty()) {
        if (!save()) {
            return false;
        }
    }
    C5Database db(fDbParams);
    if (o["f_id"].toString().isEmpty()) {
        o["f_id"] = db.uuid();
        for (QMap<QString, QVariant>::const_iterator it = o.begin(); it != o.end(); it++) {
            db[":" + it.key()] = it.value();
        }
        if (!db.insert("o_body", false)) {
            fLastError = db.fLastError;
            return false;
        }
    }
    fDishes.append(o);
    setDishesValue("f_header", fCurrentOrderId, fDishes.count() - 1);
    return true;
}

void C5OrderDriver::removeDish(int index)
{
    fDishes.removeAt(index);
}

C5OrderDriver &C5OrderDriver::setDishesValue(const QString &key, const QVariant &value, int index)
{
    fDishes[index][key] = value;
    fDishesTableData[fDishes[index]["f_id"].toString()][key] = value;
    return *this;
}

QVariant C5OrderDriver::dishesValue(const QString &key, int index)
{
    Q_ASSERT(fDishes[index].contains(key));
    return fDishes[index][key];
}

double C5OrderDriver::dishTotal(int index)
{
    double price = dishesValue("f_price", index).toDouble();
    double service = dishesValue("f_service", index).toDouble();
    double discount = dishesValue("f_discount", index).toDouble();
    QString qtyfield = headerValue("f_state").toInt() == ORDER_STATE_OPEN ? "f_qty2" : "f_qty1";
    double itemTotal = price * dishesValue(qtyfield, index).toDouble();
    if (dbdish->isHourlyPayment(dishesValue("f_dish", index).toInt())) {
        return dishesValue("f_total", index).toDouble();
    }

    if (dishesValue("f_canservice", index).toInt() > 0) {
        double serviceAmount = (itemTotal * service);
        itemTotal += serviceAmount;
    }
    if (dishesValue("f_candiscount", index).toInt() > 0) {
        double discountAmount = (itemTotal * discount);
        itemTotal -= discountAmount;
    }
    return itemTotal;
}

double C5OrderDriver::dishTotal2(int index)
{
    double price = dishesValue("f_price", index).toDouble();
    QString qtyfield = headerValue("f_state").toInt() == ORDER_STATE_OPEN ? "f_qty2" : "f_qty1";
    double itemTotal = price * dishesValue(qtyfield, index).toDouble();
    if (dbdish->isHourlyPayment(dishesValue("f_dish", index).toInt())) {
        return dishesValue("f_total", index).toDouble();
    }
    return itemTotal;
}

int C5OrderDriver::duplicateDish(int index)
{
    QMap<QString, QVariant> o = fDishes[index];

    if (fCurrentOrderId.isEmpty()) {
        if (!save()) {
            return -1;
        }
    }
    QString id = C5Database::uuid();
    C5Database db(fDbParams);
    db[":f_id"] = id;
    db[":f_header"] = fCurrentOrderId;
    db[":f_state"] = o["f_state"].toInt();
    db[":f_dish"] = o["f_dish"].toInt();
    db[":f_qty1"] = o["f_qty1"].toDouble();
    db[":f_qty2"] = o["f_qty2"].toDouble();
    db[":f_price"] = o["f_price"].toDouble();
    db[":f_service"] = o["f_service"].toDouble();
    db[":f_discount"] = o["f_discount"].toDouble();
    db[":f_total"] = o["f_total"].toDouble();
    db[":f_store"] = o["f_store"].toInt();
    db[":f_print1"] = o["f_print1"].toString();
    db[":f_print2"] = o["f_print2"].toString();
    db[":f_comment"] = o["f_comment"].toString();
    db[":f_remind"] = o["f_remind"].toInt();
    db[":f_adgcode"] = o["f_adgcode"].toString();
    db[":f_removereason"] = o["f_removereason"].toString();
    db[":f_timeorder"] = o["f_timeorder"].toInt();
    db[":f_package"] = o["f_package"].toInt();
    db[":f_candiscount"] = o["f_candiscount"].toInt();
    db[":f_canservice"] = o["f_canservice"].toInt();
    db[":f_guest"] = o["f_guest"].toInt();
    db[":f_row"] = fDishes.count();
    if (!db.insert("o_body", false)) {
        fLastError = db.fLastError;
        return -1;
    }
    if (!fetchDishesData("", id)){
        return -1;
    }
    return fDishes.count() - 1;
}

bool C5OrderDriver::fetchTableData(const QString &sql, QMap<QString, QVariant> &data)
{
    C5Database db(fDbParams);
    if (!db.exec(sql)) {
        fLastError = db.fLastError;
        return false;
    }
    if (db.nextRow()) {
        db.rowToMap(data);
    }
    return true;
}

bool C5OrderDriver::fetchDishesData(const QString &header, const QString &id)
{
    C5Database db(fDbParams);
    QString sql = "select ob.* from o_body ob where ";
    if (id.isEmpty()) {
        sql += "ob.f_header in ('" + header + "')";
    } else {
        sql += " ob.f_id='" + id + "' ";
    }
    sql += " order by ob.f_row ";
    if(!db.exec(sql)) {
        fLastError = db.fLastError;
        return false;
    }
    while (db.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < db.columnCount(); i++) {
            row[db.columnName(i)] = db.getValue(i);
        }
        fDishes.append(row);
    }
    return true;
}

void C5OrderDriver::setCloseHeader()
{
    int state = ORDER_STATE_CLOSE;
    if (isEmpty()) {
        if (headerValue("f_state").toInt() != ORDER_STATE_MOVED) {
            state = ORDER_STATE_EMPTY;
        } else {
            state = ORDER_STATE_MOVED;
        }
    } else {
        if (headerValue("f_state").toInt() == ORDER_STATE_MOVED) {
            state = ORDER_STATE_MOVED;
        }
    }
    QDate datecash;
    int dateshift;
    dateCash(datecash, dateshift);
    setHeader("f_state", state);
    setHeader("f_dateclose", QDate::currentDate());
    setHeader("f_timeclose", QTime::currentTime());
    setHeader("f_datecash", datecash);
    setHeader("f_shift", dateshift);
}

void C5OrderDriver::clearOrder()
{
    fHeader.clear();
    fHeaderOptions.clear();
    fHeaderPreorder.clear();
    fDishes.clear();
    fTaxInfo.clear();
    fPayCL.clear();
    fPayRoom.clear();
    fCurrentOrderId.clear();
}

void C5OrderDriver::dateCash(QDate &d, int &dateShift)
{
    d = QDate::currentDate();
    dateShift = 1;
    if (__c5config.getValue(param_date_cash_auto).toInt() == 1) {
       QTime t = QTime::fromString(__c5config.getValue(param_working_date_change_time), "HH:mm:ss");
       if (t.isValid()) {
           if (QTime::currentTime() < t) {
               d = d.addDays(-1);
               dateShift = 2;
           }
       }
    } else {
        d = QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL);
        dateShift = __c5config.dateShift();
    }
}
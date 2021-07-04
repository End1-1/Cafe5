#include "c5orderdriver.h"
#include "c5utils.h"
#include "c5cafecommon.h"
#include "datadriver.h"
#include "c5config.h"
#include <QJsonObject>

C5OrderDriver::C5OrderDriver(const QStringList &dbParams) :
    fDb(dbParams)
{
}

bool C5OrderDriver::newOrder(int userid, QString &id, int tableId)
{
    fTable = tableId;
    fCurrentOrderId = "";
    clearOrder();
    setHeader("f_staff", userid);
    setHeader("f_table", fTable);
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
    return true;
}

bool C5OrderDriver::closeOrder()
{
    QDate dateCash = QDate::currentDate();
    int dateShift = 1;
    if (__c5config.getValue(param_date_cash_auto).toInt() == 1) {
       QTime t = QTime::fromString(__c5config.getValue(param_working_date_change_time), "HH:mm:ss");
       if (t.isValid()) {
           if (QTime::currentTime() < t) {
               dateCash = dateCash.addDays(-1);
               dateShift = 2;
           }
       }
    } else {
        dateCash = QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR);
        dateShift = __c5config.dateShift();
    }


    int state = ORDER_STATE_CLOSE;
    if (isEmpty()) {
        if (headerValue("f_state").toInt() != ORDER_STATE_MOVED) {
            state = ORDER_STATE_EMPTY;
        }
    }
    setHeader("f_state", state);
    setHeader("f_dateclose", QDate::currentDate());
    setHeader("f_timeclose", QTime::currentTime());
    setHeader("f_datecash", dateCash);
    setHeader("f_shift", dateShift);
    if (!save()) {
        return false;
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
    if (!fetchTableData("select * from o_header where f_id in ('" + id + "') ", fHeader)) {
        return false;
    }
    if (!fetchTableData("select * from o_header_options where f_id in ('" + id + "')", fHeaderOptions)) {
        return false;
    }

    if (!fetchTableData("select * from o_tax where f_id in ('" + id + "')", fTaxInfo)) {
        return false;
    }

    if (!fetchTableData("select * from o_pay_room where f_id in ('" + id + "')", fPayRoom)) {
        return false;
    }

    if (!fetchTableData("select * from o_pay_cl where f_id in ('" + id + "')", fPayCL)) {
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
    fDb[":f_res"] = res;
    fDb[":f_inv"] = inv;
    fDb[":f_room"] = room;
    fDb[":f_guest"] = guest;
    if (!fDb.update("o_pay_room", "f_id", fCurrentOrderId)) {
        fLastError = fDb.fLastError;
        return false;
    }
    fPayRoom["f_guest"] = guest;
    fPayRoom["f_res"] = res;
    fPayRoom["f_inv"] = inv;
    fPayRoom["f_room"] = room;
    return true;
}

bool C5OrderDriver::setCL(const QString &code, const QString &name)
{
    //TODO: insert with new order
    //TODO: update insternal data
    fDb[":f_code"] = code;
    fDb[":f_name"] = name;
    if (!fDb.update("o_pay_cl", "f_id", fCurrentOrderId)) {
        fLastError = fDb.fLastError;
        return false;
    }
    fPayCL["f_code"] = code;
    fPayCL["f_name"] = name;
    return true;
}

bool C5OrderDriver::save()
{
    fDb.startTransaction();
    if (fCurrentOrderId.isEmpty()) {
        fCurrentOrderId = fDb.uuid();
        fDb[":f_id"] = fCurrentOrderId;
        fDb.insert("o_header", false);
        fDb[":f_id"] = fCurrentOrderId;
        fDb.insert("o_header_options", false);
        fDb[":f_id"] = fCurrentOrderId;
        fDb.insert("o_tax", false);
        fDb[":f_id"] = fCurrentOrderId;
        fDb.insert("o_pay_cl", false);
        fDb[":f_id"] = fCurrentOrderId;
        fDb.insert("o_pay_room", false);
        fDb[":f_id"] = fCurrentOrderId;
        fDb.insert("o_header_flags", false);
        fDb[":f_id"] = fCurrentOrderId;
        fDb.insert("o_payment", false);
        fDb[":f_id"] = fCurrentOrderId;
        fDb.insert("o_preorder", false);
        fHeader["f_id"] = fCurrentOrderId;
        loadData(fCurrentOrderId);
    }
    bool isOrderEmpty = dishesCount() == 0;
    for (int i = 0; i < dishesCount(); i++) {
        if (dishesValue("f_state", i).toInt() == DISH_STATE_OK) {
            isOrderEmpty = false;
            break;
        }
    }
    if (isOrderEmpty) {
        setHeader("f_state", ORDER_STATE_EMPTY);
    } else if (headerValue("f_state").toInt() == ORDER_STATE_EMPTY) {
        setHeader("f_state", ORDER_STATE_OPEN);
    }
    for (const QString &t: fTableData.keys()) {
        for (QMap<QString, QVariant>::const_iterator it = fTableData[t].begin(); it != fTableData[t].end(); it++) {
            fDb[":" + it.key()] = it.value();
        }
        if (!fDb.update(t, "f_id", fCurrentOrderId)) {
            fLastError = fDb.fLastError;
            fDb.rollback();
            return false;
        }
    }
    for (const QString &d: fDishesTableData.keys()) {
        for (QMap<QString, QVariant>::const_iterator it = fDishesTableData[d].begin(); it != fDishesTableData[d].end(); it++) {
            fDb[":" + it.key()] = it.value();
        }
        if (!fDb.update("o_body", "f_id", d)) {
            fLastError = fDb.fLastError;
            fDb.rollback();
            return false;
        }
    }
    fDb.commit();
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
        if (dbdish->isHourlyPayment(dishesValue("f_id", i).toInt())) {
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

bool C5OrderDriver::addDish(const QJsonObject &o)
{
    if (fCurrentOrderId.isEmpty()) {
        save();
    }
    QString id = C5Database::uuid();;
    fDb[":f_id"] = id;
    fDb[":f_header"] = fCurrentOrderId;
    fDb[":f_state"] = o["f_state"].toString().toInt();
    fDb[":f_dish"] = o["f_dish"].toString().toInt();
    fDb[":f_qty1"] = o["f_qty1"].toDouble();
    fDb[":f_qty2"] = o["f_qty2"].toDouble();
    fDb[":f_price"] = o["f_price"].toString().toDouble();
    fDb[":f_service"] = o["f_service"].toString().toDouble();
    fDb[":f_discount"] = o["f_discount"].toString().toDouble();
    fDb[":f_total"] = o["f_total"].toDouble();
    fDb[":f_store"] = o["f_store"].toString().toInt();
    fDb[":f_print1"] = o["f_print1"].toString();
    fDb[":f_print2"] = o["f_print2"].toString();
    fDb[":f_comment"] = o["f_comment"].toString();
    fDb[":f_remind"] = o["f_remind"].toString().toInt();
    fDb[":f_adgcode"] = o["f_adgcode"].toString();
    fDb[":f_removereason"] = o["f_removereason"].toString();
    fDb[":f_timeorder"] = o["f_timeorder"].toString().toInt();
    fDb[":f_package"] = o["f_package"].toString().toInt();
    fDb[":f_candiscount"] = o["f_candiscount"].toString().toInt();
    fDb[":f_canservice"] = o["f_canservice"].toString().toInt();
    fDb[":f_guest"] = o["f_guest"].toString().toInt();
    fDb[":f_row"] = o["f_row"].toInt();
    if (!fDb.insert("o_body", false)) {
        fLastError = fDb.fLastError;
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
    if (o["f_id"].toString().isEmpty()) {
        o["f_id"] = fDb.uuid();
        for (QMap<QString, QVariant>::const_iterator it = o.begin(); it != o.end(); it++) {
            fDb[":" + it.key()] = it.value();
        }
        if (!fDb.insert("o_body", false)) {
            fLastError = fDb.fLastError;
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
    if (dbdish->isHourlyPayment(dishesValue("f_id", index).toInt())) {
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

int C5OrderDriver::duplicateDish(int index)
{
    QMap<QString, QVariant> o = fDishes[index];

    if (fCurrentOrderId.isEmpty()) {
        if (!save()) {
            return -1;
        }
    }
    QString id = C5Database::uuid();;
    fDb[":f_id"] = id;
    fDb[":f_header"] = fCurrentOrderId;
    fDb[":f_state"] = o["f_state"].toInt();
    fDb[":f_dish"] = o["f_dish"].toInt();
    fDb[":f_qty1"] = o["f_qty1"].toDouble();
    fDb[":f_qty2"] = o["f_qty2"].toDouble();
    fDb[":f_price"] = o["f_price"].toDouble();
    fDb[":f_service"] = o["f_service"].toDouble();
    fDb[":f_discount"] = o["f_discount"].toDouble();
    fDb[":f_total"] = o["f_total"].toDouble();
    fDb[":f_store"] = o["f_store"].toInt();
    fDb[":f_print1"] = o["f_print1"].toString();
    fDb[":f_print2"] = o["f_print2"].toString();
    fDb[":f_comment"] = o["f_comment"].toString();
    fDb[":f_remind"] = o["f_remind"].toInt();
    fDb[":f_adgcode"] = o["f_adgcode"].toString();
    fDb[":f_removereason"] = o["f_removereason"].toString();
    fDb[":f_timeorder"] = o["f_timeorder"].toInt();
    fDb[":f_package"] = o["f_package"].toInt();
    fDb[":f_candiscount"] = o["f_candiscount"].toInt();
    fDb[":f_canservice"] = o["f_canservice"].toInt();
    fDb[":f_guest"] = o["f_guest"].toInt();
    fDb[":f_row"] = o["f_row"].toInt();
    if (!fDb.insert("o_body", false)) {
        fLastError = fDb.fLastError;
        return -1;
    }
    if (!fetchDishesData("", id)){
        return -1;
    }
    return fDishes.count() - 1;
}

bool C5OrderDriver::fetchTableData(const QString &sql, QMap<QString, QVariant> &data)
{
    if (!fDb.exec(sql)) {
        fLastError = fDb.fLastError;
        return false;
    }
    if (fDb.nextRow()) {
        fDb.rowToMap(data);
    }
    return true;
}

bool C5OrderDriver::fetchDishesData(const QString &header, const QString &id)
{
//    QString sql = "select ob.f_id, ob.f_header, ob.f_state, dp1.f_name as part1, dp2.f_name as part2, ob.f_adgcode, d.f_name as f_name, "
//             "ob.f_qty1, ob.f_qty2, ob.f_price, ob.f_service, ob.f_discount, ob.f_total, "
//             "ob.f_store, ob.f_print1, ob.f_print2, ob.f_comment, ob.f_remind, ob.f_dish, "
//             "s.f_name as f_storename, ob.f_removereason, ob.f_timeorder, ob.f_package, d.f_hourlypayment, "
//             "ob.f_canservice, ob.f_candiscount, ob.f_guest, ob.f_fromtable "
//             "from o_body ob "
//             "left join d_dish d on d.f_id=ob.f_dish "
//             "left join d_part2 dp2 on dp2.f_id=d.f_part "
//             "left join d_part1 dp1 on dp1.f_id=dp2.f_part "
//             "left join c_storages s on s.f_id=ob.f_store "
//             "where ";
    QString sql = "select ob.* from o_body ob where ";
    if (id.isEmpty()) {
        sql += "ob.f_header in ('" + header + "')";
    } else {
        sql += " ob.f_id='" + id + "' ";
    }
    sql += " order by ob.f_row ";
    if(!fDb.exec(sql)) {
        fLastError = fDb.fLastError;
        return false;
    }
    while (fDb.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < fDb.columnCount(); i++) {
            row[fDb.columnName(i)] = fDb.getValue(i);
        }
        fDishes.append(row);
    }
    return true;
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

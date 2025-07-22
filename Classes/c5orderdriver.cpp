#include "c5orderdriver.h"
#include "c5utils.h"
#include "bclientdebts.h"
#include "datadriver.h"
#include "c5config.h"
#include "cashboxconfig.h"
#include "c5message.h"
#include <QJsonObject>
#include <QDateTime>
#include <QLibrary>

typedef double (*totalOfHourly)(const QDateTime &date, QString &str);

C5OrderDriver::C5OrderDriver()
{
}

bool C5OrderDriver::closeOrder()
{
    setCloseHeader();

    if(!save()) {
        return false;
    }

    if(headerValue("f_state").toInt() == ORDER_STATE_CLOSE) {
        QString err;
        C5Database db;
        C5StoreDraftWriter dw(db);

        if(__c5config.getValue(param_waiter_automatially_storeout).toInt() > 0) {
            dw.writeStoreOfSale(currentOrderId(), err, DOC_STATE_SAVED);
        }

        if(__c5config.hotelDatabase().length() > 0) {
            if(!dw.transferToHotel(db, currentOrderId(), err)) {
                fLastError = err;
                return false;
            }
        }

        auto *cbc = Configs::construct<CashboxConfig>(2);
        QString headerPrefix;
        int headerId;
        QDate dateCash = headerValue("f_datecash").toDate();

        if(!dw.hallId(headerPrefix, headerId, headerValue("f_hall").toInt())) {
            err = dw.fErrorMsg;
        }

        if(headerValue("f_amountcash").toDouble() > 0.0001) {
            writeCashDoc(dw, headerValue("f_id").toString(), QString("%1%2").arg(headerPrefix, QString::number(headerId)), err,
                         headerValue("f_amountcash").toDouble(), headerValue("f_currentstaff").toInt(),
                         cbc->cash1, dateCash);
        }

        if(headerValue("f_amountcard").toDouble() > 0.0001) {
            writeCashDoc(dw, headerValue("f_id").toString(), QString("%1%2").arg(headerPrefix, QString::number(headerId)), err,
                         headerValue("f_amountcard").toDouble(), headerValue("f_currentstaff").toInt(),
                         cbc->cash2, dateCash);
        }

        if(headerValue("f_amountbank").toDouble() > 0.0001) {
            writeCashDoc(dw, headerValue("f_id").toString(), QString("%1%2").arg(headerPrefix, QString::number(headerId)), err,
                         headerValue("f_amountbank").toDouble(), headerValue("f_currentstaff").toInt(),
                         cbc->cash3, dateCash);
        }

        if(headerValue("f_amountprepaid").toDouble() > 0.0001) {
            writeCashDoc(dw, headerValue("f_id").toString(), QString("%1%2").arg(headerPrefix, QString::number(headerId)), err,
                         headerValue("f_amountprepaid").toDouble(), headerValue("f_currentstaff").toInt(),
                         cbc->cash4, dateCash);
        }

        if(headerValue("f_amountidram").toDouble() > 0.0001) {
            writeCashDoc(dw, headerValue("f_id").toString(), QString("%1%2").arg(headerPrefix, QString::number(headerId)), err,
                         headerValue("f_amountidram").toDouble(), headerValue("f_currentstaff").toInt(),
                         cbc->cash5, dateCash);
        }

        if(headerValue("f_amountpayx").toDouble() > 0.0001) {
            writeCashDoc(dw, headerValue("f_id").toString(), QString("%1%2").arg(headerPrefix, QString::number(headerId)), err,
                         headerValue("f_amountpayx").toDouble(), headerValue("f_currentstaff").toInt(),
                         cbc->cash6, dateCash);
        }

        if(clValue("f_code").toInt() > 0) {
            BClientDebts b;
            b.source = BCLIENTDEBTS_SOURCE_SALE;
            b.date = headerValue("f_datecash").toDate();
            b.costumer = clValue("f_code").toInt();
            b.order = headerValue("f_id").toString();
            b.amount = -1 * headerValue("f_amountother").toDouble();
            b.currency = 1;
            b.write(db, err);
        }
    }

    clearOrder();
    return true;
}

bool C5OrderDriver::setRoom(const QString &res, const QString &inv, const QString &room, const QString &guest)
{
    //TODO: insert with new order
    //TODO: update insternal data
    C5Database db;
    db[":f_res"] = res;
    db[":f_inv"] = inv;
    db[":f_room"] = room;
    db[":f_guest"] = guest;

    if(!db.update("o_pay_room", "f_id", fCurrentOrderId)) {
        fLastError = db.fLastError;
        return false;
    }

    fPayRoom["f_guest"] = guest;
    fPayRoom["f_res"] = res;
    fPayRoom["f_inv"] = inv;
    fPayRoom["f_room"] = room;

    if(!res.isEmpty()) {
        setHeader("f_otherid", PAYOTHER_TRANSFER_TO_ROOM);
    }

    return true;
}

bool C5OrderDriver::setCL(const QString &code, const QString &name)
{
    //TODO: insert with new order
    //TODO: update insternal data
    C5Database db;
    db[":f_code"] = code;
    db[":f_name"] = name;

    if(!db.update("o_pay_cl", "f_id", fCurrentOrderId)) {
        fLastError = db.fLastError;
        return false;
    }

    fPayCL["f_code"] = code;
    fPayCL["f_name"] = name;

    if(!code.isEmpty()) {
        setHeader("f_otherid", PAYOTHER_TRANSFER_TO_ROOM);
    }

    return true;
}

void C5OrderDriver::fromJson(const QJsonObject &jdoc)
{
    fHeader = jdoc["header"].toObject().toVariantMap();
    fCurrentOrderId = fHeader["f_id"].toString();
    fHeaderOptions = jdoc["headerOptions"].toObject().toVariantMap();
    fHeaderPreorder = jdoc["opreorder"].toObject().toVariantMap();
    fTaxInfo = jdoc["otax"].toObject().toVariantMap();
    fPayRoom = jdoc["oroom"].toObject().toVariantMap();
    fPayCL = jdoc["opaycl"].toObject().toVariantMap();
    fHeaderHotel = jdoc["headerHotel"].toObject().toVariantMap();
    const QJsonArray &ja  = jdoc["dishes"].toArray();
    fDishes.clear();

    for(int i = 0; i < ja.size(); i++) {
        fDishes.append(ja.at(i).toObject().toVariantMap());
    }
}

bool C5OrderDriver::save()
{
    C5Database db;
    QStringList sqls;

    if(headerValue("f_state").toInt() == ORDER_STATE_EMPTY) {
        for(int i = 0; i < fDishes.count(); i++) {
            if(dishesValue("f_state", i).toInt() == DISH_STATE_OK) {
                setHeader("f_state", ORDER_STATE_OPEN);
                break;
            }
        }
    }

    for(const QString &t : fTableData.keys()) {
        for(QMap<QString, QVariant>::const_iterator it = fTableData[t].constBegin(); it != fTableData[t].constEnd(); it++) {
            db[":" + it.key()] = it.value();
        }

        sqls.append(db.updateDry(t, "f_id", fCurrentOrderId));
    }

    for(const QString &d : fDishesTableData.keys()) {
        for(QMap<QString, QVariant>::const_iterator it = fDishesTableData[d].constBegin();
                it != fDishesTableData[d].constEnd(); it++) {
            db[":" + it.key()] = it.value();
        }

        sqls.append(db.updateDry("o_body", "f_id", d));
    }

    db.execSqlList(sqls);
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
    if(fHeader.count() == 0) {
        return true;
    }

    if(headerValue("f_state").toInt() == ORDER_STATE_VOID) {
        return true;
    }

    if(preorder("f_prepaidcash").toDouble()
            + preorder("f_prepaidcard").toDouble()
            + preorder("f_prepaidpayx").toDouble() > 0.01) {
        return false;
    }

    if(headerValue("f_state").toInt() == ORDER_STATE_PREORDER_EMPTY ||
            headerValue("f_state").toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
        return false;
    }

    for(int i = 0; i < fDishes.count(); i++) {
        if(dishesValue("f_state", i).toInt() == DISH_STATE_OK) {
            return false;
        }
    }

    return true;
}

double C5OrderDriver::amountTotal()
{
    if(headerValue("f_precheck").toInt() > 0) {
        return headerValue("f_amounttotal").toDouble();
    }

    totalOfHourly t = nullptr;
    QLibrary l("hourlypay.dll");

    if(l.load()) {
        t = reinterpret_cast<totalOfHourly>(l.resolve("total"));
    }

    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;

    for(int i = 0, count = dishesCount(); i < count; i++) {
        setDishesValue("f_discount", dishesValue("f_candiscount", i) == 0 ? 0 : headerValue("f_discountfactor"), i);
        setDishesValue("f_service",  dishesValue("f_canservice", i) == 0 ? 0 : headerValue("f_servicefactor"), i);

        if(dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
            continue;
        }

        if(dbdish->isHourlyPayment(dishesValue("f_dish", i).toInt())) {
            QString s;
            QString dt = headerValue("f_dateopen").toDate().toString("dd/MM/yyyy") + " " +
                         headerValue("f_timeopen").toTime().toString("HH:mm:ss");

            if(t) {
                setDishesValue("f_price",  t(QDateTime::fromString(dt, "dd/MM/yyyy HH:mm:ss"), s), i);
                setDishesValue("f_total",  t(QDateTime::fromString(dt, "dd/MM/yyyy HH:mm:ss"), s), i);
                setDishesValue("f_comment", s, i);
            }  else {
                C5Message::error("Could not load hourlypay.dll");
            }

            total += dishesValue("f_total", i).toDouble();
            continue;
        }

        double price = dishesValue("f_price", i).toDouble();
        double service = dishesValue("f_service", i).toDouble();
        double discount = dishesValue("f_discount", i).toDouble();
        double itemTotal = price * dishesValue("f_qty2", i).toDouble();

        if(dishesValue("f_canservice", i).toInt() > 0) {
            double serviceAmount = (itemTotal * service);
            itemTotal += serviceAmount;

            if(dishesValue("f_candiscount", i).toInt() > 0) {
                totalService += serviceAmount - (serviceAmount * discount);
            } else {
                totalService += serviceAmount;
            }
        }

        if(dishesValue("f_candiscount", i).toInt() > 0) {
            double discountAmount = (itemTotal * discount);
            itemTotal -= discountAmount;
            totalDiscount += discountAmount;
        }

        setDishesValue("f_total", price * dishesValue("f_qty2", i).toDouble(), i);
        setDishesValue("f_grandtotal", itemTotal, i);
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

    if(adiff < 0.001) {
        setHeader("f_amountother", 0);
        setHeader("f_amountbank", 0);
        setHeader("f_amountcard", 0);
        setHeader("f_amountcash", headerValue("f_amounttotal"));
    }

    return total;
}

double C5OrderDriver::clearAmount()
{
    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;

    for(int i = 0, count = dishesCount(); i < count; i++) {
        setDishesValue("f_discount", dishesValue("f_candiscount", i) == 0 ? 0 : headerValue("f_discountfactor"), i);
        setDishesValue("f_service",  dishesValue("f_canservice", i) == 0 ? 0 : headerValue("f_servicefactor"), i);

        if(dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
            continue;
        }

        if(dbdish->isHourlyPayment(dishesValue("f_dish", i).toInt())) {
            total += dishesValue("f_total", i).toDouble();
            continue;
        }

        double price = dishesValue("f_price", i).toDouble();
        double service = dishesValue("f_service", i).toDouble();
        double discount = dishesValue("f_discount", i).toDouble();
        double itemTotal = price * dishesValue("f_qty1", i).toDouble();

        if(dishesValue("f_canservice", i).toInt() > 0) {
            double serviceAmount = (itemTotal * service);
            itemTotal += serviceAmount;
            totalService += serviceAmount;
        }

        if(dishesValue("f_candiscount", i).toInt() > 0) {
            double discountAmount = (itemTotal * discount);
            itemTotal -= discountAmount;
            totalDiscount += discountAmount;
        }

        setDishesValue("f_total", price * dishesValue("f_qty1", i).toDouble(), i);
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

    if(adiff < 0.001) {
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

    for(int i = 0, count = dishesCount(); i < count; i++) {
        if(dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
            continue;
        }

        if(dishesValue("f_hourlypayment", i).toInt() > 0) {
            total += dishesValue("f_total", i).toDouble();
            continue;
        }

        double price = dishesValue("f_price", i).toDouble();
        double service = dishesValue("f_service", i).toDouble();
        double discount = dishesValue("f_discount", i).toDouble();
        double itemTotal = price * dishesValue("f_qty1", i).toDouble();

        if(dishesValue("f_canservice", i).toInt() > 0) {
            double serviceAmount = (itemTotal * service);
            itemTotal += serviceAmount;
            totalService += serviceAmount;
        }

        if(dishesValue("f_candiscount", i).toInt() > 0) {
            double discountAmount = (itemTotal * discount);
            itemTotal -= discountAmount;
            totalDiscount += discountAmount;
        }

        total += itemTotal;
    }

    return total;
}

QString C5OrderDriver::currentOrderId()
{
    return fCurrentOrderId;
}

C5OrderDriver& C5OrderDriver::setHeader(const QString &key, const QVariant &value)
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

C5OrderDriver& C5OrderDriver::setHeaderOption(const QString &key, const QVariant &value)
{
    fHeaderOptions[key] = value;
    fTableData["o_header_options"][key] = value;
    return *this;
}

QVariant C5OrderDriver::headerOptionsValue(const QString &key)
{
    Q_ASSERT(fHeaderOptions.contains(key));
    return fHeaderOptions[key];
}

C5OrderDriver& C5OrderDriver::setPreorder(const QString &key, const QVariant &value)
{
    Q_ASSERT(fHeaderPreorder.contains(key));
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

QVariant C5OrderDriver::headerHotel(const QString &key)
{
    Q_ASSERT(fHeaderHotel.contains(key));
    return fHeaderHotel[key];
}

C5OrderDriver& C5OrderDriver::setHeaderHotel(const QString &key, const QVariant &value)
{
    Q_ASSERT(fHeaderHotel.contains(key));
    fHeaderHotel[key] = value;
    fTableData["o_header_hotel"][key] = value;
    return *this;
}

const QMap<QString, QVariant>& C5OrderDriver::dish(int index) const
{
    return fDishes.at(index);
}

bool C5OrderDriver::addDish(int menuid, const QString &comment, double price, const QString &emark)
{
    if(fCurrentOrderId.isEmpty()) {
        save();
    }

    QString id = C5Database::uuid();
    C5Database db;
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
    db[":f_emarks"] = emark.isEmpty() ? QVariant() : emark;
    db[":f_appenduser"] = headerValue("f_currentstaff");
    db[":f_appendtime"] = QDateTime::currentDateTime();
    db[":f_working_day"] = __c5config.dateCash();

    if(!db.insert("o_body", false)) {
        fLastError = db.fLastError;
        return false;
    }

    db[":f_id"] = id;
    db.exec("select * from o_body where f_id=:f_id");
    db.nextRow();
    QMap<QString, QVariant> d;
    db.rowToMap(d);
    fDishes.append(d);
    return true;
}

bool C5OrderDriver::addDish2(int packageid, double qty)
{
    if(fCurrentOrderId.isEmpty()) {
        save();
    }

    QString id = C5Database::uuid();
    C5Database db;
    DbMenuPackageList p(packageid);
    db[":f_id"] = id;
    db[":f_header"] = fCurrentOrderId;
    db[":f_state"] = DISH_STATE_OK;
    db[":f_dish"] = p.dish();
    db[":f_qty1"] = p.qty() * qty;
    db[":f_qty2"] = 0;
    db[":f_price"] = p.price();
    db[":f_service"] = dbdish->canService(p.dish()) ? headerValue("f_servicefactor").toDouble() : 0;
    db[":f_discount"] = dbdish->canDiscount(p.dish()) ? headerValue("f_discountfactor").toDouble() : 0;
    db[":f_total"] = 0;
    db[":f_store"] = p.store();
    db[":f_print1"] = p.print1();
    db[":f_print2"] = p.print2();
    db[":f_comment"] = "";
    db[":f_comment2"] = "";
    db[":f_remind"] = 0;
    db[":f_adgcode"] = dbdishpart2->adgcode(dbdish->group(p.dish()));
    db[":f_removereason"] = "";
    db[":f_timeorder"] = 1;
    db[":f_package"] = 0;
    db[":f_candiscount"] = dbdish->canDiscount(p.dish());
    db[":f_canservice"] = dbdish->canService(p.dish());
    db[":f_guest"] = 1;
    db[":f_row"] = dishesCount();
    db[":f_appendtime"] = QDateTime::currentDateTime();

    if(!db.insert("o_body", false)) {
        fLastError = db.fLastError;
        return false;
    }

    db[":f_id"] = id;
    db.exec("select * from o_body where f_id=:f_id");
    db.nextRow();
    QMap<QString, QVariant> d;
    db.rowToMap(d);
    fDishes.append(d);
    return true;
}

bool C5OrderDriver::addDish(QMap<QString, QVariant> o)
{
    if(fCurrentOrderId.isEmpty()) {
        if(!save()) {
            return false;
        }
    }

    C5Database db;

    if(o["f_id"].toString().isEmpty()) {
        o["f_id"] = db.uuid();

        for(QMap<QString, QVariant>::const_iterator it = o.begin(); it != o.end(); it++) {
            db[":" + it.key()] = it.value();
        }

        if(!db.insert("o_body", false)) {
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

C5OrderDriver& C5OrderDriver::setDishesValue(const QString &key, const QVariant &value, int index)
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

    if(dbdish->isHourlyPayment(dishesValue("f_dish", index).toInt())) {
        return dishesValue("f_total", index).toDouble();
    }

    if(dishesValue("f_canservice", index).toInt() > 0) {
        double serviceAmount = (itemTotal * service);
        itemTotal += serviceAmount;
    }

    if(dishesValue("f_candiscount", index).toInt() > 0) {
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

    if(dbdish->isHourlyPayment(dishesValue("f_dish", index).toInt())) {
        return dishesValue("f_total", index).toDouble();
    }

    return itemTotal;
}

int C5OrderDriver::duplicateDish(int index)
{
    QMap<QString, QVariant> o = fDishes[index];

    if(fCurrentOrderId.isEmpty()) {
        if(!save()) {
            return -1;
        }
    }

    QString id = C5Database::uuid();
    C5Database db;
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

    if(!db.insert("o_body", false)) {
        fLastError = db.fLastError;
        return -1;
    }

    db[":f_id"] = id;
    db.exec("select * from o_body where f_id=:f_id");
    db.nextRow();
    QMap<QString, QVariant> d;
    db.rowToMap(d);
    fDishes.append(d);
    return fDishes.count() - 1;
}

void C5OrderDriver::setCloseHeader()
{
    int state = ORDER_STATE_CLOSE;

    if(isEmpty()) {
        if(headerValue("f_state").toInt() != ORDER_STATE_MOVED) {
            state = ORDER_STATE_EMPTY;
        } else {
            state = ORDER_STATE_MOVED;
        }
    } else {
        if(headerValue("f_state").toInt() == ORDER_STATE_MOVED) {
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

void C5OrderDriver::dateCash(QDate &d, int& dateShift)
{
    d = QDate::currentDate();
    dateShift = 1;

    if(__c5config.getValue(param_date_cash_auto).toInt() == 1) {
        QTime t = QTime::fromString(__c5config.getValue(param_working_date_change_time), "HH:mm:ss");

        if(t.isValid()) {
            if(QTime::currentTime() < t) {
                d = d.addDays(-1);
                dateShift = 2;
            }
        }
    } else {
        d = QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL);
        dateShift = __c5config.dateShift();
    }
}

void C5OrderDriver::writeCashDoc(C5StoreDraftWriter &dw, const QString &uuid, const QString id, QString &err,
                                 double amount, int staff, int cashboxid, QDate dateCash)
{
    QString cashdocid;

    if(!dw.writeAHeader(cashdocid, id, DOC_STATE_SAVED, DOC_TYPE_CASH,
                        staff, dateCash, QDate::currentDate(),
                        QTime::currentTime(), 0, amount,
                        tr("Revenue"), 1, 1)) {
        err = dw.fErrorMsg;
    }

    if(!dw.writeAHeaderCash(cashdocid, cashboxid,
                            0, 1, "", uuid)) {
        err = dw.fErrorMsg;
    }

    QString cashUUID;

    if(!dw.writeECash(cashUUID, cashdocid, cashboxid, 1,
                      id,
                      amount, cashUUID, 1)) {
        err = dw.fErrorMsg;
    }
}

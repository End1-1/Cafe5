#include "c5waiterorderdoc.h"
#include "c5utils.h"
#include "c5storedraftwriter.h"
#include "dict_dish_state.h"
#include <QHostInfo>

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

C5WaiterOrderDoc::C5WaiterOrderDoc(const QString &id, C5Database &db, bool openlatter)
{
    Q_UNUSED(openlatter);
    fHeader["f_id"] = id;
    fDb.setDatabase(db.dbParams());
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
    for(int i = 0, c = itemsCount(); i < c; i++) {
        if(iInt("f_state", i) == DISH_STATE_OK) {
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

    while(db.nextRow()) {
        oh.insert(db.getString(0));
        ah.insert(db.getString(1));
        ad.insert(db.getString(2));
        as.insert(db.getString(3));
        og.insert(db.getString(4));
    }

    for(const QString &id : ad) {
        db[":f_id"] = id;
        db.exec("delete from a_store_draft where f_id=:f_id");
    }

    for(const QString &id : og) {
        db[":f_id"] = id;
        db.exec("update o_goods set f_storerec=null where f_id=:f_id");
    }

    C5StoreDraftWriter dw(db);

    for(const QString &id : ah) {
        db[":f_id"] = id;
        db.exec("delete from a_header where f_id=:f_id");
        db[":f_id"] = id;
        db.exec("delete from a_header_store where f_id=:f_id");

        if(!dw.outputRollback(db, id)) {
            return false;
        }
    }

    return true;
}

void C5WaiterOrderDoc::removeDocument(C5Database &db, const QString &id)
{
    db[":f_state"] = DISH_STATE_MISTAKE;
    db[":f_state_normal"] = DISH_STATE_OK;
    db[":f_header"] = id;
    db.exec("update o_body set f_state=:f_state, f_emarks=null where f_header=:f_header and f_state=:f_state_normal");
    db[":f_id"] = id;
    db[":f_state"] = ORDER_STATE_VOID;
    db.exec("update o_header set f_state=:f_state where f_id=:f_id");
    db.deleteFromTable("b_clients_debts", "f_order", id);
    db.deleteFromTable("b_history", "f_id", id);
    db[":f_oheader"] = id;
    db.exec("select * from a_header_cash where f_oheader=:f_oheader");
    QString cashid;

    if(db.nextRow()) {
        cashid = db.getString("f_id");
        db.deleteFromTable("e_cash", "f_header", cashid);
        db.deleteFromTable("a_header_cash", cashid);
        db.deleteFromTable("a_header", cashid);
    }

    db[":f_id"] = id;
    db.exec("delete from b_history where f_id=:f_id");
    db[":f_trsale"] = id;
    db.exec("delete from b_gift_card_history where f_trsale=:f_trsale");
    db[":f_header"] = id;
    db.exec("select ah.f_id as f_header, og.f_id as f_goods "
            "from a_header ah "
            "inner join a_store_draft ad on ad.f_document=ah.f_id "
            "inner join o_goods og on og.f_storerec=ad.f_id "
            "inner join o_body ob on ob.f_id=og.f_body "
            "where ob.f_header=:f_header");
    QSet<QString> aheader;
    QSet<QString> ogoods;

    while(db.nextRow()) {
        aheader.insert(db.getString("f_header"));
        ogoods.insert(db.getString("f_goods"));
    }

    foreach(const QString &s, ogoods) {
        db.deleteFromTable("o_goods", s);
    }

    foreach(const QString &s, aheader) {
        db.deleteFromTable("a_store", "f_document", s);
        db.deleteFromTable("a_store_draft", "f_document", s);
        db.deleteFromTable("a_header_store", s);
        db.deleteFromTable("a_header", s);
    }
}

int C5WaiterOrderDoc::hInt(const QString &name)
{
    return fHeader[name].toInt();
}

double C5WaiterOrderDoc::hDouble(const QString &name)
{
    return fHeader[name].toDouble();
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
    return fItems[index].toObject()[name].toInt();
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
    db[":f_id"] = hString("f_id");
    db.exec("select h.f_name as f_hallname, t.f_name as f_tableName, concat(s.f_last, ' ', s.f_first) as f_staffname, \
        o.*, oo.f_guests, oo.f_splitted \
        from o_header o \
        left join h_tables t on t.f_id=o.f_table \
        left join h_halls h on h.f_id=t.f_hall \
        left join s_user s on s.f_id=o.f_staff \
        left join o_header_options oo on oo.f_id=o.f_id \
        where o.f_id=:f_id \
        order by o.f_id ");

    if(db.nextRow()) {
        QMap<QString, QVariant> h;
        db.rowToMap(h);
        fHeader = QJsonObject::fromVariantMap(h);
    }

    getTaxInfo(db);
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

    while(db.nextRow()) {
        QMap<QString, QVariant> h;
        db.rowToMap(h);
        fItems.append(QJsonObject::fromVariantMap(h));
    }

    // Discount
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select c.f_id, c.f_value, c.f_mode, cn.f_name, p.f_contact "
            "from b_history h "
            "left join b_cards_discount c on c.f_id=h.f_card "
            "left join c_partners p on p.f_id=c.f_client "
            "left join b_card_types cn on cn.f_id=c.f_mode "
            "where h.f_id=:f_id");

    if(db.nextRow()) {
        fHeader["f_bonusid"] = db.getString("f_id");
        fHeader["f_bonustype"] = db.getString("f_mode");
        fHeader["f_bonusname"] = db.getString("f_name");
        fHeader["f_bonusvalue"] = db.getString("f_value");
        fHeader["f_bonusholder"] = db.getString("f_contact");
        fHeader["f_discountfactor"] = QString::number(db.getDouble("f_value") / 100, 'f', 3);
    }

    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select * from o_pay_room where f_id=:f_id");

    if(db.nextRow()) {
        fHeader["f_other_res"] = db.getString("f_res");
        fHeader["f_other_room"] = db.getString("f_room");
        fHeader["f_other_guest"] = db.getString("f_guest");
        fHeader["f_other_inv"] = db.getString("f_inv");
    }

    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select * from o_pay_cl where f_id=:f_id");

    if(db.nextRow()) {
        fHeader["f_other_clcode"] = db.getString("f_code");
        fHeader["f_other_clname"] = db.getString("f_name");
    }
}

void C5WaiterOrderDoc::getTaxInfo(C5Database &db)
{
    db[":f_id"] = hString("f_id");
    db.exec("select * from o_tax where f_id=:f_id");

    if(db.nextRow()) {
        for(int i = 0; i < db.columnCount(); i++) {
            fTax[db.columnName(i)] = db.getString(i);
        }
    }
}

void C5WaiterOrderDoc::calculateSelfCost(C5Database &db)
{
    /* this method used for assigning price from prime cost in metropol */
    QMap<int, QMap<int, double> > goodsQty;
    QMap<int, double> price;
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select b.f_dish, r.f_goods, r.f_qty "
            "from d_recipes r "
            "left join o_body b on b.f_dish=r.f_dish "
            "left join o_header h on h.f_id=b.f_header "
            "where b.f_state=1 and h.f_id=:f_id ");

    while(db.nextRow()) {
        goodsQty[db.getInt(0)][db.getInt(1)] = db.getDouble(2);
    }

    db.exec("select s.f_goods, sum(s.f_total)/sum(f_qty) "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_state=1 and h.f_type=1 and s.f_type=1 "
            "group by 1");

    while(db.nextRow()) {
        price[db.getInt(0)] = db.getDouble(1);
    }

    double total = 0;

    for(int i = 0; i < fItems.count(); i++) {
        QJsonObject o = fItems.at(i).toObject();

        if(o["f_state"].toInt() != DISH_STATE_OK) {
            continue;
        }

        if(!goodsQty.contains(o["f_dish"].toInt())) {
            total += o["f_total"].toDouble();
            continue;
        }

        double selfcost = 0;
        QMap<int, double> recipe = goodsQty[o["f_dish"].toInt()];

        for(QMap<int, double>::const_iterator it = recipe.constBegin(); it != recipe.constEnd(); it++) {
            selfcost += price[it.key()] * it.value();
        }

        o["f_price"] = QString::number(selfcost, 'f', 2);
        o["f_service"] = "0";
        o["f_total"] = QString::number(selfcost * o["f_qty1"].toDouble(), 'f', 2);
        db[":f_id"] = o["f_id"].toString();
        db[":f_price"] = selfcost;
        db[":f_total"] = selfcost * o["f_qty1"].toDouble();
        db.exec("update o_body set f_price=:f_price, f_total=:f_total where f_id=:f_id");
        total += selfcost * o["f_qty1"].toDouble();
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

void C5WaiterOrderDoc::run()
{
    QString err;
    C5StoreDraftWriter dw(fDb);
    dw.writeStoreOfSale(fHeader["f_id"].toString(), err, DOC_STATE_SAVED);
    emit finished();
}

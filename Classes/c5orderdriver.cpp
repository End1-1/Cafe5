#include "c5orderdriver.h"
#include "c5utils.h"

C5OrderDriver::C5OrderDriver(const QStringList &dbParams) :
    fDb(dbParams)
{
}

bool C5OrderDriver::openTable(int table)
{
    fTable = table;
    fDb.startTransaction();
    fDb[":f_id"] = table;
    fDb.exec("select * from h_tables where f_id=:f_id for update");
    if (!fDb.nextRow()) {
        fLastError = fDb.fLastError;
        return false;
    }
    QDateTime lockTime = fDb.getDateTime("f_locktime");
    if (lockTime.isValid()) {
        if (lockTime.msecsTo(QDateTime::currentDateTime()) < 60000) {
            if (fDb.getString("f_locksrc") != hostinfo) {
                fLastError = tr("Table already locked");
            }
            return false;
        }
    }
    fDb[":f_locktime"] = QDateTime::currentDateTime();
    fDb[":f_lockSrc"] = hostinfo;
    fDb.update("h_tables", "f_id", table);
    fDb.commit();

    fDb[":f_state"] = ORDER_STATE_OPEN;
    fDb[":f_table"] = table;
    fDb.exec("select o.f_id from o_header o where o.f_table=:f_table and o.f_state=:f_state");
    QStringList orders;
    while (fDb.nextRow()) {
        orders.append("'" + fDb.getString("f_id") + "'");
    }

    return loadData(orders.join(","));
}

bool C5OrderDriver::loadData(const QString &id)
{
    if (id.isEmpty()) {
        return true;
    }

    if (!fDb.exec("select h.f_name as f_hallname, t.f_name as f_tableName, concat(s.f_last, ' ', s.f_first) as f_staffname, "
        "o.*, oo.f_guests, oo.f_splitted, co.f_car as car "
        "from o_header o "
        "left join h_tables t on t.f_id=o.f_table "
        "left join h_halls h on h.f_id=t.f_hall "
        "left join s_user s on s.f_id=o.f_staff "
        "left join b_car_orders co on co.f_order=o.f_id "
        "left join o_header_options oo on oo.f_id=o.f_id "
        "where o.f_id in (" + id + ") "
        "order by o.f_id ")) {
        fLastError = fDb.fLastError;
        return false;
    }
    while (fDb.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < fDb.columnCount(); i++) {
            row[fDb.columnName(i)] = fDb.getValue(i);
        }
        fHeader[fDb.getString("f_id")] = row;
    }

    if (!fDb.exec("select * from o_tax where f_id in (" + id + ")")) {
        fLastError = fDb.fLastError;
        return false;
    }
    while (fDb.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < fDb.columnCount(); i++) {
            row[fDb.columnName(i)] = fDb.getValue(i);
        }
        fTaxInfo[fDb.getString("f_id")] = row;
    }

    if(!fDb.exec("select ob.f_id, ob.f_header, ob.f_state, dp1.f_name as part1, dp2.f_name as part2, ob.f_adgcode, d.f_name as f_name, "
             "ob.f_qty1, ob.f_qty2, ob.f_price, ob.f_service, ob.f_discount, ob.f_total, "
             "ob.f_store, ob.f_print1, ob.f_print2, ob.f_comment, ob.f_remind, ob.f_dish, "
             "s.f_name as f_storename, ob.f_removereason, ob.f_timeorder, ob.f_package, d.f_hourlypayment, "
             "ob.f_canservice, ob.f_candiscount, ob.f_guest "
             "from o_body ob "
             "left join d_dish d on d.f_id=ob.f_dish "
             "left join d_part2 dp2 on dp2.f_id=d.f_part "
             "left join d_part1 dp1 on dp1.f_id=dp2.f_part "
             "left join c_storages s on s.f_id=ob.f_store "
             "where ob.f_header in (" + id + ")")) {
        fLastError = fDb.fLastError;
        return false;
    }
    while (fDb.nextRow()) {
        QMap<QString, QVariant> row;
        for (int i = 0; i < fDb.columnCount(); i++) {
            row[fDb.columnName(i)] = fDb.getValue(i);
        }
        if (!fDishes.contains(fDb.getString("f_header"))) {
            fDishes[fDb.getString("f_header")] = QList<QMap<QString, QVariant> >();
        }
        fDishes[fDb.getString("f_header")].append(row);
    }
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

C5OrderDriver C5OrderDriver::setCurrentOrderID(const QString &id)
{
    fCurrentOrderId = id;
    return *this;
}

C5OrderDriver C5OrderDriver::setHeader(const QString &key, const QVariant &value)
{
    return *this;
}

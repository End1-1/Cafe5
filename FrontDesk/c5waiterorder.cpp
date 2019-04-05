#include "c5waiterorder.h"
#include "ui_c5waiterorder.h"
#include "c5utils.h"

C5WaiterOrder::C5WaiterOrder(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5WaiterOrder)
{
    ui->setupUi(this);
    fLabel = tr("Order");
    fIcon = ":/order.png";
}

C5WaiterOrder::~C5WaiterOrder()
{
    delete ui;
}

void C5WaiterOrder::setOrder(const QString &id)
{

    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select o.f_prefix, os.f_name as f_statename, h.f_name as f_hallname, t.f_name as f_tableName, concat(s.f_last, ' ', s.f_first) as f_staffname, "
            "o.* "
            "from o_header o "
            "left join h_tables t on t.f_id=o.f_table "
            "left join h_halls h on h.f_id=t.f_hall "
            "left join s_user s on s.f_id=o.f_staff "
            "left join o_state os on os.f_id=o.f_state "
            "where o.f_id=:f_id ");
    if (db.nextRow()) {
        ui->leUuid->setText(id);
        ui->leNumber->setText(db.getString("f_prefix") + db.getString("f_hallid"));
        ui->deDateOpen->setDate(db.getDate("f_dateopen"));
        ui->teOpen->setTime(db.getTime("f_timeopen"));
        ui->deDateClose->setDate(db.getDate("f_dateclose"));
        ui->teClose->setTime(db.getTime("f_timeclose"));
        ui->deDateCash->setDate(db.getDate("f_datecash"));
        ui->leState->setText(db.getString("f_statename"));
        ui->leHall->setText(db.getString("f_hallname"));
        ui->leTable->setText(db.getString("f_tablename"));
        ui->leTotal->setText(float_str(db.getDouble("f_amounttotal"), 2));
    } else {
        C5Message::error(tr("Invalid order uuid"));
        return;
    }
    db[":f_header"] = id;
    db.exec("select ob.f_id, ob.f_header, ob.f_state, obs.f_name as f_statename, dp1.f_name as part1, dp2.f_name as part2, ob.f_adgcode, d.f_name as f_name, "
             "ob.f_qty1, ob.f_qty2, ob.f_price, ob.f_service, ob.f_discount, ob.f_total, "
             "ob.f_store, ob.f_print1, ob.f_print2, ob.f_comment, ob.f_remind, ob.f_dish, "
             "s.f_name as f_storename, ob.f_removereason, ob.f_store, st.f_name as f_storename "
             "from o_body ob "
             "left join d_dish d on d.f_id=ob.f_dish "
             "left join d_part2 dp2 on dp2.f_id=d.f_part "
             "left join d_part1 dp1 on dp1.f_id=dp2.f_part "
             "left join c_storages s on s.f_id=ob.f_store "
            "left join o_body_state obs on obs.f_id=ob.f_state "
            "left join c_storages st on st.f_id=ob.f_store "
             "where ob.f_header=:f_header");
    while (db.nextRow()) {
        int row = ui->tblDishes->addEmptyRow();
        ui->tblDishes->setString(row, 0, db.getString("f_id"));
        ui->tblDishes->setString(row, 1, db.getString("f_state"));
        ui->tblDishes->setString(row, 2, db.getString("f_statename"));
        ui->tblDishes->setString(row, 3, db.getString("f_dish"));
        ui->tblDishes->setString(row, 4, db.getString("f_name"));
        ui->tblDishes->setDouble(row, 5, db.getDouble("f_qty1"));
        ui->tblDishes->setDouble(row, 6, db.getDouble("f_qty2"));
        ui->tblDishes->setDouble(row, 7, db.getDouble("f_price"));
        ui->tblDishes->setDouble(row, 8, db.getDouble("f_total"));
        ui->tblDishes->setDouble(row, 9, db.getDouble("f_service"));
        ui->tblDishes->setDouble(row, 10, db.getDouble("f_discount"));
        ui->tblDishes->setString(row, 11, db.getString("f_store"));
        ui->tblDishes->setString(row, 12, db.getString("f_storename"));
        ui->tblDishes->setString(row, 13, db.getString("f_comment"));
        if (db.getInt("f_state") != DISH_STATE_OK) {
            ui->tblDishes->setRowHidden(row, true);
        }
    }
    ui->tblDishes->setColumnWidths(ui->tblDishes->columnCount(), 0, 0, 100, 0, 300, 80, 80, 80, 80, 0, 0, 0, 100, 100, 100);
}

bool C5WaiterOrder::allowChangeDatabase()
{
    return false;
}

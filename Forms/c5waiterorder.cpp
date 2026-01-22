#include "c5waiterorder.h"
#include "ui_c5waiterorder.h"
#include "c5utils.h"
#include "c5waiterorder.h"
#include "c5waiterorderdoc.h"
#include "c5storedraftwriter.h"
#include "c5config.h"
#include "c5permissions.h"
#include "c5user.h"
#include "c5dishwidget.h"
#include "c5mainwindow.h"
#include "dict_dish_state.h"
#include "proxytablewidgetdatabase.h"
#include "c5message.h"
#include "c5cashdoc.h"
#include "c5airlog.h"
#include "dlgsetwaiterordercl.h"
#include <QMenu>
#include <QClipboard>

C5WaiterOrder::C5WaiterOrder(QWidget *parent) :
    C5OfficeWidget(parent),
    ui(new Ui::C5WaiterOrder)
{
    ui->setupUi(this);
    fLabel = tr("Order");
    fIconName = ":/order.png";
    ui->tblLog->setColumnWidths(ui->tblLog->columnCount(), 100, 80, 200, 200, 200, 200);
    ui->tblStore->setColumnWidths(ui->tblStore->columnCount(), 80, 300, 200, 300, 80, 80, 80, 80);
    ui->rbDraft->setChecked(__c5config.getRegValue("sale_selfcost_mode").toBool());
    ui->rbWrited->setChecked(!__c5config.getRegValue("sale_selfcost_mode").toBool());
}

C5WaiterOrder::~C5WaiterOrder()
{
    delete ui;
}

void C5WaiterOrder::setOrder(const QString &id)
{
    C5Database db;
    db[":f_id"] = id;
    db.exec("select o.f_prefix, os.f_name as f_statename, h.f_name as f_hallname, t.f_name as f_tableName, "
            "concat(s.f_last, ' ', s.f_first) as f_staffname, "
            "o.*, oth.f_name as f_othername "
            "from o_header o "
            "left join h_tables t on t.f_id=o.f_table "
            "left join h_halls h on h.f_id=t.f_hall "
            "left join s_user s on s.f_id=o.f_staff "
            "left join o_state os on os.f_id=o.f_state "
            "left join o_pay_other oth on oth.f_id=o.f_otherid "
            "where o.f_id=:f_id ");

    if(db.nextRow()) {
        ui->leUuid->setText(id);
        ui->leNumber->setText(db.getString("f_prefix") + QString::number(db.getInt("f_hallid")));
        ui->deDateOpen->setDate(db.getDate("f_dateopen"));
        ui->teOpen->setTime(db.getTime("f_timeopen"));
        ui->deDateClose->setDate(db.getDate("f_dateclose"));
        ui->teClose->setTime(db.getTime("f_timeclose"));
        ui->deDateCash->setDate(db.getDate("f_datecash"));
        ui->leState->setText(db.getString("f_statename"));
        ui->leHall->setText(db.getString("f_hallname"));
        ui->leTable->setText(db.getString("f_tablename"));
        ui->leTable->setProperty("table_id", db.getInt("f_table"));
        ui->leTotal->setText(float_str(db.getDouble("f_amounttotal"), 2));
        ui->leCash->setDouble(db.getDouble("f_amountcash"));
        ui->leCard->setDouble(db.getDouble("f_amountcard"));
        ui->leBank->setDouble(db.getDouble("f_amountbank"));
        ui->leOther->setDouble(db.getDouble("f_amountother"));
        ui->leIdram->setDouble(db.getDouble("f_amountidram"));
        ui->lePayX->setDouble(db.getDouble("f_amountpayx"));
        ui->leOtherName->setText(db.getString("f_othername"));

        if(db.getDouble("f_servicefactor") > 0.001) {
            ui->lbService->setText(QString("%1 %2%").arg(tr("Service")).arg(db.getDouble("f_servicefactor") * 100));
            ui->leService->setDouble(db.getDouble("f_amountservice"));
        }
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

    while(db.nextRow()) {
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

        if(db.getInt("f_state") != DISH_STATE_OK) {
            ui->tblDishes->setRowHidden(row, true);
        }
    }

    ui->tblDishes->setColumnWidths(ui->tblDishes->columnCount(), 0, 0, 100, 0, 300, 80, 80, 80, 80, 0, 0, 0, 100, 100, 100);
    db[":f_id"] = ui->leUuid->text();
    db.exec("select * from o_tax where f_id=:f_id");

    if(db.nextRow()) {
        ui->leTax->setText(db.getString("f_receiptnumber"));
    }

    db[":f_id"] = ui->leUuid->text();
    db.exec("select * from o_pay_cl where f_id=:f_id");

    if(db.nextRow()) {
        ui->leCityLedger->setText(db.getString("f_code"));
    }
}

bool C5WaiterOrder::allowChangeDatabase()
{
    return false;
}

QToolBar* C5WaiterOrder::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        fToolBar = createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/app.png"), tr("Transfer to hotel"), this, SLOT(transferToHotel()));
        fToolBar->addAction(QIcon(":/constructor.png"), tr("Recount selfcost"), this, SLOT(recountSelfCost()));
        fToolBar->addAction(QIcon(":/eye.png"), tr("Show all"), this, SLOT(showAll()));
        fToolBar->addAction(QIcon(":/eye-no.png"), tr("Hide removed"), this, SLOT(hideRemoved()));
        fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeOrder()));
        //TODO
        // if(mUser->check(cp_t5_edit_closed_order)) {
        //     fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveOrder()));
        //     fToolBar->addAction(QIcon(":/storeinput.png"), tr("Store output"), this, SLOT(storeOutput()));
        // } else {
        //     ui->deDateCash->setReadOnly(true);
        //     ui->leCard->setReadOnly(true);
        //     ui->leCash->setReadOnly(true);
        //     ui->leBank->setReadOnly(true);
        //     ui->leOther->setReadOnly(true);
        //     ui->btnClearTax->setEnabled(false);
        // }
    }

    return fToolBar;
}

void C5WaiterOrder::showLog()
{
    ui->tblLog->clearContents();
    C5Database db;
    db[":f_invoice"] = ui->leUuid->text();
    db.exec("select f_date, f_time, f_user, f_action, f_value1, f_value2, f_id "
            "from airlog.log "
            "where f_invoice=:f_invoice "
            "union "
            "select f_date, f_time, '', 'TAX', f_err, f_result, 0 "
            "from o_tax_log "
            "where f_order=:f_invoice "
            "order by 1 desc, f_time desc, 6 desc ");
    ui->tblLog->setRowCount(db.rowCount());
    int row = 0;

    while(db.nextRow()) {
        ui->tblLog->setData(row, 0, db.getDate("f_date"));
        ui->tblLog->setData(row, 1, db.getTime("f_time"));
        ui->tblLog->setData(row, 2, db.getString("f_user"));
        ui->tblLog->setData(row, 3, db.getString("f_action"));
        ui->tblLog->setData(row, 4, db.getString("f_value1"));
        ui->tblLog->setData(row, 5, db.getString("f_value2"));
        row++;
    }
}

void C5WaiterOrder::showStore()
{
    ui->tblStore->clearContents();
    ui->tblStore->setRowCount(0);
    C5Database db;
    db[":f_header"] = ui->leUuid->text();
    db[":f_state1"] = DISH_STATE_OK;
    db[":f_state2"] = DISH_STATE_VOID;
    QString priceField = ui->rbDraft->isChecked() ? "g.f_lastinputprice" : "o.f_price";
    db.exec(QString("select sn.f_name as f_dishstatename, d.f_name as f_dishname, st.f_name as f_storename, "
                    "g.f_name as f_goodsname, b.f_qty1, o.f_qty / b.f_qty1 as f_qty, o.f_qty as f_outputqty, %1, "
                    "o.f_qty*%1 as f_total "
                    "from o_goods o "
                    "inner join o_body b on b.f_id=o.f_body "
                    "inner join o_body_state sn on sn.f_id=b.f_state "
                    "inner join d_dish d on d.f_id=b.f_dish "
                    "inner join c_storages st on st.f_id=o.f_store "
                    "inner join c_goods g on g.f_id=o.f_goods "
                    "where b.f_header=:f_header and (b.f_state=:f_state1 or b.f_state=:f_state2) "
                   ).arg(priceField));
    ProxyTableWidgetDatabase::fillTableWidgetRowFromDatabase(&db, ui->tblStore);
    double t = 0;

    for(int i = 0; i < ui->tblStore->rowCount(); i++) {
        t += ui->tblStore->getDouble(i, 8);
    }

    ui->leSelfCost->setDouble(t);
}

void C5WaiterOrder::saveOrder()
{
    C5Database db;
    db[":f_datecash"] = ui->deDateCash->date();
    db[":f_amountcard"] = ui->leCard->getDouble();
    db[":f_amountcash"] = ui->leCash->getDouble();
    db[":f_amountbank"] = ui->leBank->getDouble();
    db[":f_amountother"] = ui->leOther->getDouble();
    db[":f_amountidram"] = ui->leIdram->getDouble();
    db[":f_amountpayx"] = ui->lePayX->getDouble();
    db[":f_amountother"] = ui->leOther->getDouble();
    db.update("o_header", "f_id", ui->leUuid->text());
    C5Message::info(tr("Saved"));
}

void C5WaiterOrder::removeOrder()
{
    if(C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
        return;
    }

    C5Database db;
    C5StoreDraftWriter dw(db);
    db[":f_header"] = ui->leUuid->text();
    db.exec("select f_id from o_goods where f_header=:f_header");
    QStringList idList;

    while(db.nextRow()) {
        idList.append(db.getString("f_id"));
    }

    for(const QString &id : idList) {
        if(!dw.rollbackOutput(db, id)) {
            C5Message::error(dw.fErrorMsg);
            return;
        }
    }

    db[":f_state"] = ORDER_STATE_VOID;
    db[":f_id"] = ui->leUuid->text();
    db.exec("update o_header set f_state=:f_state where f_id=:f_id");
    C5WaiterOrderDoc::removeDocument(db, ui->leUuid->text());
    C5Airlog::write(hostinfo, mUser->fullName(), LOG_WAITER, "", ui->leUuid->text(), "", tr("Order removed from cash"),
                    ui->leTax->text(), "");
    __mainWindow->removeTab(this);
    C5Message::info(tr("Removed"));
}

void C5WaiterOrder::showAll()
{
    for(int i = 0; i < ui->tblDishes->rowCount(); i++) {
        ui->tblDishes->setRowHidden(i, false);
    }
}

void C5WaiterOrder::hideRemoved()
{
    for(int i = 0; i < ui->tblDishes->rowCount(); i++) {
        if(ui->tblDishes->getInteger(i, 1) != DISH_STATE_OK) {
            ui->tblDishes->setRowHidden(i, true);
        }
    }
}

void C5WaiterOrder::transferToHotel()
{
    C5Database db;
    C5StoreDraftWriter d(db);
    QString err;
    d.transferToHotel(db, ui->leUuid->text(), err);

    if(err.isEmpty()) {
        C5Message::info(tr("Done"));
    } else {
        C5Message::error(err);
    }
}

void C5WaiterOrder::recountSelfCost()
{
    C5Database db;
    C5WaiterOrderDoc d(ui->leUuid->text(), db);
    d.calculateSelfCost(db);
    ui->leTotal->setDouble(d.hDouble("f_amounttotal"));
    C5Message::info(tr("Done"));
}

void C5WaiterOrder::storeOutput()
{
    C5Database db;
    QString err;
    C5StoreDraftWriter d(db);
    d.writeStoreOfSale(ui->leUuid->text(),  err, DOC_STATE_SAVED);

    if(err.isEmpty()) {
        C5Message::info(tr("Done"));
    } else {
        C5Message::error(err);
    }
}

void C5WaiterOrder::openMenuItem()
{
    QModelIndexList ml = ui->tblDishes->selectionModel()->selectedRows();

    if(ml.count() == 0) {
        return;
    }

    C5DishWidget *ep = new C5DishWidget();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QList<QMap<QString, QVariant> > d;
    ep->setId(ui->tblDishes->getInteger(ml.at(0).row(), 3));
    e->getResult(d);
    delete e;
}

void C5WaiterOrder::on_tblDishes_customContextMenuRequested(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(tr("Open menu item"), this, SLOT(openMenuItem()));
    m->popup(ui->tblDishes->mapToGlobal(pos));
}

void C5WaiterOrder::on_tabWidget_currentChanged(int index)
{
    switch(index) {
    case 2:
        showStore();
        break;

    case 3:
        showLog();
        break;
    }
}

void C5WaiterOrder::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(ui->leUuid->text());
}

void C5WaiterOrder::on_btnClearTax_clicked()
{
    if(C5Message::question(tr("Are you sure to clear tax info?")) != QDialog::Accepted) {
        return;
    }

    C5Database db;
    db[":f_id"] = ui->leUuid->text();
    db.exec("select * from o_tax where f_id=:f_id");

    if(db.nextRow()) {
        for(int i = 0; i < db.columnCount(); i++) {
            db[":" + db.columnName(i)] = QVariant();
        }

        db.update("o_tax", "f_id", ui->leUuid->text());
        C5Airlog::write(hostinfo, mUser->fullName(), LOG_WAITER, "", ui->leUuid->text(), "", "Clear tax info",
                        ui->leTax->text(), "");
        ui->leTax->clear();
    }
}

void C5WaiterOrder::on_btnSetCL_clicked()
{
    if(__c5config.hotelDatabase().isEmpty()) {
        return;
    }

    QString clcode, clname;

    if(DlgSetWaiterOrderCL::getCL(mUser, clcode, clname)) {
        ui->leCityLedger->setText(clcode);
        C5Database db;
        db[":f_code"] = clcode;
        db[":f_name"] = clname;
        db.update("o_pay_cl", "f_id", ui->leUuid->text());
        db[":f_id"] = ui->leNumber->text();
        db[":f_paymentmode"] = 4;
        db[":f_dc"] = "DEBIT";
        db[":f_sign"] = -1;
        db[":f_cityledger"] = clcode.toInt();

        if(!db.exec(QString("update %1.m_register set f_cityledger=:f_cityledger, "
                            "f_paymentmode=:f_paymentmode, f_dc=:f_dc, f_sign=:f_sign where f_id=:f_id").arg(__c5config.hotelDatabase()))) {
            C5Message::error(db.fLastError);
            return;
        }

        C5Message::info(tr("Saved"));
    }
}

void C5WaiterOrder::on_btnClearCL_clicked()
{
    if(C5Message::question(tr("Clear CL information?")) == QDialog::Accepted) {
        C5Database db;
        db[":f_code"] = "";
        db[":f_name"] = "";
        db.update("o_pay_cl", "f_id", ui->leUuid->text());
        db[":f_id"] = ui->leNumber->text();
        db[":f_cityledger"] = 0;
        db.exec(QString("update %1.m_register set f_cityledger=:f_cityledger where f_id=:f_id").arg(
                    __c5config.hotelDatabase()));
        C5Message::info(tr("Saved"));
    }
}

void C5WaiterOrder::on_rbDraft_clicked(bool checked)
{
    __c5config.setRegValue("sale_selfcost_mode", checked);
    showStore();
}

void C5WaiterOrder::on_rbWrited_clicked(bool checked)
{
    __c5config.setRegValue("sale_selfcost_mode", !checked);
    showStore();
}

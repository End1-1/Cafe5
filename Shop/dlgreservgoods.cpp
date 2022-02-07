#include "dlgreservgoods.h"
#include "ui_dlgreservgoods.h"
#include "c5database.h"
#include "goodsreserve.h"
#include "datadriver.h"
#include "c5message.h"
#include "threadsendmessage.h"
#include "chatmessage.h"
#include "c5config.h"
#include <QJsonDocument>
#include <QJsonObject>

DlgReservGoods::DlgReservGoods(int store, int goods, double qty) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgReservGoods)
{
    ui->setupUi(this);
    fStore = store;
    fGoods = goods;
    ui->leName->setText(dbgoods->name(fGoods));
    ui->leScancode->setText(dbgoods->scancode(fGoods));
    ui->leStore->setText(dbstore->name(fStore));
    ui->leTotalQty->setDouble(qty);
    ui->btnCancelReserve->setVisible(false);
    ui->btnCompleteReserve->setVisible(false);
    ui->leEndDay->setDate(QDate::currentDate().addDays(1));
}

DlgReservGoods::DlgReservGoods(int id) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgReservGoods)
{
    ui->setupUi(this);
    C5Database db(__c5config.replicaDbParams());
    db[":f_id"] = id;
    db.exec("select * from a_store_reserve where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid reservation code"));
        return;
    }
    fGoods = db.getInt("f_goods");
    fStore = db.getInt("f_store");
    ui->leCode->setInteger(db.getInt("f_id"));
    ui->leDate->setDate(db.getDate("f_date"));
    ui->leEndDay->setDate(db.getDate("f_enddate"));
    ui->leName->setText(dbgoods->name(db.getInt("f_goods")));
    ui->leScancode->setText(dbgoods->scancode(db.getInt("f_goods")));
    ui->leStore->setText(dbstore->name(db.getInt("f_store")));
    ui->leTotalQty->setText("-");
    ui->leReservedQty->setDouble(db.getDouble("f_qty"));
    ui->leMessage->setText(db.getString("f_message"));
    ui->btnSave->setVisible(false);
    ui->btnCancelReserve->setVisible(db.getInt("f_state") == GR_RESERVED);
    ui->btnCompleteReserve->setVisible(db.getInt("f_state") == GR_RESERVED);
    ui->leReservedQty->setReadOnly(true);
}

DlgReservGoods::~DlgReservGoods()
{
    delete ui;
}

void DlgReservGoods::on_btnClose_clicked()
{
    reject();
}

void DlgReservGoods::on_btnSave_clicked()
{
    if (!ui->leEndDay->date().isValid()) {
        C5Message::error(tr("The end date is not valid"));
        return;
    }
    DbGoods g(fGoods);
    insertReserve(__c5config.replicaDbParams());
    if (__c5config.rdbReplica()) {
        insertReserve(__c5config.dbParams());
    }
    ui->btnSave->setVisible(false);
    ui->btnCancelReserve->setVisible(true);
    ui->btnCompleteReserve->setVisible(true);
    QJsonObject jo;
    jo["action"] = MSG_GOODS_RESERVE;
    jo["goods"] = fGoods;
    jo["qty"] = ui->leReservedQty->getDouble();
    jo["goodsname"] = ui->leName->text();
    jo["scancode"] = ui->leScancode->text();
    jo["unit"] = g.unit()->unitName();
    jo["usermessage"] = ui->leMessage->text();
    jo["enddate"] = ui->leEndDay->text();
    QJsonDocument jdoc(jo);
    ThreadSendMessage t;
    t.send(fStore, jdoc.toJson(QJsonDocument::Compact));
}

void DlgReservGoods::on_btnCancelReserve_clicked()
{
    setState(GR_REMOVED);
}

void DlgReservGoods::setState(int state)
{
    updateState(__c5config.replicaDbParams(), state);
    if (__c5config.rdbReplica()) {
        updateState(__c5config.dbParams(), state);
    }
    ui->btnCancelReserve->setVisible(false);
    ui->btnCompleteReserve->setVisible(false);
}

void DlgReservGoods::updateState(const QStringList &dbparams, int state)
{
    C5Database db(dbparams);
    db[":f_state"] = state;
    switch (state) {
    case GR_COMPLETED:
        db[":f_completed"] = __c5config.defaultStore();
        db[":f_completeddate"] = QDate::currentDate();
        db[":f_completedtime"] = QTime::currentTime();
        break;
    case GR_REMOVED:
        db[":f_canceled"] = __c5config.defaultStore();
        db[":f_canceleddate"] = QDate::currentDate();
        db[":f_canceledtime"] = QTime::currentTime();
        break;
    }
    db.update("a_store_reserve", "f_id", ui->leCode->getInteger());
    db[":f_qty"] = ui->leReservedQty->getDouble();
    db[":f_goods"] = fGoods;
    db[":f_store"] = fStore;
    db.exec("update a_store_sale set f_qtyreserve=f_qtyreserve-:f_qty where f_store=:f_store and f_goods=:f_goods");
}

void DlgReservGoods::insertReserve(const QStringList &dbparams)
{
    C5Database db(dbparams);
    db[":f_store"] = fStore;
    db[":f_goods"] = fGoods;
    db[":f_qty"] = ui->leReservedQty->getDouble();
    db[":f_message"] = ui->leMessage->text();
    db[":f_source"] = __c5config.defaultStore();
    db[":f_enddate"] = ui->leEndDay->date();
    if (ui->leCode->isEmpty()) {
        db[":f_date"] = QDate::currentDate();
        db[":f_time"] = QTime::currentTime();
        db[":f_state"] = GR_RESERVED;
        ui->leCode->setInteger(db.insert("a_store_reserve", true));
        db[":f_qty"] = ui->leReservedQty->getDouble();
        db[":f_goods"] = fGoods;
        db[":f_store"] = fStore;
        db.exec("update a_store_sale set f_qtyreserve=f_qtyreserve+:f_qty where f_store=:f_store and f_goods=:f_goods");
    } else {
        db.update("a_store_reserve", "f_id", ui->leCode->getInteger());
    }
}

void DlgReservGoods::on_btnCompleteReserve_clicked()
{
    setState(GR_COMPLETED);
}

void DlgReservGoods::on_leReservedQty_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    if (ui->leReservedQty->getDouble() > ui->leTotalQty->getDouble()) {
        ui->leReservedQty->setDouble(ui->leTotalQty->getDouble());
        C5Message::error(tr("Insufficient quantity"));
        return;
    }
}

#include "dlgreservgoods.h"
#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5database.h"
#include "c5message.h"
#include "chatmessage.h"
#include "goodsreserve.h"
#include "ninterface.h"
#include "printtaxn.h"
#include "ui_dlgreservgoods.h"

#include "c5user.h"
#if(!defined FRONTDESK && !defined WAITER)
#include "worder.h"
#include "working.h"
#include "c5structtableview.h"
#endif
#include <QJsonDocument>
#include <QJsonObject>

DlgReservGoods::DlgReservGoods(C5User *user, StorageItem store, GoodsItem goods, double qty)
    : C5Dialog(user)
    , ui(new Ui::DlgReservGoods)
{
    ui->setupUi(this);
    fStore = store;
    fGoods = goods;
    ui->leName->setText(fGoods.name);
    ui->leScancode->setText(fGoods.barcode);
    ui->leStore->setText(fStore.name);
    ui->leTotalQty->setDouble(qty);
    ui->btnCancelReserve->setVisible(false);
    ui->btnCompleteReserve->setVisible(false);
    ui->leEndDay->setDate(QDate::currentDate().addDays(1));
}

DlgReservGoods::DlgReservGoods(int id, C5User *user)
    : C5Dialog(user)
    , ui(new Ui::DlgReservGoods)
{
    ui->setupUi(this);
    NInterface::query1("/engine/v2/shop/shop-reserve/get",
                       mUser->mSessionKey,
                       this,
                       {{"id", id}},
                       [this](const QJsonObject &jo) {
                           fGoods = JsonParser<GoodsItem>::fromJson(
                               jo.value("goods_json").toObject());
                           fStore = JsonParser<StorageItem>::fromJson(
                               jo.value("store_json").toObject());
                           ui->leCode->setInteger(jo.value("f_id").toInt());
                           ui->leDate->setDate(QDate::fromString(jo.value("f_date").toString(),
                                                                 FORMAT_DATE_TO_STR_MYSQL));
                           ui->leEndDay->setDate(QDate::fromString(jo.value("f_enddate").toString(),
                                                                   FORMAT_DATE_TO_STR_MYSQL));
                           ui->leName->setText(fGoods.name);
                           ui->leScancode->setText(fGoods.barcode);
                           ui->leStore->setText(fStore.name);
                           ui->leTotalQty->setText("-");
                           ui->leReservedQty->setDouble(jo.value("f_qty").toDouble());
                           ui->leMessage->setText(jo.value("f_message").toString());
                           ui->btnSave->setVisible(false);
                           ui->btnCancelReserve->setVisible(jo.value("f_state") == GR_RESERVED);
                           ui->btnCompleteReserve->setVisible(jo.value("f_state") == GR_RESERVED);
                           ui->leReservedQty->setReadOnly(true);
                           ui->lePrepaid->setDouble(jo.value("f_prepaid").toDouble());
                           ui->lePrepaidCard->setDouble(jo.value("f_prepaidcard").toDouble());
                           ui->leFiscal->setInteger(jo.value("f_fiscal").toInteger());
                           ui->btnPrintFiscal->setEnabled(ui->leFiscal->getInteger() == 0);
                       });
}

DlgReservGoods::DlgReservGoods(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgReservGoods)
{
    ui->setupUi(this);
}

DlgReservGoods::~DlgReservGoods()
{
    delete ui;
}

void DlgReservGoods::createReserveResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    ui->leCode->setInteger(jdoc["f_id"].toInt());
    ui->leFiscal->setInteger(jdoc["f_fiscal"].toInt());
    ui->btnPrintFiscal->setEnabled(jdoc["f_fiscal"].toInt() == 0);
    fHttp->httpQueryFinished(sender());
}

void DlgReservGoods::on_btnClose_clicked()
{
    reject();
}

void DlgReservGoods::on_btnSave_clicked()
{
    QString err;

    if (fGoods.id == 0) {
        err += tr("Goods is not specified") + "<br>";
    }

    if (fStore.id == 0) {
        err += tr("Store is not specified") + "<br>";
    }

    if(!ui->leEndDay->date().isValid()) {
        err += tr("The end date is not valid") + "<br>";
    }

    if(ui->leEndDay->date() > QDate::currentDate().addDays(1)) {
        err += tr("The end date can be tomorrow");
    }

    if(ui->leReservedQty->getDouble() < 1) {
        err += tr("Incorrect quantity");
    }

    if(!err.isEmpty()) {
        C5Message::error(err);
        return;
    }

    ui->btnSave->setVisible(false);
    ui->btnCancelReserve->setVisible(true);
    ui->btnCompleteReserve->setVisible(true);
    QJsonObject jo;
    jo["action"] = MSG_GOODS_RESERVE;
    jo["goods"] = fGoods.id;
    jo["userfrom"] = __c5config.defaultStore();
    jo["userto"] = fStore.id;
    jo["qty"] = ui->leReservedQty->getDouble();
    jo["goodsname"] = ui->leName->text();
    jo["scancode"] = ui->leScancode->text();
    jo["unit"] = fGoods.unitName;
    jo["usermessage"] = ui->leMessage->text();
    jo["enddate"] = ui->leEndDay->text();
    jo["f_id"] = ui->leCode->getInteger();
    jo["f_enddate"] = ui->leEndDay->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    jo["f_goods"] = fGoods.id;
    jo["f_qty"] = ui->leReservedQty->getDouble();
    jo["f_message"] = ui->leMessage->text();
    jo["f_prepaid"] = ui->lePrepaid->getDouble();
    jo["f_prepaidcard"] = ui->lePrepaidCard->getDouble();
    jo["f_fiscal"] = ui->leFiscal->getInteger();
    fHttp->createHttpQuery("/engine/shop/create-reserve.php", jo, SLOT(createReserveResponse(QJsonObject)));
}

void DlgReservGoods::on_btnCancelReserve_clicked()
{
    //    if (fStore != 23) {
    //        C5Message::error("No-no-no!");
    //        return;
    //    }
    setState(GR_REMOVED);
}

void DlgReservGoods::setState(int state)
{
    updateState(state);
    ui->btnCancelReserve->setVisible(false);
    ui->btnCompleteReserve->setVisible(false);
#if(!defined FRONTDESK && !defined WAITER)

    //TODO UPDATE
    if (state == GR_COMPLETED) {
        WOrder *wo = Working::working()->newSale(SALE_RETAIL);
        wo->checkGoodsCode(fGoods.barcode, [=]() {
            wo->setQtyOfRow(0, ui->leReservedQty->getDouble());
            wo->fOHeader.amountPrepaid = ui->lePrepaid->getDouble()
                                         + ui->lePrepaidCard->getDouble();
        });
    }
#endif
}

void DlgReservGoods::updateState(int state)
{
    C5Database db;
    db[":f_state"] = state;

    switch(state) {
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
    db[":f_goods"] = fGoods.id;
    db[":f_store"] = fStore.id;
    db.exec("update a_store_sale set f_qtyreserve=f_qtyreserve-:f_qty "
            "where f_store=:f_store and f_goods=:f_goods");
}

void DlgReservGoods::on_btnCompleteReserve_clicked()
{
    setState(GR_COMPLETED);
}

void DlgReservGoods::on_leReservedQty_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);

    if(ui->leReservedQty->getDouble() > ui->leTotalQty->getDouble()) {
        ui->leReservedQty->setDouble(ui->leTotalQty->getDouble());
        C5Message::error(tr("Insufficient quantity"));
        return;
    }
}

void DlgReservGoods::on_btnGoods_clicked()
{
#if(!defined FRONTDESK && !defined WAITER)
    QVector<GoodsItem> result = selectItem<GoodsItem>();

    if(result.isEmpty()) {
        return;
    }

    GoodsItem si = result.at(0);
    ui->leName->setText(si.name);
    ui->leScancode->setText(si.barcode);
    fGoods = si;
    getAvailableGoods();
    //TODO
    // DlgDataOnline::DataResult r;
    // if(DlgDataOnline::get("c_goods", r)) {
    //     ui->leName->setText(r.value(0, tr("Name")).toString());
    //     ui->leScancode->setText(r.value(0, tr("Scancode")).toString());
    //     fGoods = r.value(0, tr("Code")).toInt();
    //     getAvailableGoods();
    // }
#endif
}

void DlgReservGoods::on_btnStore_clicked()
{
#if(!defined FRONTDESK && !defined WAITER)
    QVector<StorageItem> result = C5StructTableView::get<StorageItem>(SelectorName<StorageItem>::value, true, false,  mapPointToGlobal(ui->btnStore));

    if(result.isEmpty()) {
        return;
    }

    StorageItem si = result.at(0);
    ui->leStore->setText(si.name);
    fStore = si;
    getAvailableGoods();
#endif
}

void DlgReservGoods::getAvailableGoods()
{
    ui->leTotalQty->setDouble(0);

    if (fGoods.id > 0 && fStore.id > 0) {
        C5Database db;
        db[":f_store"] = fStore.id;
        db[":f_goods"] = fGoods.id;
        db.exec("select sum(f_qty*f_type) from a_store where f_store=:f_store and f_goods=:f_goods");

        if(db.nextRow()) {
            ui->leTotalQty->setDouble(db.getDouble(0));
        }

        db[":f_store"] = fStore.id;
        db[":f_goods"] = fGoods.id;
        db[":f_state"] = GR_RESERVED;
        db.exec("select sum(f_qty) from a_store_reserve where f_store=:f_store and f_goods=:f_goods and f_state=:f_state");

        if(db.nextRow()) {
            ui->leTotalQty->setDouble(ui->leTotalQty->getDouble() - db.getDouble(0));
        }
    }
}

void DlgReservGoods::on_btnPrintFiscal_clicked()
{
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(),
                 C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsonIn, jsonOut, err;

    if(pt.printAdvanceJson(ui->lePrepaid->getDouble(), ui->lePrepaidCard->getDouble(), jsonIn, jsonOut,
                           err) != pt_err_ok) {
        C5Message::error(err);
    } else {
        QJsonObject jdoc = QJsonDocument::fromJson(jsonOut.toUtf8()).object();
        ui->leFiscal->setInteger(jdoc["rseq"].toInt());
        on_btnSave_clicked();
        ui->btnPrintFiscal->setEnabled(false);
    }
}

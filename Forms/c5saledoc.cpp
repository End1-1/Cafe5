#include "c5saledoc.h"
#include "ui_c5saledoc.h"
#include "c5message.h"
#include "c5utils.h"
#include "c5cache.h"
#include "c5selector.h"
#include "c5checkbox.h"
#include "c5user.h"
#include "c5printrecipta4.h"
#include "ce5goods.h"
#include "c5storedraftwriter.h"
#include "threadsendmessage.h"
#include <QClipboard>

#define col_uuid 0
#define col_checkbox 1
#define col_goods_code 2
#define col_store 3
#define col_barcode 4
#define col_name 5
#define col_qty 6
#define col_unit 7
#define col_price 8
#define col_amount 9
#define col_discount_type 10
#define col_discount_value 11
#define col_discount_amount 12
#define col_grandtotal 13

C5SaleDoc::C5SaleDoc(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5SaleDoc)
{
    ui->setupUi(this);
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 50, 0, 150, 200, 300, 80, 80, 80, 80, 0, 0, 0, 0);
    ui->cbCurrency->setDBValues(dbParams, "select f_id, f_name from e_currency");
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    ui->cbStorage->setDBValues(dbParams, "select f_id, f_name from c_storages order by 2");
    ui->cbStorage->setCurrentIndex(-1);
    ui->cbHall->setDBValues(dbParams, "select f_id, f_name from h_halls order by 2");
    ui->cbHall->setCurrentIndex(ui->cbHall->findData(__c5config.getValue(param_default_hall).toInt()));
    ui->cbStorage->setCurrentIndex(ui->cbStorage->findData(__c5config.getValue(param_default_store).toInt()));
    fOpenedFromDraft = false;
    connect(ui->leUuid, &C5LineEdit::doubleClicked, this, &C5SaleDoc::uuidDoubleClicked);
}

C5SaleDoc::~C5SaleDoc()
{
    delete ui;
}

void C5SaleDoc::setMode(int mode)
{
    fMode = mode;
    switch (fMode) {
    case 1:
        break;
    case 2:
        break;
    }
}

bool C5SaleDoc::reportHandler(const QString &handleId, const QVariant &data)
{
    if (handleId == REPORT_HANDLER_SALE_DOC_OPEN_DRAFT) {
        if (!openDraft(data.toString())) {
            return false;
        }
    }
    return true;
}

QToolBar *C5SaleDoc::toolBar()
{
    if (!fToolBar) {
        createToolBar();
        fActionSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDataChanges()));
        fToolBar->addAction(QIcon(":/AS.png"), tr("Export to AS\r\n invoice"), this, SLOT(createInvoiceAS()));
        fToolBar->addAction(QIcon(":/AS.png"), tr("Export to AS\r\n retail"), this, SLOT(createRetailAS()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printSale()));
    }
    return fToolBar;
}

void C5SaleDoc::printSale()
{
    C5PrintReciptA4 p(fDBParams, ui->leUuid->text(), this);
    QString err;
    p.print(err);
    if (!err.isEmpty()) {
        C5Message::error(err);
    }
}

void C5SaleDoc::createInvoiceAS()
{
    C5Message::error(tr("ArmSoft not configured"));
}

void C5SaleDoc::createRetailAS()
{
    C5Message::error(tr("ArmSoft not configured"));
}

void C5SaleDoc::messageResult(QJsonObject jo)
{
    sender()->deleteLater();
    if (jo["status"].toInt() > 0) {
        C5Message::error(jo["data"].toString());
    }
}

void C5SaleDoc::saveDataChanges()
{
    QString err;
    if (ui->leDate->date().isValid() == false) {
        err += tr("Date is not valid") + "<br>";
    }
    if (!ui->cbHall->currentData().isValid()) {
        err += tr("Hall is not valid") + "<br>";
    }
    if (!ui->cbCashDesk->currentData().isValid()) {
        err += tr("Cash desk is not valid") + "<br>";
    }
    if (ui->cbStorage->currentData().toInt() < 1) {
        err += tr("Storage is not defined");
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->comboBox(i, col_store)->currentData().isValid() == false) {
            err += tr("Store not defined on row: ") + QString::number(i + 1) + "<br>";
        }
        if (ui->tblGoods->lineEdit(i, col_qty)->getDouble() < 0.001) {
            err += tr("Quantity not defined on row: ") + QString::number(i + 1) + "<br>";
        }
    }
    if (ui->leCash->getDouble()
            + ui->leCard->getDouble()
            + ui->lePrepaid->getDouble()
            + ui->leDebt->getDouble()
            + ui->leBankTransfer->getDouble()
            < ui->leGrandTotal->getDouble()) {
        err += tr("Incomplete payment") + "<br>";
    }
    if (err.isEmpty() == false) {
        C5Message::error(err);
        return;
    }
    QString uuid = ui->leUuid->text();
    C5Database db(fDBParams);
    db.startTransaction();

    bool isnew = fOpenedFromDraft || ui->leUuid->isEmpty();
    int hallid;
    QString prefix;
    db[":f_id"] = ui->cbHall->currentData();
    db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
    if (db.nextRow()) {
        hallid = db.getInt(0);
        prefix = db.getString(1);
        db[":f_counter"] = db.getInt(0);
        db.update("h_halls", where_id(ui->cbHall->currentData().toInt()));
    } else {
        C5Message::error(tr("No hall with id") + "<br>" + ui->cbHall->currentText());
        return;
    }
    db.commit();
    db.startTransaction();

    db[":f_hallid"] = hallid;
    db[":f_prefix"] = prefix;
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_hall"] = ui->cbHall->currentData();
    db[":f_table"] = ui->cbCashDesk->currentData();
    db[":f_dateclose"] = QDate::currentDate();
    db[":f_timeclose"] = QTime::currentTime();
    db[":f_datecash"] = ui->leDate->date();
    db[":f_staff"] = __user->id();
    db[":f_comment"] = ui->leComment->text();
    db[":f_print"] = 0;
    db[":f_amounttotal"] = ui->leGrandTotal->getDouble();
    db[":f_amountcash"] = ui->leCash->getDouble();
    db[":f_amountcard"] = ui->leCard->getDouble();
    db[":f_amountprepaid"] = ui->lePrepaid->getDouble();
    db[":f_amountbank"] = ui->leBankTransfer->getDouble();
    db[":f_amountother"] = 0;
    db[":f_amountservice"] = 0;
    db[":f_amountdiscount"] = 0;
    db[":f_amountdebt"] = ui->leDebt->getDouble();
    db[":f_servicefactor"] = 0;
    db[":f_discountfactor"] = 0;
    db[":f_creditcardid"] = 0;
    db[":f_otherid"] = ui->leDebt->getDouble();
    db[":f_shift"] = 1;
    db[":f_source"] = 2;
    db[":f_saletype"] = fMode;
    db[":f_partner"] = fPartner.id ;
    db[":f_currentstaff"] = 0;
    db[":f_guests"] = 0;
    db[":f_precheck"] = 0;
    db[":f_amountidram"] = 0;
    db[":f_amountpayx"] = 0;
    db[":f_cash"] = ui->leCountCash->getDouble();
    db[":f_change"] = ui->leChange->getDouble();
    if (ui->leUuid->isEmpty() || fOpenedFromDraft) {
        if (fOpenedFromDraft) {
            db[":f_id"] = uuid;
            db[":f_dateopen"] = ui->leDate->date();
            db[":f_timeopen"] = QTime::fromString(ui->leTime->text(), FORMAT_TIME_TO_STR);
        } else {
            uuid = db.uuid();
            db[":f_id"] = uuid;
        }
        db.insert("o_header");
    } else {
        db.update("o_header", "f_id", ui->leUuid->text());
    }

    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_header"]  = uuid;
        db[":f_body"] = 0;
        db[":f_store"] = ui->cbStorage->currentData();
        db[":f_goods"] = ui->tblGoods->getInteger(i, col_goods_code);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        db[":f_total"] = ui->tblGoods->getDouble(i, col_grandtotal);
        db[":f_tax"] = 0;
        db[":f_sign"] = 1;
        db[":f_taxdept"] = "";
        db[":f_row"] = i;
        db[":f_discountfactor"] = 0;
        db[":f_discountmode"] = 0;
        db[":f_discountamount"] = 0;
        db[":f_return"] = 0;
        db[":f_returnfrom"] = "";
        QString guuid;
        if (ui->tblGoods->getString(i, col_uuid).isEmpty()) {
            guuid = db.uuid();
            db[":f_id"] = guuid;
            db[":f_storerec"] = guuid;
            db.insert("o_goods");
            ui->tblGoods->setString(i, col_uuid, guuid);
        } else {
            db.update("o_goods", "f_id", guuid);
        }
    }

    db[":f_guests"] = 0;
    db[":f_splitted"] = 0;
    db[":f_deliveryman"] = 0;
    db[":f_currency"] = 0;
    if (isnew) {
        db[":f_id"] = uuid;
        db.insert("o_header_options");
    } else {
        db.update("o_header_options", "f_id", uuid);
    }

    db[":f_1"] = 0;
    db[":f_2"] = 0;
    db[":f_3"] = 0;
    db[":f_4"] = 0;
    db[":f_5"] = 0;
    if (isnew) {
        db[":f_id"] = uuid;
        db.insert("o_header_flags");
    } else {
        db.update("o_header_flags", "f_id", uuid);
    }

    if (fOpenedFromDraft) {
        db[":f_id"] = uuid;
        db[":f_state"] = 2;
        db.exec("update o_draft_sale set f_state=:f_state where f_id=:f_id");
    }

    QSet<int> usedStores;
    QStringList outDocIds;
    QStringList inDocsIds;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->comboBox(i, col_store)->currentData().toInt() == ui->cbStorage->currentData().toInt()) {
            continue;
        }
        usedStores.insert(ui->tblGoods->comboBox(i, col_store)->itemData(i).toInt());
    }

    //Output of storage not equal to current storage
    QMap<QString, QString> outInPrices;
    C5StoreDraftWriter dw(db);
    for (int store: usedStores) {
        QString outStoreDocComment = QString("%1 %2%3").arg(tr("Movement of sale"), prefix, QString::number(hallid));
        QString outStoreDocId;
        QString outStoredocUserNum;
        outStoredocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, store, true, 0);

        QString inStoreDocComment = QString("%1 %2%3, %4").arg(tr("Input for of sale"), prefix, QString::number(hallid), outStoredocUserNum);
        QString inStoreDocId;
        QString inStoredocUserNum;
        inStoredocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, store, true, 0);
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if (ui->tblGoods->comboBox(i, col_store)->itemData(i).toInt() != store) {
                continue;
            }
            if (outStoreDocId.isEmpty()) {
                dw.writeAHeader(outStoreDocId, outStoredocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, __user->id(), QDate::currentDate(),
                                     QDate::currentDate(), QTime::currentTime(), 0, 0, outStoreDocComment, 0, ui->cbCurrency->currentData().toInt());
                dw.writeAHeaderStore(outStoreDocId, __user->id(), __user->id(), "", QDate(), 0,
                                     ui->tblGoods->comboBox(i, col_store)->currentData().toInt(),
                                     1, "", 0, 0, uuid);
                outDocIds.append(outStoreDocId);

                dw.writeAHeader(inStoreDocId, inStoredocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_INPUT, __user->id(), QDate::currentDate(),
                                     QDate::currentDate(), QTime::currentTime(), 0, 0, inStoreDocComment, 0, ui->cbCurrency->currentData().toInt());
                dw.writeAHeaderStore(inStoreDocId, __user->id(), __user->id(), "", QDate(),
                                     ui->cbStorage->currentData().toInt(), 0,
                                     1, "", 0, 0, uuid);
                inDocsIds.append(inStoreDocId);
            }
            QString outDraftid, inDraftId;
            dw.writeAStoreDraft(outDraftid, outStoreDocId,
                                ui->tblGoods->comboBox(i, col_store)->currentData().toInt(), -1,
                                ui->tblGoods->getInteger(i, col_goods_code), ui->tblGoods->lineEdit(i, col_qty)->getDouble(),
                                0, 0, DOC_REASON_SALE, outDraftid, i + 1, "");
            dw.writeAStoreDraft(inDraftId, inStoreDocId,
                                ui->cbStorage->currentData().toInt(), 1,
                                ui->tblGoods->getInteger(i, col_goods_code), ui->tblGoods->lineEdit(i, col_qty)->getDouble(),
                                0, 0, DOC_REASON_SALE, inDraftId, i + 1, ui->tblGoods->comboBox(i, col_store)->currentText());
            outInPrices[inDraftId] = outDraftid;
        }
    }

    for (const QString &id: outDocIds) {
        //WRITE OTHER OUTPUT
        if (!dw.writeOutput(id, err)) {
            db.rollback();
            C5Message::error(err + "#1");
            return;
        }
        dw.updateField("a_header", "f_state", DOC_STATE_SAVED, "f_id", id);
    }
    for (QMap<QString, QString>::const_iterator it = outInPrices.constBegin(); it != outInPrices.constEnd(); it++) {
        db[":f_id"] = it.value();
        db.exec("select f_price from a_store_draft where f_id=:f_id");
        if (db.nextRow()) {
            db[":f_id"] = it.key();
            db[":f_price"] = db.getDouble("f_price");
            db.exec("update a_store_draft set f_price=:f_price, f_total=:f_price*f_qty where f_id=:f_id");
        } else {
            db.rollback();
            C5Message::error(tr("Error in document #1"));
            return;
        }
    }
    for (const QString &id: inDocsIds) {
        //WRITE OTHER OUTPUT
        if (!dw.writeInput(id, err)) {
            db.rollback();
            C5Message::error(err + "#2");
            return;
        }

        dw.updateField("a_header", "f_state", DOC_STATE_SAVED, "f_id", id);
    }
    //END OF OUTPUT FOR OTHER STORAGES


    //OUTPUT OF STORAGE
    QString storeDocComment = QString("%1 %2%3").arg(tr("Output of sale"), prefix, QString::number(hallid));
    QString storeDocId;
    QString storedocUserNum;
    storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, ui->cbStorage->currentData().toInt(), true, 0);
    dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, __user->id(), QDate::currentDate(),
                         QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, ui->cbCurrency->currentData().toInt());
    dw.writeAHeaderStore(storeDocId, __user->id(), __user->id(), "", QDate(), 0,
                         ui->cbStorage->currentData().toInt(),
                         1, "", 0, 0, uuid);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QString draftid;
        dw.writeAStoreDraft(draftid, storeDocId,
                            ui->cbStorage->currentData().toInt(), -1,
                            ui->tblGoods->getInteger(i, col_goods_code), ui->tblGoods->lineEdit(i, col_qty)->getDouble(),
                            0, 0, DOC_REASON_SALE, draftid, i + 1, "");
        db[":f_storerec"] = draftid;
        db.update("o_goods", "f_id", ui->tblGoods->getString(i, col_uuid));
    }
    if (!dw.writeOutput(storeDocId, err)) {
        db.rollback();
        C5Message::error(err + "#3");
        return;
    }
    dw.updateField("a_header", "f_state", DOC_STATE_SAVED, "f_id", storeDocId);
    //END OUTPUT OF STORAGE

    db.commit();

    ui->leUuid->setText(uuid);
    ui->leDocnumber->setText(QString("%1%2").arg(prefix, QString::number(hallid)));
    fOpenedFromDraft = false;

    db[":f_order"] = ui->leUuid->text();
    db.exec("delete from b_clients_debts where f_order=:f_order");
    if (fPartner.id.toInt() > 0) {
        db[":f_costumer"] = fPartner.id;
        db[":f_order"] = ui->leUuid->text();
        db[":f_amount"] = ui->leGrandTotal->getDouble();
        db[":f_date"] = ui->leDate->date();
        db[":f_currency"] = 1;
        db[":f_source"] = 1;
        db.insert("b_clients_debts", false);
    }

    if (isnew) {
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            QJsonObject jo;
            jo["action"] = 1; //MSG_GOODS_RESERVE;
            jo["goods"] = ui->tblGoods->getInteger(i, col_goods_code);
            jo["qty"] = ui->tblGoods->getDouble(i, col_qty);
            jo["goodsname"] = ui->tblGoods->getString(i, col_name);
            jo["scancode"] = ui->tblGoods->getString(i, col_barcode);
            jo["unit"] = ui->tblGoods->getString(i, col_unit);
            jo["usermessage"] = tr("Online shop") + " " + ui->leDocnumber->text();
            jo["enddate"] = ui->leDate->date().addDays(7).toString(FORMAT_DATE_TO_STR);
            QJsonDocument jdoc(jo);
            auto *t = new ThreadSendMessage();
            connect(t, SIGNAL(result(QJsonObject)), this, SLOT(messageResult(QJsonObject)));
            t->send(ui->tblGoods->comboBox(i, col_store)->currentData().toInt(),
                    jdoc.toJson(QJsonDocument::Compact));
            if (ui->tblGoods->comboBox(i, col_store)->currentData().toInt() != ui->cbStorage->currentData().toInt()) {
                jo["usermessage"] = ui->tblGoods->comboBox(i, col_store)->currentText() + " " + ui->leDocnumber->text();
                jdoc = QJsonDocument(jo);
                auto *t = new ThreadSendMessage();
                connect(t, SIGNAL(result(QJsonObject)), this, SLOT(messageResult(QJsonObject)));
                t->send(ui->cbStorage->currentData().toInt(),
                        jdoc.toJson(QJsonDocument::Compact));
            }
        }
    }
    fActionSave->setEnabled(false);
    C5Message::info(tr("Saved"));
}

void C5SaleDoc::uuidDoubleClicked()
{
    qApp->clipboard()->setText(ui->leUuid->text());
}

void C5SaleDoc::on_PriceTextChanged(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int r, c;
    if (ui->tblGoods->findWidget(l, r, c)) {
        ui->tblGoods->lineEdit(r, col_amount)->setDouble(str_float(arg1) * ui->tblGoods->lineEdit(r, col_qty)->getDouble());
        countGrandTotal();
    }
}

void C5SaleDoc::on_QtyTextChanged(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int r, c;
    if (ui->tblGoods->findWidget(l, r, c)) {
        ui->tblGoods->lineEdit(r, col_amount)->setDouble(str_float(arg1) * ui->tblGoods->lineEdit(r, col_price)->getDouble());
        ui->tblGoods->setDouble(r, col_grandtotal, ui->tblGoods->lineEdit(r, col_amount)->getDouble());
        countGrandTotal();
    }
}

void C5SaleDoc::on_TotalTextChanged(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int r, c;
    if (ui->tblGoods->findWidget(l, r, c)) {
        if (ui->tblGoods->lineEdit(r, col_qty)->getDouble() > 0.0001) {
            ui->tblGoods->lineEdit(r, col_price)->setDouble(str_float(arg1) / ui->tblGoods->lineEdit(r, col_qty)->getDouble());
            countGrandTotal();
        }
    }
}

void C5SaleDoc::on_leCmd_returnPressed()
{
    QString code = ui->leCmd->text().replace("?", "").replace(";", "");
    ui->leCmd->clear();
    C5Database db(fDBParams);
    db[":f_scancode"] = code;
    db.exec("select f_id from c_goods where f_scancode=:f_scancode");
    if (db.nextRow()) {
        addGoods(db.getInt(0), db);
        return;
    }
}

void C5SaleDoc::addGoods(int goodsId, C5Database &db)
{
    QString priceField;
    switch (fMode) {
    case PRICEMODE_RETAIL:
        priceField = "f_price1";
        break;
    case PRICEMODE_WHOSALE:
        priceField = "f_price2";
        break;
    }
    db[":f_id"] = goodsId;
    db[":f_currency"] = ui->cbCurrency->currentData();
    db.exec(QString("select g.*, gu.f_name as f_unitname, gpr.%1 as f_price "
            "from c_goods g "
            "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
            "left join c_units gu on gu.f_id=g.f_unit "
            "where g.f_id=:f_id and gpr.f_currency=:f_currency").arg(priceField));
    if (db.nextRow()) {
        addGoods(ui->cbStorage->currentData().toInt(), goodsId, db.getString("f_scancode"), db.getString("f_name"), db.getString("f_unitname"), 0, db.getDouble("f_price"));
    } else {
        C5Message::error(tr("Invalid goods id"));
    }
}

void C5SaleDoc::addGoods(int store, int goodsId, const QString &barcode, const QString &name, const QString &unitname, double qty, double price)
{
    int r = ui->tblGoods->addEmptyRow();
    ui->tblGoods->setString(r, col_uuid, "");
    ui->tblGoods->createCheckbox(r, col_checkbox);
    ui->tblGoods->setInteger(r, col_goods_code, goodsId);
    C5ComboBox *cs = ui->tblGoods->createComboBox(r, col_store);
    ui->tblGoods->setString(r, col_barcode, barcode);
    ui->tblGoods->setString(r, col_name, name);
    ui->tblGoods->createLineEdit(r, col_qty)->setDouble(qty);
    ui->tblGoods->setString(r, col_unit, unitname);
    ui->tblGoods->createLineEdit(r, col_price)->setDouble(price);
    ui->tblGoods->createLineEdit(r, col_amount)->setDouble(price*qty);
    ui->tblGoods->setString(r, col_discount_type, "");
    ui->tblGoods->setString(r, col_discount_value, "");
    ui->tblGoods->setDouble(r, col_discount_amount, 0);
    ui->tblGoods->setDouble(r, col_grandtotal, qty * price);
    connect(ui->tblGoods->lineEdit(r, col_qty), &C5LineEdit::textEdited, this, &C5SaleDoc::on_QtyTextChanged);
    connect(ui->tblGoods->lineEdit(r, col_price), &C5LineEdit::textEdited, this, &C5SaleDoc::on_PriceTextChanged);
    connect(ui->tblGoods->lineEdit(r, col_amount), &C5LineEdit::textEdited, this, &C5SaleDoc::on_TotalTextChanged);
    for (int i = 0; i < ui->cbStorage->count(); i++) {
        cs->addItem(ui->cbStorage->itemText(i), ui->cbStorage->itemData(i));
    }
    cs->setCurrentIndex(cs->findData(store));
    countGrandTotal();
}

void C5SaleDoc::countGrandTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->getDouble(i, col_grandtotal);
    }
    ui->leGrandTotal->setDouble(total);
}

bool C5SaleDoc::openDraft(const QString &id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    if (!fDraftSale.getRecord(db)) {
        C5Message::error(tr("Invalid draft document id"));
        return false;
    }
    if (fDraftSale.state != 1) {
        C5Message::error(tr("This draft not editable"));
        return false;
    }
    fPartner.queryRecordOfId(db, fDraftSale.partner);
    ui->leTaxpayerName->setText(fPartner.taxName);
    ui->leTaxpayerId->setText(fPartner.taxCode);

    ui->leDate->setDate(fDraftSale.date);
    ui->leTime->setText(fDraftSale.time.toString(FORMAT_TIME_TO_STR));
    ui->leComment->setText(fDraftSale.comment);
    ui->leUuid->setText(fDraftSale.id);
    QString priceField = "f_price1";
    db[":f_header"] = id;
    db[":f_state"] = 1;
    db[":f_currency"] = ui->cbCurrency->currentData();
    db.exec(QString("select dsb.f_store, dsb.f_qty, g.*, gu.f_name as f_unitname, gpr.%1 as f_price "
            "from o_draft_sale_body dsb "
            "left join c_goods g on g.f_id=dsb.f_goods "
            "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
            "left join c_units gu on gu.f_id=g.f_unit "
            "where dsb.f_header=:f_header and gpr.f_currency=:f_currency "
            "and dsb.f_state=:f_state and dsb.f_qty>0 ").arg(priceField));
    while (db.nextRow()) {
        addGoods(db.getInt("f_store"), db.getInt("f_id"), db.getString("f_scancode"), db.getString("f_name"),
                 db.getString("f_unitname"), db.getDouble("f_qty"), db.getDouble("f_price"));
    }
    fOpenedFromDraft = true;

    switch (fDraftSale.payment) {
    case 1:
        ui->leCash->setDouble(ui->leGrandTotal->getDouble());
        break;
    case 2:
        ui->leCard->setDouble(ui->leGrandTotal->getDouble());
        break;
    case 3:
        ui->leBankTransfer->setDouble(ui->leGrandTotal->getDouble());
        break;
    case 6:
        ui->leCard->setDouble(ui->leGrandTotal->getDouble());
        break;
    case 7:
        ui->leDebt->setDouble(ui->leGrandTotal->getDouble());
        break;
    }

    return true;
}

void C5SaleDoc::on_btnAddGoods_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods, vals, 3)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    C5Database db(fDBParams);
    addGoods(vals.at(1).toInt(), db);
}

void C5SaleDoc::on_btnRemoveGoods_clicked()
{
    for (int i = ui->tblGoods->rowCount() - 1; i > -1; i--) {
        if (ui->tblGoods->checkBox(i, col_checkbox)->isChecked()) {
            ui->tblGoods->removeRow(i);
        }
    }
    countGrandTotal();
}

void C5SaleDoc::on_btnEditGoods_clicked()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (ui->tblGoods->getInteger(row, col_goods_code) == 0) {
        return;
    }
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    ep->setId(ui->tblGoods->getInteger(row, col_goods_code));
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot change goods without code"));
            return;
        }
        ui->tblGoods->setData(row, col_goods_code, data.at(0)["f_id"]);
        ui->tblGoods->setData(row, col_name, data.at(0)["f_name"]);
        ui->tblGoods->setData(row, col_unit, data.at(0)["f_unitname"]);
        ui->tblGoods->lineEdit(row, col_qty)->setFocus();
    }
    delete e;
}

void C5SaleDoc::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot add goods without code"));
            return;
        }
        C5Database db(fDBParams);
        addGoods(data.at(0)["f_id"].toInt(), db);
    }
    delete e;
    countGrandTotal();
}

void C5SaleDoc::on_cbHall_currentIndexChanged(int index)
{
    ui->cbCashDesk->clear();
    ui->cbCashDesk->setDBValues(fDBParams, QString("select f_id, f_name from h_tables where f_hall=%1").arg(ui->cbHall->itemData(index).toInt()));
    if (ui->cbHall->itemData(index).toInt() == __c5config.getValue(param_default_hall).toInt()) {
        ui->cbCashDesk->setCurrentIndex(ui->cbCashDesk->findData(__c5config.getValue(param_default_table).toInt()));
    }
}

void C5SaleDoc::on_btnSearchTaxpayer_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValue(fDBParams, cache_goods_partners, values)) {
        return;
    }
    if (values.count() == 0) {
        return;
    }
    C5Database db(fDBParams);
    fPartner.queryRecordOfId(db, values.at(0));
    ui->leTaxpayerName->setText(fPartner.taxName);
    ui->leTaxpayerId->setText(fPartner.taxCode);
}


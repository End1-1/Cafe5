#include "storeinputdocument.h"
#include "ui_storeinputdocument.h"
#include "c5checkbox.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5user.h"
#include "c5storedraftwriter.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"
#include "ce5goods.h"

#define col_checkbox 0
#define col_uuid 1
#define col_goodsid 2
#define col_scancode 3
#define col_group 4
#define col_name 5
#define col_qty 6
#define col_qtybox 7
#define col_unit 8
#define col_qtyinbox 9
#define col_price 10
#define col_total 11
#define col_stock 12
#define col_saleprice 13
#define col_valid 14

StoreInputDocument::StoreInputDocument(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::StoreInputDocument)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 30, 0, 0, 120, 120, 250, 80, 80, 0, 0, 80, 80, 80, 80, 80);
    C5Cache::cache(fDBParams, cache_goods)->refresh();
}

StoreInputDocument::~StoreInputDocument()
{
    delete ui;
}

QToolBar *StoreInputDocument::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fDraft = fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDoc()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, SLOT(newDoc()));
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
        fToolBar->addAction(QIcon(":/excel.png"), tr("Export\nto Excel"),  this, SLOT(exportToExcel()));
        fDraft->setEnabled(false);
    }
    return fToolBar;
}

void StoreInputDocument::qtyChanged(const QString &arg1)
{
    int row, col;
    if (!ui->tbl->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    disconnectSlotSignal(row);
    ui->tbl->lineEdit(row, col_qtybox)->setDouble(ui->tbl->lineEdit(row, col_qty)->getDouble() / ui->tbl->getDouble(row, col_qtyinbox));
    ui->tbl->lineEdit(row, col_total)->setDouble(ui->tbl->lineEdit(row, col_qtybox)->getDouble() * ui->tbl->lineEdit(row, col_price)->getDouble());
    connectSlotSignal(row);
    countTotal();
}

void StoreInputDocument::qtyBoxChanged(const QString &arg1)
{
    int row, col;
    if (!ui->tbl->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    disconnectSlotSignal(row);
    ui->tbl->lineEdit(row, col_qty)->setDouble(ui->tbl->lineEdit(row, col_qtybox)->getDouble() * ui->tbl->getDouble(row, col_qtyinbox));
    ui->tbl->lineEdit(row, col_total)->setDouble(ui->tbl->lineEdit(row, col_qtybox)->getDouble() * ui->tbl->lineEdit(row, col_price)->getDouble());
    connectSlotSignal(row);
    countTotal();
}

void StoreInputDocument::priceChanged(const QString &arg1)
{
    int row, col;
    if (!ui->tbl->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    disconnectSlotSignal(row);
    ui->tbl->lineEdit(row, col_total)->setDouble(ui->tbl->lineEdit(row, col_qtybox)->getDouble() * ui->tbl->lineEdit(row, col_price)->getDouble());
    connectSlotSignal(row);
    countTotal();
}

void StoreInputDocument::totalChanged(const QString &arg1)
{
    int row, col;
    if (!ui->tbl->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    if (ui->tbl->lineEdit(row, col_qtybox)->getDouble() < 0.001) {
        return;
    }
    ui->tbl->lineEdit(row, col_price)->setDouble(ui->tbl->lineEdit(row, col_total)->getDouble() / ui->tbl->lineEdit(row, col_qtybox)->getDouble());
    countTotal();
}

void StoreInputDocument::on_btnDeleteRow_clicked()
{
    for (int i = ui->tbl->rowCount() - 1; i > -1; i--) {
        if (ui->tbl->checkBox(i, col_checkbox)->isChecked()) {
            ui->tbl->removeRow(i);
        }
    }
    countTotal();
}

void StoreInputDocument::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        total += ui->tbl->lineEdit(i, col_total)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

int StoreInputDocument::newEmptyRow()
{
    int row = ui->tbl->rowCount();
    ui->tbl->setRowCount(row + 1);
    ui->tbl->createCheckbox(row, col_checkbox);
    ui->tbl->createLineEdit(row, col_qty)->setText("");
    ui->tbl->createLineEdit(row, col_qtybox)->setText("");
    ui->tbl->createLineEdit(row, col_price);
    ui->tbl->createLineEdit(row, col_total);
    ui->tbl->createWidget<C5DateEdit>(row, col_valid);
    return row;
}

void StoreInputDocument::connectSlotSignal(int row)
{
    connect(ui->tbl->lineEdit(row, col_qty), &C5LineEdit::textChanged, this, &StoreInputDocument::qtyChanged);
    connect(ui->tbl->lineEdit(row, col_qtybox), &C5LineEdit::textChanged, this, &StoreInputDocument::qtyBoxChanged);
    connect(ui->tbl->lineEdit(row, col_price), &C5LineEdit::textChanged, this, &StoreInputDocument::priceChanged);
    connect(ui->tbl->lineEdit(row, col_total), &C5LineEdit::textChanged, this, &StoreInputDocument::totalChanged);
}

void StoreInputDocument::disconnectSlotSignal(int row)
{
    disconnect(ui->tbl->lineEdit(row, col_qty), &C5LineEdit::textChanged, this, &StoreInputDocument::qtyChanged);
    disconnect(ui->tbl->lineEdit(row, col_qtybox), &C5LineEdit::textChanged, this, &StoreInputDocument::qtyBoxChanged);
    disconnect(ui->tbl->lineEdit(row, col_price), &C5LineEdit::textChanged, this, &StoreInputDocument::priceChanged);
    disconnect(ui->tbl->lineEdit(row, col_total), &C5LineEdit::textChanged, this, &StoreInputDocument::totalChanged);
}

void StoreInputDocument::on_btnAddRow_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods, vals, 3)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    ui->leScancode->setText(vals.at(5).toString());
    on_leScancode_returnPressed();
}

void StoreInputDocument::on_toolButton_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods_store, vals, 2)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    ui->leStore->setProperty("f_id", vals.at(1));
    ui->leStore->setText(vals.at(2).toString());

    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    ui->leDocnum->setPlaceholderText(dw.storeDocNum(DOC_TYPE_STORE_INPUT, vals.at(1).toInt(), false, 0));
}

void StoreInputDocument::saveDoc()
{
    QString err;
    if (ui->tbl->rowCount() == 0) {
        err += tr("Empty document") + "\r\n";
    }
    if (ui->leStore->property("f_id").toInt() == 0) {
        err += tr("Store not defined") + "\r\n";
    }
    if (ui->lePartner->property("f_id").toInt() == 0) {
        err += tr("Partner not defined") + "\r\n";
    }
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->lineEdit(i, col_qty)->getDouble() > 10000 || ui->tbl->lineEdit(i, col_qty)->getDouble() < 0.001) {
            err += tr("Check quantity");
            break;
        }
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return;
    }
    if (ui->leDocnum->text().isEmpty()) {
        ui->leDocnum->setText(ui->leDocnum->placeholderText());
    }
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    db.startTransaction();
    if (ui->leDocnum->text().isEmpty()) {
        ui->leDocnum->setText(dw.storeDocNum(DOC_TYPE_STORE_INPUT, ui->leStore->property("f_id").toInt(), true, 0));
    }
    db[":f_userid"] = ui->leDocnum->text();
    db[":f_state"] = DOC_STATE_SAVED;
    db[":f_type"]  = DOC_TYPE_STORE_INPUT;
    db[":f_operator"] = __user->id();
    db[":f_date"] = ui->leDate->date();
    db[":f_createdate"] = QDate::currentDate();
    db[":f_createtime"] = QTime::currentTime();
    db[":f_partner"] = ui->lePartner->property("f_id");
    db[":f_amount"] = ui->leTotal->getDouble();
    db[":f_comment"] = ui->leComment->text();
    db[":f_paid"] = 0;
    db[":f_currency"] = 1;
    if (ui->leDocnum->property("f_id").toString().isEmpty()) {
        ui->leDocnum->setProperty("f_id", db.uuid());
        db[":f_id"] = ui->leDocnum->property("f_id");
        db.insert("a_header", false);
    } else {
        db.update("a_header", "f_id", ui->leDocnum->property("f_id"));
    }

    db[":f_document"] = ui->leDocnum->property("f_id");
    db.exec("delete from a_store_draft where f_document=:f_document");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->setString(i, col_uuid, db.uuid());
        db[":f_id"] = ui->tbl->getString(i, col_uuid);
        db[":f_document"] = ui->leDocnum->property("f_id");
        db[":f_store"] = ui->leStore->property("f_id");
        db[":f_type"] = 1;
        db[":f_goods"] = ui->tbl->getInteger(i, col_goodsid);
        db[":f_qty"] = ui->tbl->getDouble(i, col_qtybox);
        db[":f_price"] = ui->tbl->getDouble(i, col_price);
        db[":f_total"] = ui->tbl->getDouble(i, col_total);
        db[":f_reason"] = DOC_REASON_INPUT;
        db[":f_base"] = ui->tbl->getString(i, col_uuid);
        db[":f_row"] = i;
        db.insert("a_store_draft", false);

        db[":f_id"] = ui->tbl->getString(i, col_uuid);
        db[":f_document"] = ui->leDocnum->property("f_id");
        db[":f_store"] = ui->leStore->property("f_id");
        db[":f_type"] = 1;
        db[":f_goods"] = ui->tbl->getInteger(i, col_goodsid);
        db[":f_qty"] = ui->tbl->getDouble(i, col_qtybox);
        db[":f_price"] = ui->tbl->getDouble(i, col_price);
        db[":f_total"] = ui->tbl->getDouble(i, col_total);
        db[":f_reason"] = DOC_REASON_INPUT;
        db[":f_base"] = ui->tbl->getString(i, col_uuid);
        db[":f_basedoc"] = ui->leDocnum->property("f_id");
        db.insert("a_store", false);

        db[":f_id"] = ui->tbl->getString(i, col_uuid);
        db[":f_document"] = ui->leDocnum->property("f_id");
        db[":f_date"] = static_cast<C5DateEdit*>(ui->tbl->cellWidget(i, col_valid))->date();
        db.insert("a_store_valid", false);

        db[":f_lastvaliddate"] = static_cast<C5DateEdit*>(ui->tbl->cellWidget(i, col_valid))->date();
        db.update("c_goods_option", "f_id", ui->tbl->getInteger(i, col_goodsid));

        db[":f_lastinputprice"] = ui->tbl->getDouble(i, col_price);
        db.update("c_goods", "f_id", ui->tbl->getInteger(i, col_goodsid));
    }
    db.commit();
    fSave->setEnabled(false);
    C5Cache::cache(fDBParams, cache_goods)->refresh();
}

bool StoreInputDocument::openDoc(const QString &id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from a_header where f_id=:f_id");
    if (db.nextRow() == false) {
        return false;
    }
    ui->leDate->setDate(db.getDate("f_date"));
    ui->leDocnum->setProperty("f_id", id);
    ui->leDocnum->setText(db.getString("f_userid"));
    ui->lePartner->setProperty("f_id", db.getInt("f_partner"));
    ui->leComment->setText(db.getString("f_comment"));
    ui->leTotal->setDouble(db.getDouble("f_amount"));

    fSave->setEnabled(db.getInt("f_state") != DOC_STATE_SAVED);
    fDraft->setEnabled(db.getInt("f_state") == DOC_STATE_SAVED);

    db[":f_document"] = id;
    db.exec("select d.f_id, d.f_store, d.f_goods, g.f_scancode, gr.f_name as f_groupname, g.f_name as f_goodsname, "
            "d.f_qty, g.f_qtybox, u.f_name as f_unitname, d.f_price, d.f_total "
            "from a_store_draft d "
            "left join c_goods g on g.f_id=d.f_goods "
            "left join c_groups gr on gr.f_id=g.f_group "
            "left join c_units u on u.f_id=g.f_unit "
            "where d.f_document=:f_document ");
    while (db.nextRow()) {
        int row = newEmptyRow();
        ui->tbl->setString(row, col_uuid, db.getString("f_id"));
        ui->tbl->setInteger(row, col_goodsid, db.getInt("f_goods"));
        ui->tbl->setString(row, col_scancode, db.getString("f_scancode"));
        ui->tbl->setString(row, col_group, db.getString("f_groupname"));
        ui->tbl->setString(row, col_name, db.getString("f_goodsname"));
        ui->tbl->setString(row, col_unit, db.getString("f_unitname"));
        ui->tbl->setDouble(row, col_qtyinbox, db.getDouble("f_qtybox"));
        ui->tbl->setDouble(row, col_saleprice, 0);
        ui->tbl->lineEdit(row, col_price)->setDouble(db.getDouble("f_price"));
        ui->tbl->lineEdit(row, col_qty)->setDouble(db.getDouble("f_qtybox") * db.getDouble("f_qty"));
        ui->tbl->lineEdit(row, col_qtybox)->setDouble(db.getDouble("f_qty"));
        ui->tbl->lineEdit(row, col_total)->setDouble(db.getDouble("f_total"));
        connectSlotSignal(row);
        if (ui->leStore->property("f_id").toInt() == 0) {
            ui->leStore->setProperty("f_id", db.getInt("f_store"));
        }
    }

    db[":f_id"] = ui->leStore->property("f_id");
    db.exec("select * from c_storages where f_id=:f_id");
    db.nextRow();
    ui->leStore->setText(db.getString("f_name"));

    db[":f_id"] = ui->lePartner->property("f_id");
    db.exec("select * from c_partners where f_id=:f_id");
    if (db.nextRow()) {
    db.nextRow();
    ui->lePartner->setText(db.getString("f_name"));
    }
}

bool StoreInputDocument::draftDoc()
{
    if (C5Message::question(tr("Make draft?")) != QDialog::Accepted) {
        return false;
    }
    C5Database db(fDBParams);
    db[":f_basedoc"] = ui->leDocnum->property("f_id");
    db.exec("select distinct(s.f_document) as f_document, d.f_date, d.f_userid from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where f_basedoc=:f_basedoc and f_document<>:f_basedoc and s.f_type=-1");
    QString err = "";
    while (db.nextRow()) {
        err += QString("No: %1, %2<br>").arg(db.getString("f_userid")).arg(db.getDate("f_date").toString(FORMAT_DATE_TO_STR));
    }
    if (!err.isEmpty()) {
        err = tr("This order used in next documents") + "<br>" + err;
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return false;
    }
    db.startTransaction();
    db[":f_document"] = ui->leDocnum->property("f_id");
    db.exec("delete from a_store where f_document=:f_document");

    db[":f_state"] = DOC_STATE_DRAFT;
    db.update("a_header", "f_id",  ui->leDocnum->property("f_id"));
    db.commit();
    fSave->setEnabled(true);
    fDraft->setEnabled(false);
    return true;
}

void StoreInputDocument::removeDocument()
{
    if (C5Message::question(tr("Confirm to remove selected documents")) != QDialog::Accepted) {
        return;
    }
    if (C5StoreDoc::removeDoc(fDBParams, ui->leDocnum->property("f_id").toString(), false)) {
       __mainWindow->removeTab(this);
    }
}

void StoreInputDocument::on_btnSetPartner_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods_partners, vals, -1)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    ui->lePartner->setProperty("f_id", vals.at(1));
    ui->lePartner->setText(vals.at(2).toString());
}


void StoreInputDocument::on_leScancode_returnPressed()
{
    C5Database db(fDBParams);
    QString code = ui->leScancode->text();
    db[":f_scancode"] = code;
    db.exec("select gg.f_scancode, gg.f_id, gg.f_name as f_name, gu.f_name as f_unitname, gg.f_saleprice, "
            "gr.f_taxdept, gr.f_adgcode, gg.f_lastinputprice, gr.f_name as f_groupname, gg.f_qtybox, gpr.f_price1 "
            "from c_goods gg  "
            "left join c_groups gr on gr.f_id=gg.f_group "
            "left join c_units gu on gu.f_id=gg.f_unit "
            "left join c_goods_prices gpr on gpr.f_goods=gg.f_id and gpr.f_currency=1 "
            "where gg.f_scancode=:f_scancode");
    ui->leScancode->clear();
    if (db.nextRow()) {
        int row = newEmptyRow();
        int id = db.getInt("f_id");
        ui->tbl->setInteger(row, col_goodsid, id);
        ui->tbl->setString(row, col_scancode, code);
        ui->tbl->setString(row, col_group, db.getString("f_groupname"));
        ui->tbl->setString(row, col_name, db.getString("f_name"));
        ui->tbl->setString(row, col_unit, db.getString("f_unitname"));
        ui->tbl->setDouble(row, col_qtyinbox, db.getInt("f_qtybox"));
        ui->tbl->setDouble(row, col_saleprice, db.getDouble("f_price1"));
        ui->tbl->lineEdit(row, col_price)->setPlaceholderText(float_str(db.getDouble("f_lastinputprice"), 2));
        ui->tbl->lineEdit(row, col_qty)->setFocus();
        connectSlotSignal(row);

        if (ui->leStore->property("f_id").toInt() > 0) {
            C5Database db(fDBParams);
            db[":f_store"] = ui->leStore->property("f_id");
            db[":f_goods"] = id;
            db.exec("select sum(f_qty*f_type) as f_qty from a_store where f_store=:f_store and f_goods=:f_goods");
            if (db.nextRow()) {
                ui->tbl->setDouble(row,  col_stock, db.getDouble("f_qty"));
            }
        }

        db[":f_id"] = id;
        db.exec("select f_lastvaliddate from c_goods_option where f_id=:f_id");
        if (db.nextRow()) {
            QDate date = db.getDate(0);
            if (!date.isValid()) {
                date = QDate::currentDate();
            }
            static_cast<C5DateEdit*>(ui->tbl->cellWidget(row, col_valid))->setDate(date);
        }
    }


}

void StoreInputDocument::on_btnEditGoods_clicked()
{
    int row = ui->tbl->currentRow();
    if (row < 0) {
        return;
    }
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    ep->setId(ui->tbl->getInteger(row, col_goodsid));
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot change goods without code"));
            return;
        }
        ui->tbl->setData(row, col_name, data.at(0)["f_name"]);
        ui->tbl->setData(row, col_unit, data.at(0)["f_unitname"]);
        ui->tbl->setData(row, col_saleprice, data.at(0)["f_saleprice1"]);
    }
    delete e;
}

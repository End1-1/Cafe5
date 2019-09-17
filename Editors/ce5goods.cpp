#include "ce5goods.h"
#include "ui_ce5goods.h"
#include "c5cache.h"
#include "ce5goodsgroup.h"
#include "ce5goodsunit.h"
#include "c5message.h"
#include "c5selector.h"

CE5Goods::CE5Goods(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Goods)
{
    ui->setupUi(this);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group);
    ui->leUnit->setSelector(dbParams, ui->leUnitName, cache_goods_unit);
    ui->leLowLevel->setValidator(new QDoubleValidator(0, 100000, 4));
    ui->tblGoods->setColumnWidths(7, 0, 0, 300, 80, 80, 80, 80);
}

CE5Goods::~CE5Goods()
{
    delete ui;
}

QString CE5Goods::title()
{
    return tr("Goods");
}

QString CE5Goods::table()
{
    return "c_goods";
}

void CE5Goods::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    db[":f_goods"] = id;
    db.exec("select f_code from c_goods_scancode where f_goods=:f_goods");
    while (db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstCodes);
        item->setText(db.getString(0));
        ui->lstCodes->addItem(item);
    }
    db[":f_base"] = id;
    db.exec("select c.f_id, c.f_goods, g.f_name as f_goodsname, c.f_qty, u.f_name as f_unitname, "
            "c.f_price, c.f_qty*c.f_price as f_total "
            "from c_goods_complectation c "
            "left join c_goods g on g.f_id=c.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "where c.f_base=:f_base");
    while (db.nextRow()) {
        int row = addGoodsRow();
        ui->tblGoods->setInteger(row, 0, db.getInt("f_id"));
        ui->tblGoods->setInteger(row, 1, db.getInt("f_goods"));
        ui->tblGoods->setString(row, 2, db.getString("f_goodsname"));
        ui->tblGoods->lineEdit(row, 3)->setDouble(db.getDouble("f_qty"));
        ui->tblGoods->setString(row, 4, db.getString("f_unitname"));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble("f_price"));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble("f_total"));
    }
}

bool CE5Goods::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (!CE5Editor::save(err, data)) {
        return false;
    }
    C5Database db(fDBParams);
    db[":f_goods"] = ui->leCode->text();
    db.exec("delete from c_goods_scancode where f_goods=:f_goods");
    for (int i = 0; i < ui->lstCodes->count(); i++) {
        db[":f_code"] = ui->lstCodes->item(i)->text();
        db.exec("delete from c_goods_scancode where f_code=:f_code");
        db[":f_code"] = ui->lstCodes->item(i)->text();
        db[":f_goods"] = ui->leCode->getInteger();
        db.insert("c_goods_scancode", false);
    }
    if (ui->lstCodes->count() > 0) {
        db[":f_goods"] = ui->leCode->getInteger();
        db.exec("update c_goods_scancode set f_receipt=0 where f_goods=:f_goods");
        db[":f_goods"] = ui->leCode->getInteger();
        db[":f_code"] = ui->lstCodes->item(0)->text();
        db.exec("update c_goods_scancode set f_receipt=1 where f_code=:f_code and f_goods=:f_goods");
    }
    db[":f_base"] = ui->leCode->text();
    db.exec("delete from c_goods_complectation where f_base=:f_base");
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_base"] = ui->leCode->text();
        db[":f_goods"] = ui->tblGoods->getInteger(i, 1);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        db.insert("c_goods_complectation", false);
    }
    return true;
}

void CE5Goods::clear()
{
    CE5Editor::clear();
    ui->lstCodes->clear();
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->leLowLevel->setText("0");
}

void CE5Goods::on_leAddScanCode_returnPressed()
{
    if (ui->leAddScanCode->isEmpty()) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_code"] = ui->leAddScanCode->text();
    db.exec("select gg.f_name, gg.f_id from c_goods_scancode gs left join c_goods gg on gs.f_goods=gg.f_id where gs.f_code=:f_code");
    if (db.nextRow()) {
        if (ui->leCode->getInteger() != db.getInt(1)) {
            if (C5Message::question(tr("Scancode already registered with other goods. Append to current goods?") + "<br>" + db.getString(0)) != QDialog::Accepted) {
                ui->leAddScanCode->clear();
                ui->leAddScanCode->setFocus();
                return;
            }
        }
    }
    QListWidgetItem *item = new QListWidgetItem(ui->lstCodes);
    item->setText(ui->leAddScanCode->text());
    ui->lstCodes->addItem(item);
    ui->leAddScanCode->clear();
    ui->leAddScanCode->setFocus();
}

void CE5Goods::on_btnRemoveScaneCode_clicked()
{
    if (ui->lstCodes->currentRow() < 0) {
        return;
    }
    delete ui->lstCodes->currentItem();
}

void CE5Goods::on_btnNewGroup_clicked()
{
    CE5GoodsGroup *ep = new CE5GoodsGroup(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leGroup->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5Goods::on_btnNewUnit_clicked()
{
    CE5GoodsUnit *ep = new CE5GoodsUnit(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leUnit->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5Goods::on_btnAddGoods_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_goods, vals)) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setString(row, 1, vals.at(0).toString());
    ui->tblGoods->setString(row, 2, vals.at(2).toString());
    ui->tblGoods->setString(row, 4, vals.at(3).toString());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
}

void CE5Goods::on_btnRemoveGoods_clicked()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblGoods->item(row, 2)->text()) != QDialog::Accepted) {
        return;
    }
    ui->tblGoods->removeRow(row);
    countTotal();
}

int CE5Goods::addGoodsRow()
{
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setItem(row, 0, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 1, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 2, new QTableWidgetItem());
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, 3);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 3));
    ui->tblGoods->setItem(row, 4, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, 5);
    lprice->setValidator(new QDoubleValidator(0, 100000000, 3));
    lprice->fDecimalPlaces = 3;
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, 6);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    connect(lqty, SIGNAL(textEdited(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textEdited(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textEdited(QString)), this, SLOT(tblTotalChanged(QString)));
    return row;
}

void CE5Goods::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 6)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

void CE5Goods::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        int row = addGoodsRow();
        ui->tblGoods->setData(row, 1, data.at(0)["f_id"]);
        ui->tblGoods->setData(row, 2, data.at(0)["f_name"]);
        ui->tblGoods->setData(row, 4, data.at(0)["f_unitname"]);
        ui->tblGoods->lineEdit(row, 3)->setFocus();
    }
    delete e;
}

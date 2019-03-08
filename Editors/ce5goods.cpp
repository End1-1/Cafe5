#include "ce5goods.h"
#include "ui_ce5goods.h"
#include "c5cache.h"
#include "c5message.h"

CE5Goods::CE5Goods(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Goods)
{
    ui->setupUi(this);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group);
    ui->leUnit->setSelector(dbParams, ui->leUnitName, cache_goods_unit);
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
    return true;
}

void CE5Goods::clear()
{
    CE5Editor::clear();
    ui->lstCodes->clear();
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

}

void CE5Goods::on_btnNewUnit_clicked()
{

}

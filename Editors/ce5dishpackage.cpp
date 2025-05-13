#include "ce5dishpackage.h"
#include "ui_ce5dishpackage.h"
#include "c5selector.h"
#include "c5combobox.h"
#include "c5cache.h"
#include "c5database.h"
#include "c5message.h"
#include <QMenu>

CE5DishPackage::CE5DishPackage(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishPackage)
{
    ui->setupUi(this);
    ui->tblDishes->setColumnWidths(ui->tblDishes->columnCount(), 0, 0, 300, 80, 80, 200, 200, 80);
    ui->leMenu->setSelector(fDBParams, ui->leMenuName, cache_menu_names);
}

CE5DishPackage::~CE5DishPackage()
{
    delete ui;
}

QString CE5DishPackage::title()
{
    return tr("Dish package");
}

QString CE5DishPackage::table()
{
    return "d_package";
}

void CE5DishPackage::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    db[":f_package"] = id;
    db.exec("select p.f_id, p.f_dish, d.f_name, p.f_qty, p.f_price, p.f_store, p.f_printer, "
            "d.f_cost "
            "from d_package_list p "
            "inner join d_dish d on d.f_id=p.f_dish "
            "where p.f_package=:f_package");
    while (db.nextRow()) {
        int r = ui->tblDishes->addEmptyRow();
        ui->tblDishes->setData(r, 0, db.getValue(0));
        ui->tblDishes->setData(r, 1, db.getValue(1));
        ui->tblDishes->setData(r, 2, db.getValue(2));
        ui->tblDishes->createLineEdit(r, 3)->setDouble(db.getDouble(3));
        connect(ui->tblDishes->lineEdit(r, 3), SIGNAL(textChanged(QString)), this, SLOT(setQty(QString)));
        ui->tblDishes->createLineEdit(r, 4)->setDouble(db.getDouble(4));
        connect(ui->tblDishes->lineEdit(r, 4), SIGNAL(textChanged(QString)), this, SLOT(setPrice(QString)));
        C5ComboBox *cb = ui->tblDishes->createComboBox(r, 5);
        cb->setCache(fDBParams, cache_goods_store);
        cb->setIndexForValue(db.getInt(5));
        cb = ui->tblDishes->createComboBox(r, 6);
        cb->setCache(fDBParams, cache_waiter_printers);
        cb->setCurrentIndex(cb->findText(db.getString(6)));
        ui->tblDishes->setData(r, 7, db.getDouble("f_cost"));
    }
    double p = 0, sc = 0;
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        p += ui->tblDishes->lineEdit(i, 3)->getDouble() * ui->tblDishes->lineEdit(i, 4)->getDouble();
        sc += ui->tblDishes->getDouble(i, 7) * ui->tblDishes->lineEdit(i, 3)->getDouble();
    }
    ui->lePrice->setDouble(p);
    ui->leSelfCost->setDouble(sc);
}

bool CE5DishPackage::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        if (ui->tblDishes->comboBox(i, 5)->currentIndex() < 0) {
            err = tr("Storage not selected") + "<br>";
            return false;
        }
    }
    bool result = CE5Editor::save(err, data);
    if (!result) {
        C5Message::error(err);
        return false;
    }
    C5Database db(fDBParams);
    for (int id : fRemovedDish) {
        db[":f_id"] = id;
        db.exec("delete from d_package_list where f_id=:f_id");
    }
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        db[":f_package"] = ui->leCode->getInteger();
        db[":f_dish"] = ui->tblDishes->getInteger(i, 1);
        db[":f_qty"] = ui->tblDishes->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblDishes->lineEdit(i, 4)->getDouble();
        db[":f_store"] = ui->tblDishes->comboBox(i, 5)->currentData();
        db[":f_printer"] = ui->tblDishes->comboBox(i, 6)->currentText();
        if (ui->tblDishes->getInteger(i, 0) == 0) {
            ui->tblDishes->setInteger(i, 0, db.insert("d_package_list"));
        } else {
            db.update("d_package_list", "f_id", ui->tblDishes->getInteger(i, 0));
        }
    }
    return result;
}

void CE5DishPackage::clear()
{
    CE5Editor::clear();
    ui->tblDishes->clearContents();
    ui->tblDishes->setRowCount(0);
    setPrice("");
}

bool CE5DishPackage::checkData(QString &err)
{
    CE5Editor::checkData(err);
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        if (ui->tblDishes->lineEdit(i, 3)->getDouble() < 0.001) {
            err += QString("%1<br>").arg(tr("The quantity of dishes must be greater then zero."));
            break;
        }
    }
    return err.isEmpty();
}

void CE5DishPackage::newDish()
{
    QJsonArray vals;
    if (!C5Selector::getValue(fDBParams, cache_dish, vals)) {
        return;
    }
    int r = ui->tblDishes->addEmptyRow();
    ui->tblDishes->setData(r, 0, 0);
    ui->tblDishes->setData(r, 1, vals.at(1));
    ui->tblDishes->setData(r, 2, vals.at(2));
    ui->tblDishes->createLineEdit(r, 3);
    connect(ui->tblDishes->lineEdit(r, 3), SIGNAL(textChanged(QString)), this, SLOT(setPrice(QString)));
    ui->tblDishes->createLineEdit(r, 4);
    connect(ui->tblDishes->lineEdit(r, 4), SIGNAL(textChanged(QString)), this, SLOT(setPrice(QString)));
    ui->tblDishes->lineEdit(r, 3)->setFocus();
    ui->tblDishes->createComboBox(r, 5)->setCache(fDBParams, cache_goods_store);
    ui->tblDishes->createComboBox(r, 6)->setCache(fDBParams, cache_waiter_printers);
}

void CE5DishPackage::removeDish()
{
    int r = ui->tblDishes->currentRow();
    if (r < 0) {
        return;
    }
    if (ui->tblDishes->getInteger(r, 0) > 0) {
        fRemovedDish.append(ui->tblDishes->getInteger(r, 0));
    }
    ui->tblDishes->removeRow(r);
    double p = 0, sc = 0;
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        p += ui->tblDishes->lineEdit(i, 3)->getDouble() * ui->tblDishes->lineEdit(i, 4)->getDouble();
        sc += ui->tblDishes->getDouble(i, 7) * ui->tblDishes->lineEdit(i, 3)->getDouble();
    }
    ui->lePrice->setDouble(p);
    ui->leSelfCost->setDouble(sc);
}

void CE5DishPackage::setQty(const QString &arg1)
{
    Q_UNUSED(arg1);
    double p = 0, sc = 0;
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        p += ui->tblDishes->lineEdit(i, 3)->getDouble() * ui->tblDishes->lineEdit(i, 4)->getDouble();
        sc += ui->tblDishes->getDouble(i, 7) * ui->tblDishes->lineEdit(i, 3)->getDouble();
    }
    ui->lePrice->setDouble(p);
    ui->leSelfCost->setDouble(sc);
}

void CE5DishPackage::setPrice(const QString &arg)
{
    Q_UNUSED(arg);
    double p = 0, sc = 0;
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        p += ui->tblDishes->lineEdit(i, 3)->getDouble() * ui->tblDishes->lineEdit(i, 4)->getDouble();
        sc += ui->tblDishes->getDouble(i, 7) * ui->tblDishes->lineEdit(i, 3)->getDouble();
    }
    ui->lePrice->setDouble(p);
    ui->leSelfCost->setDouble(sc);
}

void CE5DishPackage::on_tblDishes_customContextMenuRequested(const QPoint &pos)
{
    auto *m = new QMenu(this);
    m->addAction(QIcon(":/add.png"), tr("Append"), this, SLOT(newDish()));
    m->addAction(QIcon(":/remove.png"), tr("Remove"), this, SLOT(removeDish()));
    m->popup(ui->tblDishes->mapToGlobal(pos));
}

#include "c5dishwidget.h"
#include "ui_c5dishwidget.h"
#include "c5grid.h"
#include "c5cache.h"
#include "c5selector.h"
#include "c5combobox.h"
#include <QColorDialog>

C5DishWidget::C5DishWidget(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::C5DishWidget)
{
    ui->setupUi(this);
    ui->lePart2->setSelector(dbParams, ui->lePart2Name, cache_dish_part2);
    ui->tblPricing->setColumnWidths(7, 0, 0, 100, 80, 100, 100, 100);
    ui->tblRecipe->setColumnWidths(7, 0, 0, 200, 80, 80, 80, 80);
    C5Database db(dbParams);
    db.exec("select f_id, f_name from d_menu_names");
    int row = 0;
    while (db.nextRow()) {
        ui->tblPricing->setRowCount(row + 1);
        for (int i = 0; i < ui->tblPricing->columnCount(); i++) {
            ui->tblPricing->setItem(row, i, new QTableWidgetItem());
        }
        ui->tblPricing->setInteger(row, 1, db.getInt("f_id"));
        ui->tblPricing->setString(row, 2, db.getString("f_name"));
        ui->tblPricing->createComboBox(row, 4)->setCache(dbParams, cache_goods_store);
        ui->tblPricing->createComboBox(row, 5)->setCache(dbParams, cache_waiter_printers);
        ui->tblPricing->createComboBox(row, 6)->setCache(dbParams, cache_waiter_printers);
        row++;
    }
    this->setEnabled(false);
    connect(ui->leColor, SIGNAL(doubleClicked()), this, SLOT(setColor()));
}

C5DishWidget::~C5DishWidget()
{
    delete ui;
}

QString C5DishWidget::title()
{
    return tr("Dish");
}

QString C5DishWidget::table()
{
    return "d_dish";
}

void C5DishWidget::setId(int id)
{
    CE5Editor::setId(id);
    setDish(id);
}

void C5DishWidget::clear()
{
    CE5Editor::clear();
    for (int i = 0; i < ui->tblPricing->rowCount(); i++) {
        ui->tblPricing->setString(i, 3, "");
    }
    ui->tblRecipe->clearContents();
    ui->tblRecipe->setRowCount(0);
    this->setEnabled(false);
}

void C5DishWidget::setDish(int id)
{
    C5Database db(fDBParams);
    db[":f_dish"] = id;
    db.exec("select m.f_id, m.f_menu, m.f_price, m.f_store, m.f_print1, m.f_print2 from d_menu m where f_dish=:f_dish");
    while (db.nextRow()) {
        for (int i = 0; i < ui->tblPricing->rowCount(); i++) {
            if (ui->tblPricing->getInteger(i, 1) == db.getInt("f_menu")) {
                ui->tblPricing->setInteger(i, 0, db.getInt("f_id"));
                ui->tblPricing->setDouble(i, 3, db.getDouble("f_price"));
                ui->tblPricing->comboBox(i, 4)->setCurrentIndex(ui->tblPricing->comboBox(i, 4)->findData(db.getInt("f_store")));
                ui->tblPricing->comboBox(i, 5)->setCurrentIndex(ui->tblPricing->comboBox(i, 5)->findText(db.getString("f_print1")));
                ui->tblPricing->comboBox(i, 6)->setCurrentIndex(ui->tblPricing->comboBox(i, 6)->findText(db.getString("f_print2")));
                break;
            }
        }
    }
    ui->tblRecipe->clearContents();
    ui->tblRecipe->setRowCount(0);
    int row = 0;
    db[":f_dish"] = id;
    db.exec("select r.f_id, r.f_goods, g.f_name, r.f_qty, u.f_name as g_unit, r.f_price, r.f_qty*r.f_price as f_total "
            "from d_recipes r "
            "left join c_goods g on g.f_id=r.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "where r.f_dish=:f_dish");
    while (db.nextRow()) {
        row = ui->tblRecipe->addEmptyRow();
        ui->tblRecipe->setInteger(row, 0, db.getInt("f_id"));
        ui->tblRecipe->setInteger(row, 1, db.getInt("f_goods"));
        ui->tblRecipe->setString(row, 2, db.getString("f_name"));
        C5LineEdit *l = ui->tblRecipe->createLineEdit(row, 3);
        l->setValidator(new QDoubleValidator(0, 100000, 4));
        l->setDouble(db.getDouble("f_qty"));
        ui->tblRecipe->setString(row, 4, db.getString("f_unit"));
        C5LineEdit *le = ui->tblRecipe->createLineEdit(row, 5);
        le->setDouble(db.getDouble("f_price"));
        le = ui->tblRecipe->createLineEdit(row, 6);
        le->setDouble(db.getDouble("f_total"));
        row++;
    }
    countTotalSelfCost();
    this->setEnabled(id > 0);
}

bool C5DishWidget::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    bool result = CE5Editor::save(err, data);
    if (!result) {
        return false;
    }
    C5Database db(fDBParams);
    for (int i = 0; i < ui->tblPricing->rowCount(); i++) {
        db[":f_dish"] = ui->leCode->getInteger();
        db[":f_price"] = ui->tblPricing->getDouble(i, 3);
        db[":f_menu"] = ui->tblPricing->getInteger(i, 1);
        db[":f_store"] = ui->tblPricing->comboBox(i, 4)->currentIndex() > -1 ? ui->tblPricing->comboBox(i, 4)->currentData() : "null";
        db[":f_print1"] = ui->tblPricing->comboBox(i, 5)->currentText();
        db[":f_print2"] = ui->tblPricing->comboBox(i, 6)->currentText();
        if (ui->tblPricing->getInteger(i, 0) == 0) {
            ui->tblPricing->setInteger(i, 0, db.insert("d_menu"));
        } else {
            db.update("d_menu", where_id(ui->tblPricing->getInteger(i, 0)));
        }
    }
    for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
        if (ui->tblRecipe->isRowHidden(i)) {
            if (ui->tblRecipe->getInteger(i, 0) > 0) {
                db[":f_id"] = ui->tblRecipe->getInteger(i, 0);
                db.exec("delete from d_recipes where f_id=:f_id");
            }
        }
        db[":f_dish"] = ui->leCode->getInteger();
        db[":f_goods"] = ui->tblRecipe->getInteger(i, 1);
        db[":f_qty"] = ui->tblRecipe->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblRecipe->lineEdit(i, 5)->getDouble();
        if (ui->tblRecipe->getInteger(i, 0) == 0) {
            ui->tblRecipe->setInteger(i, 0, db.insert("d_recipes"));
        } else {
            db.update("d_recipes", where_id(ui->tblRecipe->getInteger(i, 0)));
        }
    }
    return true;
}

void C5DishWidget::on_btnAddRecipe_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValue(fDBParams, cache_goods, values)) {
        return;
    }
    int row = ui->tblRecipe->addEmptyRow();
    C5LineEdit *l = ui->tblRecipe->createLineEdit(row, 3);
    l->setValidator(new QDoubleValidator(0, 100000, 4));
    l->clear();
    ui->tblRecipe->setInteger(row, 0, 0);
    ui->tblRecipe->setInteger(row, 1, values.at(0).toInt());
    ui->tblRecipe->setString(row, 2, values.at(2).toString());
    ui->tblRecipe->setString(row, 4, values.at(3).toString());
    ui->tblRecipe->createLineEdit(row, 5)->setDouble(0);
    ui->tblRecipe->createLineEdit(row, 6)->setDouble(0);
    l->setFocus();
}

void C5DishWidget::on_btnRemoveRecipe_clicked()
{
    int row = ui->tblRecipe->currentRow();
    if (row < 0) {
        return;
    }
    ui->tblRecipe->setRowHidden(row, true);
}

void C5DishWidget::setColor()
{
    QColor initColor = QColor::fromRgb(ui->leColor->color());
    int color = QColorDialog::getColor(initColor, this, tr("Background color")).rgb();
    ui->leColor->setColor(color);
    ui->leColor->setInteger(color);
}

void C5DishWidget::countTotalSelfCost()
{
    double total = 0;
    for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
        total += ui->tblRecipe->lineEdit(i, 6)->getDouble();
    }
    ui->leTotal->setDouble(total);
}
#include "c5dishwidget.h"
#include "ui_c5dishwidget.h"
#include "c5grid.h"
#include "c5cache.h"
#include "c5selector.h"
#include "c5combobox.h"
#include "c5printing.h"
#include "ce5dishpart2.h"
#include "c5printpreview.h"
#include <QColorDialog>

C5DishWidget::C5DishWidget(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::C5DishWidget)
{
    ui->setupUi(this);
    ui->lePart2->setSelector(dbParams, ui->lePart2Name, cache_dish_part2);
    ui->lePart2->setCallbackWidget(this);
    ui->tblPricing->setColumnWidths(8, 0, 0, 100, 80, 100, 100, 100, 30);
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
        ui->tblPricing->createLineEdit(row, 3)->setValidator(new QDoubleValidator(0, 999999999, 2));
        ui->tblPricing->createComboBox(row, 4)->setCache(dbParams, cache_goods_store);
        ui->tblPricing->createComboBox(row, 5)->setCache(dbParams, cache_waiter_printers);
        ui->tblPricing->createComboBox(row, 6)->setCache(dbParams, cache_waiter_printers);
        ui->tblPricing->createCheckbox(row, 7);
        row++;
    }
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
        ui->tblPricing->setInteger(i, 0, 0);
        ui->tblPricing->lineEdit(i, 3)->clear();
        ui->tblPricing->checkBox(i, 7)->setChecked(false);
    }
    ui->tblRecipe->clearContents();
    ui->tblRecipe->setRowCount(0);
}

void C5DishWidget::setDish(int id)
{
    C5Database db(fDBParams);
    db[":f_dish"] = id;
    db.exec("select m.f_id, m.f_menu, m.f_price, m.f_store, m.f_print1, m.f_print2, m.f_state from d_menu m where f_dish=:f_dish");
    while (db.nextRow()) {
        for (int i = 0; i < ui->tblPricing->rowCount(); i++) {
            if (ui->tblPricing->getInteger(i, 1) == db.getInt("f_menu")) {
                ui->tblPricing->setInteger(i, 0, db.getInt("f_id"));
                ui->tblPricing->lineEdit(i, 3)->setDouble(db.getDouble("f_price"));
                ui->tblPricing->comboBox(i, 4)->setCurrentIndex(ui->tblPricing->comboBox(i, 4)->findData(db.getInt("f_store")));
                ui->tblPricing->comboBox(i, 5)->setCurrentIndex(ui->tblPricing->comboBox(i, 5)->findText(db.getString("f_print1")));
                ui->tblPricing->comboBox(i, 6)->setCurrentIndex(ui->tblPricing->comboBox(i, 6)->findText(db.getString("f_print2")));
                ui->tblPricing->checkBox(i, 7)->setChecked(db.getInt(6) == 1);
                break;
            }
        }
    }
    ui->tblRecipe->clearContents();
    ui->tblRecipe->setRowCount(0);
    int row = 0;
    db[":f_dish"] = id;
    db.exec("select r.f_id, r.f_goods, g.f_name, r.f_qty, u.f_name as f_unit, r.f_price, r.f_qty*r.f_price as f_total "
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
        connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        ui->tblRecipe->setString(row, 4, db.getString("f_unit"));
        C5LineEdit *le = ui->tblRecipe->createLineEdit(row, 5);
        connect(le, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        le->setDouble(db.getDouble("f_price"));
        le = ui->tblRecipe->createLineEdit(row, 6);
        le->setDouble(db.getDouble("f_total"));
        connect(le, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        row++;
    }
    countTotalSelfCost();
}

void C5DishWidget::selectorCallback(int row, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    ui->lePart1Name->setText(values.at(2).toString());
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
        db[":f_price"] = ui->tblPricing->lineEdit(i, 3)->getDouble();
        db[":f_menu"] = ui->tblPricing->getInteger(i, 1);
        db[":f_store"] = ui->tblPricing->comboBox(i, 4)->currentIndex() > -1 ? ui->tblPricing->comboBox(i, 4)->currentData() : "null";
        db[":f_print1"] = ui->tblPricing->comboBox(i, 5)->currentText();
        db[":f_print2"] = ui->tblPricing->comboBox(i, 6)->currentText();
        db[":f_state"] = ui->tblPricing->checkBox(i, 7)->isChecked() ? 1 : 0;
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
    l->setFocus();
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
    ui->tblRecipe->setInteger(row, 0, 0);
    ui->tblRecipe->setInteger(row, 1, values.at(0).toInt());
    ui->tblRecipe->setString(row, 2, values.at(2).toString());
    ui->tblRecipe->setString(row, 4, values.at(3).toString());
    l = ui->tblRecipe->createLineEdit(row, 5);
    l->setDouble(0);
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
    l = ui->tblRecipe->createLineEdit(row, 6);
    l->setDouble(0);
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
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

void C5DishWidget::on_btnNewType_clicked()
{
    CE5DishPart2 *ep = new CE5DishPart2(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->lePart2->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void C5DishWidget::recipeQtyPriceChanged(const QString &arg)
{
    Q_UNUSED(arg);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row = l->property("row").toInt();
    switch (l->property("column").toInt()) {
    case 3:
        ui->tblRecipe->lineEdit(row, 6)->setDouble(ui->tblRecipe->lineEdit(row, 5)->getDouble() * l->getDouble());
        break;
    case 5:
        ui->tblRecipe->lineEdit(row, 6)->setDouble(ui->tblRecipe->lineEdit(row, 3)->getDouble() * l->getDouble());
        break;
    case 6:
        if (ui->tblRecipe->lineEdit(row, 3)->getDouble() > 0.0001) {
            ui->tblRecipe->lineEdit(row, 5)->setDouble(l->getDouble() / ui->tblRecipe->lineEdit(row, 3)->getDouble());
        } else {
            ui->tblRecipe->lineEdit(row, 5)->setDouble(0);
        }
        break;
    }
    countTotalSelfCost();
}

void C5DishWidget::on_btnPrintRecipe_clicked()
{
    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    QSize paperSize(2000, 2800);
    p.setSceneParams(paperSize.width(), paperSize.height(), QPrinter::Portrait);
    p.setFont(font);
    //int page = p.currentPageIndex();
    p.setFontBold(true);
    p.ctext(tr("Recipe") + " " + ui->leName->text());
    p.br();
    QList<qreal> points;
    QStringList vals;
    points << 0 << 600 << 300 << 300 << 300 << 300 ;
    vals.clear();
    vals << tr("Goods");
    vals << tr("Qty");
    vals << tr("Unit");
    vals << tr("Price");
    vals << tr("Cost");
    p.tableText(points, vals, 60);
    p.br(60);
    p.setFontBold(false);
    for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
        vals.clear();
        vals << ui->tblRecipe->getString(i, 2);
        vals << ui->tblRecipe->lineEdit(i, 3)->text();
        vals << ui->tblRecipe->getString(i, 4);
        vals << ui->tblRecipe->lineEdit(i, 5)->text();
        vals << ui->tblRecipe->lineEdit(i, 6)->text();
        p.tableText(points, vals, 60);
        p.br(60);
    }
    if (ui->tblRecipe->rowCount() > 0) {
        vals.clear();
        points.clear();
        points << 0 << 1500 << 300;
        vals << tr("Total") << ui->leTotal->text();
        p.tableText(points, vals, 60);
        p.br(60);
    }

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
}

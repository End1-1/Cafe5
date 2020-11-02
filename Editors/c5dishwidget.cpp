#include "c5dishwidget.h"
#include "ui_c5dishwidget.h"
#include "c5grid.h"
#include "c5cache.h"
#include "c5selector.h"
#include "c5combobox.h"
#include "ce5goods.h"
#include "c5printing.h"
#include "ce5dishpart2.h"
#include "c5printpreview.h"
#include <QColorDialog>
#include <QKeyEvent>
#include <QClipboard>
#include <QInputDialog>

C5DishWidget::C5DishWidget(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::C5DishWidget)
{
    ui->setupUi(this);
    ui->lePart2->setSelector(dbParams, ui->lePart2Name, cache_dish_part2);
    ui->lePart2->setCallbackWidget(this);
    ui->tblPricing->setColumnWidths(8, 0, 0, 100, 80, 100, 100, 100, 30);
    ui->tblRecipe->setColumnWidths(10, 0, 0, 200, 80, 80, 80, 80, 0, 0, 0);
    ui->tblComplex->setColumnWidths(3, 0, 250, 45);
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
    ui->tblRecipe->setColumnDecimals(3, 4);
    ui->lePortionQty->setValidator(new QDoubleValidator(0, 14, 2));
    connect(ui->leColor, SIGNAL(doubleClicked()), this, SLOT(setColor()));
    installEventFilter(this);
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
    ui->tblComplex->clearContents();
    ui->tblComplex->setRowCount(0);
    ui->lwComment->clear();
    ui->leDishComment->clear();
    ui->leQueue->setText("999");
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
    db.exec("select r.f_id, r.f_goods, g.f_name, r.f_qty, u.f_name as f_unit, g.f_lastinputprice as f_price, r.f_qty*g.f_lastinputprice as f_total, "
            "r.f_complex, f_complexQty, f_complexBaseQty "
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
        l->setValidator(new QDoubleValidator(0, 100000, 5));
        l->fDecimalPlaces = 5;
        l->setDouble(db.getDouble("f_qty"));
        if (db.getInt("f_complex") > 0) {
            l->setReadOnly(true);
        }
        if (db.getInt("f_complex") > 0) {
            connect(l, SIGNAL(textChanged(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        } else {
            connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        }
        ui->tblRecipe->setString(row, 4, db.getString("f_unit"));
        C5LineEdit *le = ui->tblRecipe->createLineEdit(row, 5);
        connect(le, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        le->setDouble(db.getDouble("f_price"));
        le = ui->tblRecipe->createLineEdit(row, 6);
        le->setDouble(db.getDouble("f_total"));
        connect(le, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        ui->tblRecipe->setString(row, 7, db.getString("f_complex"));
        ui->tblRecipe->setDouble(row, 8, db.getDouble("f_complexqty"));
        ui->tblRecipe->setDouble(row, 9, db.getDouble("f_complexbaseqty"));
        if (db.getInt("f_complex") > 0) {
            for (int i = 0; i < ui->tblRecipe->columnCount(); i++) {
                if (ui->tblRecipe->item(row, i)) {
                    ui->tblRecipe->item(row, i)->setBackground(QBrush(Qt::magenta));
                }
            }
        }
        row++;
    }
    C5Cache *c = C5Cache::cache(fDBParams, cache_dish);
    db[":f_dish"] = id;
    db.exec("select distinct(r.f_complex), f_complexQty  "
            "from d_recipes r "
            "where r.f_dish=:f_dish and r.f_complex>0 ");
    while (db.nextRow()) {
        if (db.getInt(0) == 0) {
            continue;
        }
        QString s = c->getString(db.getInt(0));
        int row = ui->tblComplex->addEmptyRow();
        ui->tblComplex->setString(row, 0, db.getString(0));
        ui->tblComplex->setString(row, 1, s);
        C5LineEdit *l = ui->tblComplex->createLineEdit(row, 2);
        l->setDouble(db.getDouble(1));
        connect(l, SIGNAL(textChanged(QString)), this, SLOT(complextQtyChanged(QString)));
    }
    countTotalSelfCost();
    db[":f_dish"] = id;
    db.exec("select f_comment from d_special where f_dish=:f_dish");
    while (db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lwComment);
        item->setText(db.getString(0));
    }
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
        db[":f_complex"] = ui->tblRecipe->getDouble(i, 7);
        db[":f_complexqty"] = ui->tblRecipe->getDouble(i, 8);
        db[":f_complexbaseqty"] = ui->tblRecipe->getDouble(i, 9);
        if (ui->tblRecipe->getInteger(i, 0) == 0) {
            ui->tblRecipe->setInteger(i, 0, db.insert("d_recipes"));
        } else {
            db.update("d_recipes", where_id(ui->tblRecipe->getInteger(i, 0)));
        }
    }
    db[":f_dish"] = ui->leCode->text();
    db.exec("delete from d_special where f_dish=:f_dish");
    for (int i = 0; i < ui->lwComment->count(); i++) {
        db[":f_dish"] = ui->leCode->text();
        db[":f_comment"] = ui->lwComment->item(i)->text();
        db.insert("d_special", false);
    }
    return true;
}

bool C5DishWidget::event(QEvent *e)
{
    if (e->type() == QEvent::KeyRelease) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        switch (ke->key()) {
        case Qt::Key_Plus:
            if (ui->tabWidget->currentIndex() == 1) {
                on_btnAddRecipe_clicked();
            }
            break;
        case Qt::Key_Minus:
            if (ui->tabWidget->currentIndex() == 1) {
                on_btnRemoveRecipe_clicked();
            }
            break;
        case Qt::Key_N:
            if (ui->tabWidget->currentIndex() == 1) {
                if (ke->modifiers() & Qt::ControlModifier) {
                    on_btnNewGoods_clicked();
                }
            }
            break;
        case Qt::Key_P:
            if (ui->tabWidget->currentIndex() == 1) {
                if (ke->modifiers() & Qt::ControlModifier) {
                    on_btnPrintRecipe_clicked();
                }
            }
            break;
        case Qt::Key_C:
            if (ui->tabWidget->currentIndex() == 1) {
                if (ke->modifiers() & Qt::ControlModifier) {
                    on_btnCopy_clicked();
                }
            }
            break;
        case Qt::Key_V:
            if (ui->tabWidget->currentIndex() == 1) {
                if (ke->modifiers() & Qt::ControlModifier) {
                    on_btnPasteRecipt_clicked();
                }
            }
            break;
        }
    }
    return CE5Editor::event(e);
}

void C5DishWidget::complextQtyChanged(const QString &arg)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row, col;
    if (ui->tblComplex->findWidget(l, row, col)) {
        QString code = ui->tblComplex->getString(row, 0);
        for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
            if (ui->tblRecipe->getString(i, 7) == code) {
                ui->tblRecipe->setData(i, 8, arg.toDouble());
                ui->tblRecipe->lineEdit(i, 3)->setDouble(ui->tblRecipe->getDouble(i, 9) * QLocale().toDouble(arg));
            }
        }
    }
}

void C5DishWidget::on_btnAddRecipe_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValue(fDBParams, cache_goods, values)) {
        return;
    }
    int row = addRecipeRow();
    ui->tblRecipe->setInteger(row, 0, 0);
    ui->tblRecipe->setInteger(row, 1, values.at(1).toInt());
    ui->tblRecipe->setString(row, 2, values.at(3).toString());
    ui->tblRecipe->setString(row, 4, values.at(4).toString());
    ui->tblRecipe->lineEdit(row, 5)->setDouble(values.at(6).toDouble());
}

void C5DishWidget::on_btnRemoveRecipe_clicked()
{
    int row = ui->tblRecipe->currentRow();
    if (row < 0) {
        return;
    }
    int complex = ui->tblRecipe->getInteger(row, 7);
    if (complex > 0) {
        if (C5Message::question(tr("This is part of complex and will remove immediatelly with other components. Continue?")) != QDialog::Accepted) {
            return;
        }
    }
    ui->tblRecipe->setRowHidden(row, true);
    if (complex > 0) {
        C5Database db(fDBParams);
        int code = ui->tblRecipe->getInteger(row, 7);
        for (int i = ui->tblRecipe->rowCount() - 1; i > -1; i--) {
            if (ui->tblRecipe->getInteger(i, 7) == code) {
                db[":f_id"] = ui->tblRecipe->getInteger(i, 0);
                db.exec("delete from d_recipes where f_id=:f_id");
                ui->tblRecipe->removeRow(i);
            }
        }
        for (int i = 0; i < ui->tblComplex->rowCount(); i++) {
            if (ui->tblComplex->getInteger(i, 0) == complex) {
                ui->tblComplex->removeRow(i);
                break;
            }
        }
    }
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
    double total = 0, totalWeight = 0;
    for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
        totalWeight += ui->tblRecipe->lineEdit(i, 3)->getDouble();
        total += ui->tblRecipe->lineEdit(i, 6)->getDouble();
    }
    ui->leWeight->setDouble(totalWeight);
    ui->leTotal->setDouble(total);
}

int C5DishWidget::addRecipeRow()
{
    int row = ui->tblRecipe->addEmptyRow();
    C5LineEdit *l = ui->tblRecipe->createLineEdit(row, 3);
    l->setValidator(new QDoubleValidator(0, 100000, 5));
    l->installEventFilter(this);
    l->fDecimalPlaces = 5;
    l->clear();
    l->setFocus();
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
    l = ui->tblRecipe->createLineEdit(row, 5);
    l->installEventFilter(this);
    l->setDouble(0);
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
    l = ui->tblRecipe->createLineEdit(row, 6);
    l->setDouble(0);
    l->installEventFilter(this);
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
    return row;
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

void C5DishWidget::on_btnAddDish_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValue(fDBParams, cache_dish, values)) {
        return;
    }
    if (ui->leCode->text() == values.at(0).toString()) {
        C5Message::info(tr("The recipe cannot contains itself"));
        return;
    }
    for (int i = 0; i < ui->tblComplex->rowCount(); i++) {
        if (ui->tblComplex->getString(i, 0) == values.at(0).toString()) {
            C5Message::info(tr("This dish already in the list"));
            return;
        }
    }
    int row = ui->tblComplex->addEmptyRow();
    C5LineEdit *l = ui->tblComplex->createLineEdit(row, 2);
    l->setValidator(new QDoubleValidator(0, 100000, 5));
    l->setDouble(1.0);
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(complextQtyChanged(QString)));
    l->setFocus();
    ui->tblComplex->setString(row, 0, values.at(0).toString());
    ui->tblComplex->setString(row, 1, values.at(1).toString());

    C5Database db(fDBParams);
    db[":f_dish"] = values.at(0).toInt();
    db.exec("select r.f_id, r.f_goods, g.f_name, r.f_qty, u.f_name as f_unit, r.f_price, r.f_qty*r.f_price as f_total, f_complex, f_complexqty, f_complexbaseqty "
            "from d_recipes r "
            "left join c_goods g on g.f_id=r.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "where r.f_dish=:f_dish");
    while (db.nextRow()) {
        row = ui->tblRecipe->addEmptyRow();
        ui->tblRecipe->setInteger(row, 0, 0);
        ui->tblRecipe->setInteger(row, 1, db.getInt("f_goods"));
        ui->tblRecipe->setString(row, 2, db.getString("f_name"));
        C5LineEdit *l = ui->tblRecipe->createLineEdit(row, 3);
        l->setValidator(new QDoubleValidator(0, 100000, 5));
        l->fDecimalPlaces = 4;
        l->setDouble(db.getDouble("f_qty"));
        l->setReadOnly(true);
        connect(l, SIGNAL(textChanged(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        ui->tblRecipe->setString(row, 4, db.getString("f_unit"));
        C5LineEdit *le = ui->tblRecipe->createLineEdit(row, 5);
        connect(le, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        le->setDouble(db.getDouble("f_price"));
        le = ui->tblRecipe->createLineEdit(row, 6);
        le->setDouble(db.getDouble("f_total"));
        connect(le, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        ui->tblRecipe->setString(row, 7, values.at(0).toString());
        for (int i = 0; i < ui->tblRecipe->columnCount(); i++) {
            if (ui->tblRecipe->item(row, i)) {
                ui->tblRecipe->item(row, i)->setBackground(QBrush(Qt::magenta));
            }
        }
        ui->tblRecipe->setDouble(row, 8, 1);
        ui->tblRecipe->setDouble(row, 9, db.getDouble("f_qty"));
        row++;
    }
}

void C5DishWidget::on_tblComplex_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    QString code = ui->tblComplex->getString(row, 0);
    for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
        if (code == ui->tblRecipe->getString(i, 7)) {
            for (int j = 0; j < ui->tblRecipe->columnCount(); j++) {
                if (ui->tblRecipe->item(i, j)) {
                    ui->tblRecipe->item(i, j)->setBackground(QBrush(Qt::green));
                }
            }
        } else if (ui->tblRecipe->getInteger(i, 7) > 0) {
            for (int j = 0; j < ui->tblRecipe->columnCount(); j++) {
                if (ui->tblRecipe->item(i, j)) {
                    ui->tblRecipe->item(i, j)->setBackground(QBrush(Qt::magenta));
                }
            }
        }
    }
}

void C5DishWidget::on_btnDeleteDish_clicked()
{
    QModelIndexList ml = ui->tblComplex->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    if (C5Message::question(tr("This is part of complex and will remove immediatelly with other components. Continue?")) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    int row = ml.at(0).row();
    QString code = ui->tblComplex->getString(row, 0);
    for (int i = ui->tblRecipe->rowCount() - 1; i > -1; i--) {
        QString rcode = ui->tblRecipe->getString(i, 7);
        if (rcode == code) {
            db[":f_id"] = ui->tblRecipe->getInteger(i, 0);
            db.exec("delete from d_recipes where f_id=:f_id");
            ui->tblRecipe->removeRow(i);
        }
    }
    ui->tblComplex->removeRow(0);
}

void C5DishWidget::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        int row = ui->tblRecipe->addEmptyRow();
        C5LineEdit *l = ui->tblRecipe->createLineEdit(row, 3);
        l->setValidator(new QDoubleValidator(0, 100000, 4));
        l->clear();
        l->setFocus();
        connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        ui->tblRecipe->setInteger(row, 0, 0);
        ui->tblRecipe->setInteger(row, 1, data.at(0)["f_id"].toInt());
        ui->tblRecipe->setString(row, 2, data.at(0)["f_name"].toString());
        ui->tblRecipe->setString(row, 4, data.at(0)["f_unitname"].toString());
        l = ui->tblRecipe->createLineEdit(row, 5);
        l->setDouble(0);
        connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        l = ui->tblRecipe->createLineEdit(row, 6);
        l->setDouble(0);
        connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
    }
    delete e;
}

void C5DishWidget::on_btnRemoveComment_clicked()
{
    QModelIndexList ml = ui->lwComment->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove dish comment")) == QDialog::Accepted) {
        ui->lwComment->removeItemWidget(ui->lwComment->item(ml.at(0).row()));
    }
}

void C5DishWidget::on_btnAddComment_clicked()
{
    if (ui->leDishComment->text().isEmpty()) {
        return;
    }
    QListWidgetItem *item = new QListWidgetItem(ui->lwComment);
    item->setText(ui->leDishComment->text());
    ui->leDishComment->clear();
    ui->leDishComment->setFocus();
}

void C5DishWidget::on_leDishComment_returnPressed()
{
    on_btnAddComment_clicked();
}

void C5DishWidget::on_btnDivQty_clicked()
{
    bool ok;
    double d = QInputDialog::getDouble(this, tr("Divider"), "", ui->lePortionQty->getDouble(), 0, 99999, 3, &ok);
    if (!ok) {
        return;
    }
    if (d > 0.00001) {
        for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
            ui->tblRecipe->lineEdit(i, 3)->setDouble(ui->tblRecipe->lineEdit(i, 3)->getDouble() / d);
            ui->tblRecipe->lineEdit(i, 3)->textEdited(ui->tblRecipe->lineEdit(i, 3)->text());
        }
    }
}

void C5DishWidget::on_btnCopy_clicked()
{
    QString t;
    for (int r = 0; r < ui->tblRecipe->rowCount(); r++) {
        t += ui->tblRecipe->getString(r, 1) + "\t";
        t += ui->tblRecipe->getString(r, 2) + "\t";
        t += ui->tblRecipe->lineEdit(r, 3)->text() + "\t";
        t += ui->tblRecipe->getString(r, 4) + "\t";
        t += ui->tblRecipe->lineEdit(r, 5)->text() + "\t";
        t += ui->tblRecipe->lineEdit(r, 6)->text() + "\t";
        t += ui->tblRecipe->getString(r, 7) + "\t";
        t += ui->tblRecipe->getString(r, 8) + "\t";
        t += ui->tblRecipe->getString(r, 9) + "\t";
        t += "\r\n";
    }
    qApp->clipboard()->setText(t);
}

void C5DishWidget::on_btnPasteRecipt_clicked()
{
    QStringList s = qApp->clipboard()->text().split("\r\n", Qt::SkipEmptyParts);
    for (const QString &l: s) {
        QStringList c = l.split("\t", Qt::SkipEmptyParts);
        int r = addRecipeRow();
        ui->tblRecipe->setString(r, 1, c.at(0));
        ui->tblRecipe->setString(r, 2, c.at(1));
        ui->tblRecipe->lineEdit(r, 3)->setText(c.at(2));
        ui->tblRecipe->setString(r, 4, c.at(3));
        ui->tblRecipe->lineEdit(r, 5)->setText(c.at(4));
        ui->tblRecipe->lineEdit(r, 6)->setText(c.at(5));
        ui->tblRecipe->setString(r, 7, c.at(6));
        ui->tblRecipe->setString(r, 8, c.at(7));
        ui->tblRecipe->setString(r, 9, c.at(8));
    }
    countTotalSelfCost();
}

void C5DishWidget::on_btnMultRecipe_clicked()
{
    bool mult = !ui->btnMultRecipe->property("mult").toBool();
    ui->btnMultRecipe->setProperty("mult", mult);
    for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
        if (mult) {
            ui->tblRecipe->lineEdit(i, 3)->setDouble(ui->tblRecipe->lineEdit(i, 3)->getDouble() * ui->lePortionQty->getDouble());
        } else {
            if (ui->lePortionQty->getDouble() > 0.00001) {
                ui->tblRecipe->lineEdit(i, 3)->setDouble(ui->tblRecipe->lineEdit(i, 3)->getDouble() / ui->lePortionQty->getDouble());
            }
        }
        ui->tblRecipe->lineEdit(i, 3)->textEdited(ui->tblRecipe->lineEdit(i, 3)->text());
    }
}

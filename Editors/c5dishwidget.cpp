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
#include <QMenu>
#include <QFileDialog>
#include <QClipboard>
#include <QInputDialog>
#include <QBuffer>
#include <QScrollBar>

C5DishWidget::C5DishWidget(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::C5DishWidget)
{
    ui->setupUi(this);
    ui->lePart2->setSelector(dbParams, ui->lePart2Name, cache_dish_part2);
    ui->lePart2->setCallbackWidget(this);
    ui->tblPricing->setColumnWidths(9, 0, 0, 100, 80, 100, 100, 100, 30, 30);
    ui->tblRecipe->setColumnWidths(10, 0, 0, 200, 80, 80, 80, 80, 80, 80);
    ui->tblRecipeTotal->setColumnWidths(10, 0, 0, 200, 80, 80, 80, 80, 80, 80);
    connect(ui->tblRecipe->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(recipeHeaderResized(int,int,int)));
    connect(ui->tblRecipeTotal->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(tableRecipeScroll(int)));
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
        ui->tblPricing->createCheckbox(row, 8);
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
        ui->tblPricing->checkBox(i, 8)->setChecked(false);
    }
    ui->tblRecipe->clearContents();
    ui->tblRecipe->setRowCount(0);
    ui->lwComment->clear();
    ui->leDishComment->clear();
    ui->leQueue->setText("999");
    ui->lbImage->setText(tr("Image, right click to begin"));
}

void C5DishWidget::setDish(int id)
{
    C5Database db(fDBParams);
    db[":f_dish"] = id;
    db.exec("select m.f_id, m.f_menu, m.f_price, m.f_store, m.f_print1, m.f_print2, m.f_state, m.f_recent from d_menu m where f_dish=:f_dish");
    while (db.nextRow()) {
        for (int i = 0; i < ui->tblPricing->rowCount(); i++) {
            if (ui->tblPricing->getInteger(i, 1) == db.getInt("f_menu")) {
                ui->tblPricing->setInteger(i, 0, db.getInt("f_id"));
                ui->tblPricing->lineEdit(i, 3)->setDouble(db.getDouble("f_price"));
                ui->tblPricing->comboBox(i, 4)->setCurrentIndex(ui->tblPricing->comboBox(i, 4)->findData(db.getInt("f_store")));
                ui->tblPricing->comboBox(i, 5)->setCurrentIndex(ui->tblPricing->comboBox(i, 5)->findText(db.getString("f_print1")));
                ui->tblPricing->comboBox(i, 6)->setCurrentIndex(ui->tblPricing->comboBox(i, 6)->findText(db.getString("f_print2")));
                ui->tblPricing->checkBox(i, 7)->setChecked(db.getInt(6) == 1);
                ui->tblPricing->checkBox(i, 8)->setChecked(db.getInt(7) == 1);
                break;
            }
        }
    }
    ui->tblRecipe->clearContents();
    ui->tblRecipe->setRowCount(0);
    int row = 0;
    db[":f_dish"] = id;
    db.exec("select r.f_id, r.f_goods, g.f_name, r.f_qty, u.f_name as f_unit, g.f_lastinputprice as f_price, "
            "r.f_qty*g.f_lastinputprice as f_total "
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
        connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        ui->tblRecipe->setString(row, 6, db.getString("f_unit"));
        C5LineEdit *le = ui->tblRecipe->createLineEdit(row, 7);
        connect(le, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        le->setDouble(db.getDouble("f_price"));
        le = ui->tblRecipe->createLineEdit(row, 8);
        le->setDouble(db.getDouble("f_total"));
        connect(le, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        row++;
    }

    countTotalSelfCost();
    db[":f_dish"] = id;
    db.exec("select f_comment from d_special where f_dish=:f_dish");
    while (db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lwComment);
        item->setText(db.getString(0));
    }

    if (ui->tabWidget->currentIndex() == 3) {
        loadImage();
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
        db[":f_store"] = ui->tblPricing->comboBox(i, 4)->currentIndex() > -1 ? ui->tblPricing->comboBox(i, 4)->currentData() : QVariant();
        db[":f_print1"] = ui->tblPricing->comboBox(i, 5)->currentText();
        db[":f_print2"] = ui->tblPricing->comboBox(i, 6)->currentText();
        db[":f_state"] = ui->tblPricing->checkBox(i, 7)->isChecked() ? 1 : 0;
        db[":f_recent"] = ui->tblPricing->checkBox(i, 8)->isChecked() ? 1 : 0;
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
        db[":f_price"] = ui->tblRecipe->lineEdit(i, 7)->getDouble();
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
    db[":f_recipeqty"] = ui->tblRecipeTotal->getDouble(0, 3);
    db[":f_netweight"] = ui->tblRecipeTotal->getDouble(0, 4);
    db[":f_cost"] = ui->tblRecipeTotal->getDouble(0, 8);
    db.update("d_dish", "f_id", ui->leCode->text());

    db[":f_app"] = "menu";
    db.exec("select f_version from s_app where f_app=:f_app");
    if (db.nextRow()) {
        db[":f_app"] = "menu";
        db[":f_version"] = db.getString("f_version").toInt() + 1;
        db.exec("update s_app set f_version=:f_version where f_app=:f_app");
    } else {
        db[":f_app"] = "menu";
        db[":f_version"] = "1";
        db.insert("s_app", false);
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

void C5DishWidget::on_btnAddRecipe_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods, values, 3)) {
        return;
    }
    int row = addRecipeRow();
    ui->tblRecipe->setInteger(row, 0, 0);
    ui->tblRecipe->setInteger(row, 1, values.at(1).toInt());
    ui->tblRecipe->setString(row, 2, values.at(3).toString());
    ui->tblRecipe->setString(row, 6, values.at(4).toString());
    ui->tblRecipe->lineEdit(row, 7)->setDouble(values.at(6).toDouble());
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

void C5DishWidget::uploadImage()
{
    if (ui->leCode->getInteger() == 0) {
        if (C5Message::question(tr("You should to save dish before upload an image")) ==  QDialog::Accepted) {
            QString err;
            QList<QMap<QString, QVariant> > data;
            if (!save(err, data)) {
                C5Message::error(err);
                return;
            }
        } else {
            return;
        }
    }
    QString fn = QFileDialog::getOpenFileName(this, tr("Image"), "", "*.jpg;*.png;*.bmp");
    if (fn.isEmpty()) {
        return;
    }
    QPixmap pm;
    if (!pm.load(fn)) {
        C5Message::error(tr("Could not load image"));
        return;
    }
    ui->lbImage->setPixmap(pm.scaled(ui->lbImage->size(), Qt::KeepAspectRatio));

    QByteArray ba;
    do {
        pm = pm.scaled(pm.width() * 0.8,  pm.height() * 0.8);
        ba.clear();
        QBuffer buff(&ba);
        buff.open(QIODevice::WriteOnly);
        pm.save(&buff, "JPG");
    } while (ba.size() > 100000);

    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("delete from d_image where f_id=:f_id");
    db[":f_id"] = ui->leCode->getInteger();
    db[":f_image"] = ba;
    db.exec("insert into d_image (f_id, f_image) values (:f_id, :f_image)");

}

void C5DishWidget::removeImage()
{
    if (C5Message::question(tr("Remove image")) !=  QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("delete from d_image where f_id=:f_id");
    ui->lbImage->setText(tr("Image"));
}

void C5DishWidget::recipeHeaderResized(int section, int oldsize, int newsize)
{
    Q_UNUSED(oldsize);
    ui->tblRecipeTotal->setColumnWidth(section, newsize);
}

void C5DishWidget::tableRecipeScroll(int value)
{
    ui->tblRecipe->horizontalScrollBar()->setValue(value);
}

void C5DishWidget::countTotalSelfCost()
{
    double total = 0, totalWeight = 0, totalNetto = 0, totalBrutto = 0;
    for (int i = 0; i < ui->tblRecipe->rowCount(); i++) {
        totalWeight += ui->tblRecipe->lineEdit(i, 3)->getDouble();
        totalNetto += ui->tblRecipe->getDouble(i, 4);
        totalBrutto += ui->tblRecipe->getDouble(i, 5);
        total += ui->tblRecipe->lineEdit(i, 8)->getDouble();
    }
    ui->tblRecipeTotal->setDouble(0, 3, totalWeight);
    ui->tblRecipeTotal->setDouble(0, 4, totalNetto);
    ui->tblRecipeTotal->setDouble(0, 5, totalBrutto);
    ui->tblRecipeTotal->setDouble(0, 8, total);
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
    l = ui->tblRecipe->createLineEdit(row, 7);
    l->installEventFilter(this);
    l->setDouble(0);
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
    l = ui->tblRecipe->createLineEdit(row, 8);
    l->setDouble(0);
    l->installEventFilter(this);
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));

    QStringList h;
    h.append(QString::number(row + 1));
    ui->tblRecipeTotal->setVerticalHeaderLabels(h);
    return row;
}

void C5DishWidget::loadImage()
{
    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("select * from d_image where f_id=:f_id");
    if (db.nextRow()) {
        QPixmap p;
        if (p.loadFromData(db.getValue("f_image").toByteArray())) {
            ui->lbImage->setPixmap(p.scaled(ui->lbImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
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
    int row, col;
    ui->tblRecipe->findWidget(l, row, col);
    switch (l->property("column").toInt()) {
    case 3:
        ui->tblRecipe->lineEdit(row, 8)->setDouble(ui->tblRecipe->lineEdit(row, 7)->getDouble() * l->getDouble());
        break;
    case 7:
        ui->tblRecipe->lineEdit(row, 8)->setDouble(ui->tblRecipe->lineEdit(row, 3)->getDouble() * l->getDouble());
        break;
    case 8:
        if (ui->tblRecipe->lineEdit(row, 3)->getDouble() > 0.0001) {
            ui->tblRecipe->lineEdit(row, 7)->setDouble(l->getDouble() / ui->tblRecipe->lineEdit(row, 3)->getDouble());
        } else {
            ui->tblRecipe->lineEdit(row, 7)->setDouble(0);
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
        vals << ui->tblRecipe->getString(i, 6);
        vals << ui->tblRecipe->lineEdit(i, 7)->text();
        vals << ui->tblRecipe->lineEdit(i, 8)->text();
        p.tableText(points, vals, 60);
        p.br(60);
    }
    if (ui->tblRecipe->rowCount() > 0) {
        vals.clear();
        points.clear();
        points << 0 << 1500 << 300;
        vals << tr("Total") << ui->tblRecipeTotal->getString(0, 8);
        p.tableText(points, vals, 60);
        p.br(60);
    }

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
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
        ui->tblRecipe->setString(row, 6, data.at(0)["f_unitname"].toString());
        l = ui->tblRecipe->createLineEdit(row, 7);
        l->setDouble(0);
        connect(l, SIGNAL(textEdited(QString)), this, SLOT(recipeQtyPriceChanged(QString)));
        l = ui->tblRecipe->createLineEdit(row, 8);
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
    QStringList s = qApp->clipboard()->text().split("\r\n", QString::SkipEmptyParts);
    for (const QString &l: s) {
        QStringList c = l.split("\t", QString::SkipEmptyParts);
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
        emit ui->tblRecipe->lineEdit(i, 3)->textEdited(ui->tblRecipe->lineEdit(i, 3)->text());
    }
}

void C5DishWidget::on_leName_textChanged(const QString &arg1)
{
    setWindowTitle(tr("Dish") + ": " + arg1);
}

void C5DishWidget::on_lbImage_customContextMenuRequested(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(QIcon(":/new.png"), tr("Upload image"), this, SLOT(uploadImage()));
    m->addAction(QIcon(":/delete.png"), tr("Remove image"), this, SLOT(removeImage()));
    m->popup(ui->lbImage->mapToGlobal(pos));
}

void C5DishWidget::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 3:
        loadImage();
        break;
    }
}

void C5DishWidget::on_chSameAsInStore_clicked(bool checked)
{
    if (checked) {
        if (ui->leBarcode->isEmpty()) {
            ui->chSameAsInStore->setChecked(false);
            C5Message::error(tr("No barcode defined."));
            return;
        }
        if (ui->leName->isEmpty()) {
            C5Message::error(tr("Name must be defined."));
            return;
        }
        if (ui->lePart2->getInteger() == 0) {
            C5Message::error(tr("Goods group must be defined"));
            return;
        }
        C5Database db(fDBParams);
        db[":f_scancode"] = ui->leBarcode->text();
        db.exec("select * from c_goods where f_scancode=:f_scancode");
        int goodsid;
        QString goodsname = ui->leName->text();
        if (db.nextRow()) {
            goodsid = db.getInt("f_id");
            goodsname = db.getString("f_name");
        } else {
            int groupid;
            db[":f_name"] = ui->lePart2Name->text();
            db.exec("select * from c_groups where f_name=:f_name");
            if (db.nextRow()) {
                groupid = db.getInt("f_id");
            } else {
                db[":f_name"] = ui->lePart2Name->text();
                db[":f_chargevalue"] = 0;
                groupid = db.insert("c_groups");
            }
            db[":f_group"] = groupid;
            db[":f_unit"] = 1;
            db[":f_name"] = ui->leName->text();
            db[":f_scancode"] = ui->leBarcode->text();
            db[":f_service"] = 0;
            db[":f_iscomplect"] = 0;
            goodsid = db.insert("c_goods");
            db[":f_storeid"] = goodsid;
            db.update("c_goods", "f_id", goodsid);
        }
        int row = addRecipeRow();
        ui->tblRecipe->setInteger(row, 0, 0);
        ui->tblRecipe->setInteger(row, 1, goodsid);
        ui->tblRecipe->setString(row, 2, goodsname);
        ui->tblRecipe->setString(row, 6, C5Cache::cache(fDBParams, cache_goods_unit)->getValuesForId(1).at(2).toString());
        ui->tblRecipe->lineEdit(row, 7)->setDouble(0);
    }
}

#include "ce5goods.h"
#include "ui_ce5goods.h"
#include "c5cache.h"
#include "ce5goodsgroup.h"
#include "ce5goodsunit.h"
#include "ce5partner.h"
#include "c5message.h"
#include "ce5goodsclass.h"
#include "c5selector.h"
#include "barcode.h"
#include "c5printpreview.h"
#include "c5printing.h"
#include "c5user.h"
#include "c5storebarcode.h"
#include <QClipboard>
#include <QFontDatabase>
#include <QDateTime>
#include <QCompleter>
#include <QMenu>
#include <QFileDialog>
#include <QBuffer>
#include <QStringListModel>
#include <QInputDialog>
#include <QPaintEngine>

static int fLastGroup = 0;
static int fLastUnit = 0;

CE5Goods::CE5Goods(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Goods)
{
    ui->setupUi(this);
    ui->leSupplier->setSelector(dbParams, ui->leSupplierName, cache_goods_partners);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group);
    ui->leUnit->setSelector(dbParams, ui->leUnitName, cache_goods_unit);
    ui->leLowLevel->setValidator(new QDoubleValidator(0, 100000, 4));
    ui->tblGoods->setColumnWidths(7, 0, 0, 400, 80, 80, 80, 80);
    ui->leClass1->setSelector(dbParams, ui->leClassName1, cache_goods_classes);
    ui->leClass2->setSelector(dbParams, ui->leClassName2, cache_goods_classes);
    ui->leClass3->setSelector(dbParams, ui->leClassName3, cache_goods_classes);
    ui->leClass4->setSelector(dbParams, ui->leClassName4, cache_goods_classes);
    ui->leStoreId->setSelector(dbParams, ui->leStoreIdName, cache_goods, 1, 3);
#ifndef QT_DEBUG
    ui->leIsComplect->setVisible(false);
#endif

    C5Database db(dbParams);
    db.exec("select f_name from c_goods");
    while (db.nextRow()) {
        fStrings.insert(db.getString(0));
    }
    QStringListModel *m = new QStringListModel(fStrings.toList());
    QCompleter *c = new QCompleter(m);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    ui->leName->setCompleter(c);
    fBarcode = new Barcode();
    ui->lbScancodeType->setVisible(false);
    ui->btnPinLast->setChecked(__c5config.getRegValue("last_goods_editor").toBool());
    ui->leCostPrice->fDecimalPlaces = 4;
    connect(ui->tblMultiscancode, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tblMultiscancodeContextMenu(QPoint)));
    QStringList l;
    l.append(tr("Retail"));
    l.append(tr("Whosale"));
    ui->tblPricing->setVerticalHeaderLabels(l);
    db.exec("select * from e_currency order by f_id");
    QStringList colLabels;
    while (db.nextRow()) {
        int c = ui->tblPricing->columnCount();
        ui->tblPricing->setColumnCount(c + 1);
        colLabels.append(db.getString("f_name"));
        ui->tblPricing->createLineEdit(0, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->createLineEdit(1, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->lineEdit(0, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(1, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(0, c)->setProperty("c", db.getInt("f_id"));
        ui->tblPricing->lineEdit(1, c)->setProperty("c", db.getInt("f_id"));
        connect(ui->tblPricing->lineEdit(0, c), &C5LineEdit::textEdited, this, &CE5Goods::priceEdited);
        connect(ui->tblPricing->lineEdit(1, c), &C5LineEdit::textEdited, this, &CE5Goods::priceEdited);
    }
    ui->tblPricing->setHorizontalHeaderLabels(colLabels);
    ui->tblPricing->fitColumnsToWidth();
    ui->cbCurrency->setDBValues(dbParams, "select f_id, f_name from e_currency");
    int basecurrecny = __c5config.getValue(param_default_currency).toInt();
    ui->cbCurrency->setIndexForValue(basecurrecny);
    connect(ui->leScanCode, &C5LineEditWithSelector::doubleClicked, this, &CE5Goods::genScancode);
    fScancodeGenerated = false;
    db.exec("select * from e_currency_cross_rate");
    while (db.nextRow()) {
        fCrossRate[QString("%1-%2").arg(db.getString("f_currency1"), db.getString("f_currency2"))] = db.getDouble("f_rate");
    }
    db.exec("select f_id, f_name from as_list");
    while (db.nextRow()) {
        int r = ui->tblAs->rowCount();
        ui->tblAs->setRowCount(r + 1);
        ui->tblAs->setInteger(r, 0, db.getInt("f_id"));
        ui->tblAs->setString(r, 1, db.getString("f_name"));
        ui->tblAs->createLineEdit(r, 2);
    }
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

QString CE5Goods::dbError(QString err)
{
    if (err.contains("f_scancode_UNIQUE")) {
        return tr("Duplicate scancode");
    }
    return CE5Editor::dbError(err);
}

void CE5Goods::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    /* scancode */
    if (id > 0) {
        db[":f_id"] = id;
        db.exec("select f_scancode from c_goods where f_id=:f_id");
        db.nextRow();
        ui->leScanCode->setText(db.getString("f_scancode"));
    }
    /* Complectation */
    db[":f_base"] = id;
    db.exec("select c.f_id, c.f_goods, concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_goodsname, c.f_qty, u.f_name as f_unitname, "
            "g.f_lastinputprice, c.f_qty*g.f_lastinputprice as f_total "
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
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble("f_lastinputprice"));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble("f_total"));
    }

    db[":f_goods"] = ui->leCode->getInteger();
    db.exec("select f_id from c_goods_multiscancode where f_goods=:f_goods");
    while (db.nextRow()) {
        int r = ui->tblMultiscancode->addEmptyRow();
        ui->tblMultiscancode->setString(r, 0, db.getString(0));
    }
    ui->chSameStoreId->setChecked(ui->leStoreId->getInteger() == ui->leCode->getInteger());
    emit ui->chSameStoreId->clicked();
    countTotal();

    if (__user->check(cp_t6_goods_only_price_edit)) {
        bool enabled = ui->leCode->getInteger() == 0;
        ui->tab_2->setEnabled(enabled);
        ui->tab_3->setEnabled(enabled);
        ui->tab_4->setEnabled(enabled);
        ui->tab_5->setEnabled(enabled);
        ui->leGroup->setEnabled(enabled);
        ui->leClass1->setEnabled(enabled);
        ui->leClass2->setEnabled(enabled);
        ui->leClass3->setEnabled(enabled);
        ui->leClass4->setEnabled(enabled);
        ui->leName->setEnabled(enabled);
        ui->leScanCode->setEnabled(enabled);
    }

    db[":f_goods"] = ui->leCode->getInteger();
    db.exec("select * from c_goods_prices where f_goods=:f_goods");
    while (db.nextRow()) {
        for (int j = 0; j < ui->tblPricing->columnCount(); j++) {
            if (ui->tblPricing->lineEdit(0, j)->property("c").toInt() == db.getInt("f_currency")) {
                ui->tblPricing->lineEdit(0, j)->setDouble(db.getDouble("f_price1"));
                ui->tblPricing->lineEdit(1, j)->setDouble(db.getDouble("f_price2"));
                break;
            }
        }
    }
    if (ui->cbCurrency->currentData().toInt() == 0) {
        ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    }
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("select f_asdbid, f_ascode from as_convert where f_table='c_goods' and f_tableid=:f_id");
    while (db.nextRow()) {
        int asrow = -1;
        for (int i = 0; i < ui->tblAs->rowCount(); i++) {
            if (ui->tblAs->getInteger(i, 0) == db.getInt("f_asdbid")) {
                asrow = i;
                break;
            }
        }
        if (asrow < 0) {
            throw std::exception(QString("The database id (%1) not exists. Check database structure.").arg(asrow).toLocal8Bit().data());
        }
        ui->tblAs->lineEdit(asrow, 2)->setText(db.getString("f_ascode"));
    }
}

bool CE5Goods::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    fLastGroup = ui->leGroup->getInteger();
    fLastUnit = ui->leUnit->getInteger();

    if (!ui->chSameStoreId->isChecked()) {
        if (ui->leStoreId->getInteger() == 0) {
            err += tr("Goods code for store output cannot be undefined") + "<br>";
        }
    }
    if (!err.isEmpty()) {
        return false;
    }
    if (ui->leCode->getInteger() > 0) {
        if (ui->chSameStoreId->isChecked() && ui->leStoreId->getInteger() == 0) {
            ui->leStoreId->setInteger(ui->leCode->getInteger());
        }
    }
    if (!CE5Editor::save(err, data)) {
        return false;
    }
    C5Database db(fDBParams);
    if (ui->leScanCode->text().length() > 0) {
        db[":f_id"] = ui->leCode->getInteger();
        db[":f_scancode"] = ui->leScanCode->text();
        db.exec("select * from c_goods where f_scancode=:f_scancode and f_id<>:f_id");
        if (db.nextRow()) {
            err += tr("Duplicate barecode");
            return false;
        }
        db[":f_scancode"] = ui->leScanCode->text();
        db.update("c_goods", "f_id", ui->leCode->getInteger());
    } else {
        db[":f_scancode"] = QVariant();
        db.update("c_goods", "f_id", ui->leCode->getInteger());
    }

    if (ui->chSameStoreId->isChecked()) {
        if (ui->leStoreId->getInteger() == 0) {
            ui->leStoreId->setInteger(ui->leCode->getInteger());
            db[":f_storeid"] = ui->leStoreId->getInteger();
            db.update("c_goods", where_id(ui->leCode->getInteger()));
        }
    }
    /* Complectation */
    db[":f_base"] = ui->leCode->text();
    db.exec("delete from c_goods_complectation where f_base=:f_base");
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_base"] = ui->leCode->text();
        db[":f_goods"] = ui->tblGoods->getInteger(i, 1);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        db.insert("c_goods_complectation", false);
    }

    /* Additional Options */
    fStrings.insert(ui->leName->text());
    static_cast<QStringListModel*>(ui->leName->completer()->model())->setStringList(fStrings.values());

    for (const QString &s: qAsConst(fScancodeAppend)) {
        db[":f_id"] = s;
        db[":f_goods"] = ui->leCode->getInteger();
        db.insert("c_goods_multiscancode", false);
    }
    for (const QString &s: fScancodeRemove) {
        db[":f_id"] = s;
        db.exec("delete from c_goods_multiscancode where f_id=:f_id");
    }
    fScancodeAppend.clear();
    fScancodeRemove.clear();

    db[":f_goods"] = ui->leCode->getInteger();
    db.exec("delete from c_goods_prices where f_goods=:f_goods");
    for (int j = 0; j < ui->tblPricing->columnCount(); j++) {
        db[":f_goods"] = ui->leCode->getInteger();
        db[":f_price1"] = ui->tblPricing->lineEdit(0, j)->getDouble();
        db[":f_price2"] = ui->tblPricing->lineEdit(1, j)->getDouble();
        db[":f_currency"] = ui->tblPricing->lineEdit(0, j)->property("c");
        db.insert("c_goods_prices");
    }

    if (fScancodeGenerated) {
        db[":f_counter"] = ui->leScanCode->text().left(7).toInt();
        db.exec("update c_goods_scancode_counter set f_counter=:f_counter where f_counter<:f_counter");
        fScancodeGenerated = false;
    }

    db[":f_table"] = "c_goods";
    db[":f_tableid"] = ui->leCode->getInteger();
    db.exec("delete from as_convert where f_table=:f_table and f_tableid=:f_tableid");
    for (int i = 0; i < ui->tblAs->rowCount(); i++) {
        db[":f_asdbid"] = ui->tblAs->getInteger(i, 0);
        db[":f_table"] = "c_goods";
        db[":f_tableid"] = ui->leCode->getInteger();
        db[":f_ascode"] = ui->tblAs->lineEdit(i, 2)->text();
        db.insert("as_convert");
    }

    return true;
}

void CE5Goods::clear()
{
    int scancode = ui->leScanCode->getInteger();
    CE5Editor::clear();
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->leLowLevel->setText("0");
    if (ui->tabWidget->currentIndex() > 1) {
        ui->tabWidget->setCurrentIndex(0);
    }
    ui->lbImage->setText(tr("Image"));
    ui->leTotal->clear();
    ui->chStoreInputBeforeSale->setChecked(false);
    ui->chOnlyWholeNumber->setChecked(false);
    ui->chSameStoreId->setChecked(true);
    ui->chSameStoreId->clicked(true);
    if (__c5config.getRegValue("last_goods_editor").toBool()) {
        ui->leGroup->setValue(fLastGroup);
        ui->leUnit->setValue(fLastUnit);
        if (scancode) {
            ui->leScanCode->setInteger(scancode + 1);
        }
        ui->leName->setFocus();
    }
    ui->tblMultiscancode->clearContents();
    ui->tblMultiscancode->setRowCount(0);
    fScancodeAppend.clear();
    fScancodeRemove.clear();
    for (int i = 0; i < ui->tblPricing->rowCount(); i++) {
        for (int c = 0; c < ui->tblPricing->columnCount(); c++) {
            ui->tblPricing->lineEdit(i, c)->clear();
        }
    }
    fScancodeGenerated = false;
    if (ui->cbCurrency->currentData().toInt() == 0) {
        ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    }
    for (int i = 0; i < ui->tblAs->rowCount(); i++) {
        ui->tblAs->lineEdit(i, 2)->clear();
    }
}

QPushButton *CE5Goods::b1()
{
    QPushButton *btn = new QPushButton(tr("Print card"));
    connect(btn, SIGNAL(clicked()), this, SLOT(printCard()));
    return btn;
}

void CE5Goods::printCard()
{
    C5Printing p;
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2000, 2700, QPrinter::Portrait);

    p.setFontSize(16);
    p.setFontBold(true);
    p.ltext(QString("%1: %2").arg(tr("Printed")).arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 0);
    p.setFontSize(25);
    QString docTypeText = tr("Goods card") + " " + ui->leName->text() + " " + ui->leScanCode->text();
    p.ctext(QString("%1").arg(docTypeText));
    p.br();
    p.ltext(tr("Goods group")  + ": " + ui->leGroupName->text(), 5);
    p.rtext(tr("Internal code") + ": " + ui->leCode->text());
    p.br();
    points << 5 << 300;
    for (int i = 0; i < ui->tblPricing->columnCount(); i++) {
        points << 300;
    }
    vals << tr("Retail price");
    for (int i = 0; i < ui->tblPricing->columnCount(); i++) {
        vals << ui->tblPricing->lineEdit(0, i)->text();
    }
    p.tableText(points, vals, p.fLineHeight);
    p.br(p.fLineHeight + 20);
    vals.clear();
    vals << tr("Whosale price");
    for (int i = 0; i < ui->tblPricing->columnCount(); i++) {
        vals << ui->tblPricing->lineEdit(1, i)->text();
    }
    p.tableText(points, vals, p.fLineHeight);
    p.br(p.fLineHeight + 20);
    p.br();

    points.clear();
    vals.clear();
    if (ui->tblGoods->rowCount() > 0) {
        p.br();
        p.ctext(tr("Complecation"));
        p.br();
        points << 5 << 250 << 700 << 200 << 200 << 200 << 200;
        vals << tr("Code") << tr("Name") << tr("Qty") << tr("Unit") << tr("Price") << tr("Total");
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            vals.clear();
            for (int c = 1; c < ui->tblGoods->columnCount(); c++) {
                vals << ui->tblGoods->getString(i, c);
            }
            p.tableText(points, vals, p.fLineHeight + 20);
            p.br(p.fLineHeight + 20);
        }
        p.br(p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        p.rtext(QString("%1: %2").arg(tr("Complectation cost")).arg(ui->leTotal->text()));
    }

    fEditor->close();
    C5PrintPreview pp(&p, fDBParams);
    pp.setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    pp.raise();  // for MacOS
    pp.activateWindow();
    pp.exec();
}

void CE5Goods::priceEdited(const QString &arg1)
{
    C5LineEdit *e = static_cast<C5LineEdit*>(sender());
    int r, c;
    if (!ui->tblPricing->findWidget(e, r, c)) {
        return;
    }
    C5LineEditWithSelector *l;
    switch (r) {
    case 0:
        l = ui->leMargin;
        break;
    case 1:
        l = ui->leMargin2;
        break;
    }
    int basecurrency = e->property("c").toInt();
    for (int i = 0; i < ui->tblPricing->columnCount(); i++) {
        if (ui->tblPricing->lineEdit(r, i) == e) {
            double costprice = ui->leCostPrice->getDouble();
            if (basecurrency != ui->cbCurrency->currentData().toInt()) {
                QString mcrossrate = QString("%1-%2").arg(ui->cbCurrency->currentData().toString(), QString::number(basecurrency));
                costprice *= fCrossRate[mcrossrate];
            }
            l->setDouble(((str_float(arg1) / costprice) - 1) * 100);
            continue;
        }
        QString crossrate = QString("%1-%2").arg(QString::number(basecurrency), ui->tblPricing->lineEdit(r, i)->property("c").toString());
        double rate = fCrossRate[crossrate];
        ui->tblPricing->lineEdit(r, i)->setDouble(str_float(arg1) * rate);
    }
}

void CE5Goods::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row, col;
    ui->tblGoods->findWidget(l, row, col);
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, 6);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void CE5Goods::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row, col;
    ui->tblGoods->findWidget(l, row, col);
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, 6);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void CE5Goods::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row, col;
    ui->tblGoods->findWidget(l, row, col);
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, 6);
    if (lqty->getDouble() > 0.0001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    } else {
        lprice->setDouble(0);
    }
    countTotal();
}

void CE5Goods::uploadImage()
{
    if (ui->leCode->getInteger() == 0) {
        if (C5Message::question(tr("You should to item before upload an image")) ==  QDialog::Accepted) {
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
    db.exec("delete from c_goods_images where f_id=:f_id");
    db[":f_id"] = ui->leCode->getInteger();
    db[":f_data"] = ba;
    db.exec("insert into c_goods_images (f_id, f_data) values (:f_id, :f_data)");
}

void CE5Goods::removeImage()
{
    if (C5Message::question(tr("Remove image")) !=  QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("delete from c_goods_images where f_id=:f_id");
    ui->lbImage->setText(tr("Image"));
}

void CE5Goods::newScancode()
{
    bool ok;
    QString scancode = QInputDialog::getText(this, tr("Scancode"), "", QLineEdit::Normal, "", &ok);
    ok = ok && !scancode.trimmed().isEmpty();
    if (!ok) {
        return;
    }
    for (int i = 0; i < ui->tblMultiscancode->rowCount(); i++) {
        if (ui->tblMultiscancode->getString(i, 0) == scancode) {
            C5Message::error(tr("Scancode exists"));
            return;
        }
    }
    C5Database db(fDBParams);
    db[":f_scancode"] = scancode;
    db.exec("select f_name from c_goods where f_scancode=:f_scancode");
    if (db.nextRow()) {
        C5Message::error(tr("This code already used") + "<br>" + db.getString(0));
        return;
    }
    db[":f_id"] = scancode;
    db.exec("select g.f_name from c_goods_multiscancode m left join c_goods g on g.f_id=m.f_goods where m.f_id=:f_id");
    if (db.nextRow()) {
        C5Message::error(tr("This code already used") + "<br>" + db.getString(0));
        return;
    }
    int r = ui->tblMultiscancode->addEmptyRow();
    ui->tblMultiscancode->setString(r, 0, scancode);
    fScancodeAppend.append(scancode);
}

void CE5Goods::removeScancode()
{
    int r = ui->tblMultiscancode->currentRow();
    if (r < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove scancode")) != QDialog::Accepted) {
        return;
    }
    fScancodeRemove.append(ui->tblMultiscancode->getString(r, 0));
    ui->tblMultiscancode->removeRow(r);
}

void CE5Goods::genScancode()
{
    if (!ui->leScanCode->isEmpty()) {
        C5Message::error(tr("Scancode field must be empty"));
        return;
    }
    C5Database db(fDBParams);
    db.exec("select * from c_goods_scancode_counter");
    db.nextRow();
    ui->leScanCode->setText(QString("%1").arg(db.getInt("f_counter") + 1, db.getInt("f_digitsnumber"), 10, QChar('0')));
//    int checksum = fBarcode->ean8CheckSum(ui->leScanCode->text());
//    ui->leScanCode->setText(QString("%1%2").arg(ui->leScanCode->text(), QString::number(checksum)));
    fScancodeGenerated = true;
}

void CE5Goods::tblMultiscancodeContextMenu(const QPoint &p)
{
    QMenu m;
    m.addAction(QIcon(":/new.png"), tr("New"), this, SLOT(newScancode()));
    m.addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeScancode()));
    m.exec(ui->tblMultiscancode->mapToGlobal(p));
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
    ui->tblGoods->setString(row, 1, vals.at(1).toString());
    ui->tblGoods->setString(row, 2, vals.at(3).toString());
    ui->tblGoods->setString(row, 4, vals.at(4).toString());
    ui->tblGoods->lineEdit(row, 5)->setDouble(vals.at(6).toDouble());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
    setComplectFlag();
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
    setComplectFlag();
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
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
    ui->tblGoods->setItem(row, 4, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, 5);
    lprice->setValidator(new QDoubleValidator(0, 100000000, 4));
    lprice->fDecimalPlaces = 4;
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
    setComplectFlag();
}

void CE5Goods::setComplectFlag()
{
    ui->leIsComplect->setInteger(ui->tblGoods->rowCount() == 0 ? 0 : 1);
    if (ui->leComplectOutputQty->getDouble() < 0.001) {
        ui->leComplectOutputQty->setDouble(1);
    }
}

void CE5Goods::countSalePrice(int r, double margin)
{
    int basecurrency = ui->cbCurrency->currentData().toInt();
    for (int c = 0; c < ui->tblPricing->columnCount(); c++) {
        if (ui->tblPricing->lineEdit(r, c)->property("c").toInt() == basecurrency) {
            ui->tblPricing->lineEdit(r, c)->setDouble(((margin / 100) * ui->leCostPrice->getDouble()) + ui->leCostPrice->getDouble());
        } else {
            QString crossrate = QString("%1-%2").arg(QString::number(basecurrency), ui->tblPricing->lineEdit(r, c)->property("c").toString());
            double rate = fCrossRate[crossrate];
            ui->tblPricing->lineEdit(r, c)->setDouble((((margin / 100) * ui->leCostPrice->getDouble()) + ui->leCostPrice->getDouble()) * rate);
        }
    }
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
    setComplectFlag();
}

void CE5Goods::on_btnNewPartner_clicked()
{
    CE5Partner *ep = new CE5Partner(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leSupplier->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5Goods::on_btnNewGroup1_clicked()
{
    CE5GoodsClass *ep = new CE5GoodsClass(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leClass1->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5Goods::on_btnNewGroup2_clicked()
{
    CE5GoodsClass *ep = new CE5GoodsClass(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leClass2->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5Goods::on_btnNewGroup3_clicked()
{
    CE5GoodsClass *ep = new CE5GoodsClass(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leClass3->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5Goods::on_btnNewGroup4_clicked()
{
    CE5GoodsClass *ep = new CE5GoodsClass(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leClass4->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5Goods::on_btnCopy_clicked()
{
    QString clipbrd;
    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        clipbrd += ui->tblGoods->getString(r, 1) + "\t";
        clipbrd += ui->tblGoods->getString(r, 2) + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 3)->text() + "\t";
        clipbrd += ui->tblGoods->getString(r, 4) + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 5)->text() + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 6)->text() + "\t";
        clipbrd += "\r\n";
    }
    if (!clipbrd.isEmpty()) {
        qApp->clipboard()->setText(clipbrd);
    }
}

void CE5Goods::on_btnPaste_clicked()
{
    QStringList rows = qApp->clipboard()->text().split("\r\n");
    foreach (const QString &s, rows) {
        QStringList cols = s.split("\t");
        if (cols.count() < 6) {
            continue;
        }
        int row = addGoodsRow();
        ui->tblGoods->setData(row, 1, cols.at(0));
        ui->tblGoods->setData(row, 2, cols.at(1));
        ui->tblGoods->lineEdit(row, 3)->setText(cols.at(2));
        ui->tblGoods->setData(row, 4, cols.at(3));
        ui->tblGoods->lineEdit(row, 5)->setText(cols.at(4));
        ui->tblGoods->lineEdit(row, 6)->setText(cols.at(5));
    }
    setComplectFlag();
}

void CE5Goods::on_lbImage_customContextMenuRequested(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(QIcon(":/new.png"), tr("Upload image"), this, SLOT(uploadImage()));
    m->addAction(QIcon(":/delete.png"), tr("Remove image"), this, SLOT(removeImage()));
    m->popup(ui->lbImage->mapToGlobal(pos));
}

void CE5Goods::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 3: {
        C5Database db(fDBParams);
        db[":f_id"] = ui->leCode->getInteger();
        db.exec("select * from c_goods_images where f_id=:f_id");
        if (db.nextRow()) {
            QPixmap p;
            if (p.loadFromData(db.getValue("f_data").toByteArray())) {
                ui->lbImage->setPixmap(p.scaled(ui->lbImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    }
    }
}

void CE5Goods::on_leScanCode_textChanged(const QString &arg1)
{
    if (fBarcode->isEan13(arg1)) {
        ui->lbScancodeType->setVisible(true);
        ui->lbScancodeType->setText("EAN13");
        ui->btnSetControlSum->setVisible(false);
    } else if (fBarcode->isEan13(arg1)) {
        ui->lbScancodeType->setVisible(true);
        ui->lbScancodeType->setText("EAN8");
        ui->btnSetControlSum->setVisible(false);
    } else {
        ui->lbScancodeType->setVisible(false);
        ui->btnSetControlSum->setVisible(true);
        ui->btnSetControlSum->setEnabled(arg1.length() == 12);
    }
}

void CE5Goods::on_btnSetControlSum_clicked()
{
    int checksum = fBarcode->ean13CheckSum(ui->leScanCode->text());
    if (checksum > -1) {
        ui->leScanCode->setText(ui->leScanCode->text() + QString::number(checksum));
    }
}

void CE5Goods::on_btnPinLast_clicked(bool checked)
{
    __c5config.setRegValue("last_goods_editor", checked);
}

void CE5Goods::on_chSameStoreId_clicked()
{
    ui->leStoreId->setEnabled(!ui->chSameStoreId->isChecked());
    ui->leStoreIdName->setEnabled(!ui->chSameStoreId->isChecked());
    if (ui->chSameStoreId->isChecked()) {
        ui->leStoreId->setValue(ui->leCode->getInteger());
    }
}

void CE5Goods::on_leUnitName_textChanged(const QString &arg1)
{
    ui->lbOutputUnit->setText(arg1);
}

void CE5Goods::on_btnPrintBarcode_clicked()
{
    QPrintDialog pd(this);
    if (pd.exec() == QDialog::Accepted) {
        C5StoreBarcode::printOneBarcode(ui->leScanCode->text(), ui->tblPricing->lineEdit(0, 0)->text(), "", ui->leName->text(), pd);
    }
}

void CE5Goods::on_leCostPrice_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    countSalePrice(0, str_float(ui->leMargin->text()));
    countSalePrice(1, str_float(ui->leMargin2->text()));
}

void CE5Goods::on_leMargin_textEdited(const QString &arg1)
{
    countSalePrice(0, str_float(arg1));
}

void CE5Goods::on_leMargin2_textEdited(const QString &arg1)
{
    countSalePrice(1, str_float(arg1));
}

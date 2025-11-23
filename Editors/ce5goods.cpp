#include "ce5goods.h"
#include "ui_ce5goods.h"
#include "c5cache.h"
#include "ce5goodsgroup.h"
#include "ce5goodsunit.h"
#include "ce5partner.h"
#include "c5message.h"
#include "c5selector.h"
#include "barcode.h"
#include "c5printpreview.h"
#include "c5printing.h"
#include "ce5goodsmodel.h"
#include "c5database.h"
#include "c5config.h"
#include "c5user.h"
#include "c5permissions.h"
#include "c5storebarcode.h"
#include "ean8generator.h"
#include "c5utils.h"
#include "c5replacecharacter.h"
#include <QClipboard>
#include <QFontDatabase>
#include <QDateTime>
#include <QCompleter>
#include <QMenu>
#include <QFileDialog>
#include <QBuffer>
#include <QStringListModel>
#include <QInputDialog>
#include <QPushButton>
#include <QPaintEngine>
#include <stdexcept>

static int fLastGroup = 0;
static int fLastUnit = 0;

CE5Goods::CE5Goods(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5Goods)
{
    ui->setupUi(this);
    ui->leSupplier->setSelector(ui->leSupplierName, cache_goods_partners);
    ui->leGroup->setSelector(ui->leGroupName, cache_goods_group);
    ui->leUnit->setSelector(ui->leUnitName, cache_goods_unit);
    ui->leLowLevel->setValidator(new QDoubleValidator(0, 100000, 4));
    ui->tblGoods->setColumnWidths(7, 0, 0, 400, 80, 80, 80, 80);
    ui->leStoreId->setSelector(ui->leStoreIdName, cache_goods, 1, 3);
#ifndef QT_DEBUG
    ui->leIsComplect->setVisible(false);
#endif
    C5Database db;
    db.exec("select f_name from c_goods");

    while(db.nextRow()) {
        fStrings.insert(db.getString(0));
    }

    QStringListModel *m = new QStringListModel(fStrings.values());
    QCompleter *c = new QCompleter(m);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    ui->leName->setCompleter(c);
    fBarcode = new Barcode();
    ui->lbScancodeType->setVisible(false);
    ui->btnPinLast->setChecked(__c5config.getRegValue("last_goods_editor").toBool());
    ui->leCostPrice->fDecimalPlaces = 4;
    QStringList l;
    l.append(tr("Sale price"));
    l.append(tr("Whosale"));
    l.append(tr("Retail disc"));
    l.append(tr("Whosale disc"));
    ui->tblPricing->setVerticalHeaderLabels(l);
    ui->tblPricing->setRowHidden(1, __c5config.getValue(211).toInt() == 1);
    db.exec("select * from e_currency order by f_id");
    QStringList colLabels;

    while(db.nextRow()) {
        int c = ui->tblPricing->columnCount();
        ui->tblPricing->setColumnCount(c + 1);
        colLabels.append(db.getString("f_name"));
        ui->tblPricing->createLineEdit(0, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->createLineEdit(1, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->createLineEdit(2, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->createLineEdit(3, c)->setValidator(new QDoubleValidator(0, 99999999, 4));
        ui->tblPricing->lineEdit(0, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(1, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(2, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(3, c)->fDecimalPlaces = 7;
        ui->tblPricing->lineEdit(0, c)->setProperty("c", db.getInt("f_id"));
        ui->tblPricing->lineEdit(1, c)->setProperty("c", db.getInt("f_id"));
        ui->tblPricing->lineEdit(2, c)->setProperty("c", db.getInt("f_id"));
        ui->tblPricing->lineEdit(3, c)->setProperty("c", db.getInt("f_id"));
        connect(ui->tblPricing->lineEdit(0, c), &C5LineEdit::textEdited, this, &CE5Goods::priceEdited);
        connect(ui->tblPricing->lineEdit(1, c), &C5LineEdit::textEdited, this, &CE5Goods::priceEdited);
    }

    ui->tblPricing->setHorizontalHeaderLabels(colLabels);
    ui->tblPricing->fitColumnsToWidth();
    ui->cbCurrency->setDBValues("select f_id, f_name from e_currency");
    int basecurrecny = __c5config.getValue(param_default_currency).toInt();
    ui->cbCurrency->setIndexForValue(basecurrecny);
    connect(ui->leScanCode, &C5LineEditWithSelector::doubleClicked, this, &CE5Goods::genScancode);
    fScancodeGenerated = false;
    db.exec("select * from e_currency_cross_rate");

    while(db.nextRow()) {
        fCrossRate[QString("%1-%2").arg(db.getInt("f_currency1")).arg(db.getInt("f_currency2"))] = db.getDouble("f_rate");
    }

    db.exec("select f_id, f_name from as_list");

    while(db.nextRow()) {
        int r = ui->tblAs->rowCount();
        ui->tblAs->setRowCount(r + 1);
        ui->tblAs->setInteger(r, 0, db.getInt("f_id"));
        ui->tblAs->setString(r, 1, db.getString("f_name"));
        ui->tblAs->createLineEdit(r, 2);
    }

    ui->rbGenEAN8->setChecked(__c5config.getRegValue("gen_ean8").toBool());
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
    if(err.contains("f_scancode_UNIQUE")) {
        return tr("Duplicate scancode");
    }

    return CE5Editor::dbError(err);
}

void CE5Goods::setId(int id)
{
    if(id > 0) {
        fHttp->createHttpQuery("/engine/goods/goods-open.php", QJsonObject{{"id", id}}, SLOT(openResponse(QJsonObject)));
    }
}

bool CE5Goods::save(QString &err, QList<QMap<QString, QVariant> >& data)
{
    fLastGroup = ui->leGroup->getInteger();
    fLastUnit = ui->leUnit->getInteger();

    if(!ui->chSameStoreId->isChecked()) {
        if(ui->leStoreId->getInteger() == 0) {
            err += tr("Goods code for store output cannot be undefined") + "<br>";
        }
    }

    if(ui->leQtyBox->getDouble() < 1) {
        ui->leQtyBox->setDouble(1);
    }

    if(!err.isEmpty()) {
        return false;
    }

    /* Additional Options */
    fStrings.insert(ui->leName->text());
    static_cast<QStringListModel*>(ui->leName->completer()->model())->setStringList(fStrings.values());
    fHttp->createHttpQuery("/engine/goods/goods-save.php", makeJsonObject(), SLOT(saveResponse(QJsonObject)));
    err = "json";
    return true;
}

void CE5Goods::clear()
{
    int scancode = ui->leScanCode->getInteger();
    fImage.clear();
    ui->tblBarcodes->setRowCount(0);
    CE5Editor::clear();
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->leLowLevel->setText("0");

    if(ui->tabWidget->currentIndex() > 1) {
        ui->tabWidget->setCurrentIndex(0);
    }

    ui->lbImage->setText(tr("Image"));
    ui->leTotal->clear();
    ui->chOnlyWholeNumber->setChecked(false);
    ui->chSameStoreId->setChecked(true);
    emit ui->chSameStoreId->clicked(true);

    if(__c5config.getRegValue("last_goods_editor").toBool()) {
        ui->leGroup->setValue(fLastGroup);
        ui->leUnit->setValue(fLastUnit);

        if(scancode) {
            ui->leScanCode->setInteger(scancode + 1);
        }

        ui->leName->setFocus();
    }

    for(int i = 0; i < ui->tblPricing->rowCount(); i++) {
        for(int c = 0; c < ui->tblPricing->columnCount(); c++) {
            ui->tblPricing->lineEdit(i, c)->clear();
        }
    }

    fScancodeGenerated = false;

    if(ui->cbCurrency->currentData().toInt() == 0) {
        ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    }

    for(int i = 0; i < ui->tblAs->rowCount(); i++) {
        ui->tblAs->lineEdit(i, 2)->clear();
    }
}

QPushButton* CE5Goods::b1()
{
    QPushButton *btn = new QPushButton(tr("Print card"));
    connect(btn, SIGNAL(clicked()), this, SLOT(printCard()));
    return btn;
}

QJsonObject CE5Goods::makeJsonObject()
{
    fJsonData = QJsonObject();
    QJsonObject j;
    j["f_id"] = ui->leCode->getInteger();
    j["f_name"] = ui->leName->text();
    j["f_supplier"] = ui->leSupplier->getInteger();
    j["f_group"] =  ui->leGroup->getInteger();
    j["f_unit"] = ui->leUnit->getInteger();
    j["f_lowlevel"] = ui->leLowLevel->getDouble();
    j["f_qtybox"] = ui->leQtyBox->getDouble();
    j["f_scancode"] = ui->leScanCode->text().isEmpty() ? QJsonValue() : ui->leScanCode->text();
    j["f_fiscalname"] = ui->leFiscalName->text();
    //TODO: option
    //j["f_lastinputprice"] = ui->leTotal->getDouble() > 0 ? ui->leTotal->getDouble() : ui->leCostPrice->getDouble();
    j["f_lastinputprice"] = ui->leCostPrice->getDouble();
    j["f_price_margin"] = ui->leMargin->getDouble();
    j["f_price_margin2"] = ui->leMargin2->getDouble();
    j["f_base_currency"] = ui->cbCurrency->currentData().toInt();
    j["f_storeid"] = ui->leStoreId->getInteger();
    j["f_adg"] = ui->leAdg->text();
    j["f_nospecial_price"] = ui->chNoSpecialPrice->isChecked() ? 1 : 0;
    j["f_service"] = ui->chService->isChecked() ? 1 : 0;
    j["f_enabled"] = ui->chEnabled->isChecked() ? 1 : 0;
    j["f_production"] = ui->chProduction->isChecked() ? 1  : 0;
    j["f_wholenumber"] = ui->chOnlyWholeNumber->isChecked() ? 1 : 0;
    j["f_complectout"] = ui->leComplectOutputQty->getDouble();
    j["f_component_exit"] = ui->chComponentExit->isChecked() ? 1 : 0;
    j["f_weblink"] = ui->leWebLink->text();
    j["f_queue"] = ui->leQueue->getInteger();
    j["f_description"] = ui->plainTextEdit->toPlainText();
    j["f_acc"] = ui->leAcc->text();
    fJsonData["goods"] = j;
    fJsonData["samestore"] = ui->chSameStoreId->isChecked();
    fJsonData["scangenerated"] = fScancodeGenerated;
    fJsonData["scancounter"] = ui->leScanCode->text().left(7).toInt();
    fJsonData["image"] = fImage.isEmpty() ? QJsonValue() : fImage;
    fJsonData["bigimage"] = fBigImage.isEmpty() ? QJsonValue() : fBigImage;
    QJsonArray ja;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        j = QJsonObject();
        j["f_base"] = ui->leCode->text();
        j["f_goods"] = ui->tblGoods->getInteger(i, 1);
        j["f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        j["f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        ja.append(j);
    }

    fJsonData["c_goods_complectation"] = ja;
    ja = QJsonArray();

    for(int i = 0; i < ui->tblPricing->columnCount(); i++) {
        j = QJsonObject();
        j["f_goods"] = ui->leCode->getInteger();
        j["f_price1"] = ui->tblPricing->lineEdit(0, i)->getDouble();
        j["f_price2"] = ui->tblPricing->lineEdit(1, i)->getDouble();
        j["f_price1disc"] = ui->tblPricing->lineEdit(2, i)->getDouble();
        j["f_price2disc"] = ui->tblPricing->lineEdit(3, i)->getDouble();
        j["f_currency"] = ui->tblPricing->lineEdit(0, i)->property("c").toInt();
        ja.append(j);
    }

    fJsonData["c_goods_prices"] = ja;
    fJsonData["astable"] = "c_goods";
    fJsonData["astableid"] = ui->leCode->getInteger();
    ja = QJsonArray();

    for(int i = 0; i < ui->tblAs->rowCount(); i++) {
        j = QJsonObject();
        j["f_asdbid"] = ui->tblAs->getInteger(i, 0);
        j["f_table"] = "c_goods";
        j["f_tableid"] = ui->leCode->getInteger();
        j["f_ascode"] = ui->tblAs->lineEdit(i, 2)->text();
        ja.append(j);
    }

    fJsonData["asconver"] = ja;

    if(ui->leCode->getInteger() > 0) {
        if(ui->chSameStoreId->isChecked() && ui->leStoreId->getInteger() == 0) {
            ui->leStoreId->setInteger(ui->leCode->getInteger());
        }
    }

    fJsonData["f_unitname"] = ui->leUnitName->text();
    QJsonArray barcodes;

    for(int i = 0; i < ui->tblBarcodes->rowCount(); i++) {
        barcodes.append(ui->tblBarcodes->getString(i, 0));
    }

    fJsonData["f_barcodes"] = barcodes;
    return fJsonData;
}

bool CE5Goods::acceptOnSave() const
{
    return true;
}

bool CE5Goods::isOnline()
{
    return true;
}

void CE5Goods::saveResponse(const QJsonObject &jdoc)
{
    // data[0]["f_saleprice1"] = ui->tblPricing->lineEdit(0, 0)->getDouble();
    // data[0]["f_saleprice2"] = ui->tblPricing->lineEdit(1, 0)->getDouble();
    QJsonObject j = fJsonData["goods"].toObject();
    j["f_id"] = jdoc["f_id"].toInt();
    j["f_unitname"] = ui->leUnitName->text();
    fJsonData["goods"] = j;
    fHttp->httpQueryFinished(sender());

    if(jdoc["isnew"].toBool()) {
        C5Cache::cache(cache_goods)->refreshId("g.f_id", j["f_id"].toInt());
    }

    if(acceptOnSave()) {
        emit Accept();
    }
}

void CE5Goods::openResponse(const QJsonObject &jdoc)
{
    fImage = jdoc["image"].toString();
    QByteArray ba = QByteArray::fromBase64(fImage.toLatin1());
    QPixmap p;
    p.loadFromData(ba);
    QPixmap scaledPixmap = p.scaled(ui->lbImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->lbImage->setPixmap(scaledPixmap);
    QJsonObject j = jdoc["goods"].toObject();
    ui->leCode->setInteger(j["f_id"].toInt());
    ui->leName->setText(j["f_name"].toString());
    ui->leGroup->setValue(j["f_group"].toInt());
    ui->leSupplier->setValue(j["f_supplier"].toInt());
    ui->leUnit->setValue(j["f_unit"].toInt());
    ui->leScanCode->setText(j["f_scancode"].toString());
    ui->leLowLevel->setDouble(j["f_lowlevel"].toDouble());
    ui->leQtyBox->setDouble(j["f_qtybox"].toDouble());
    ui->leFiscalName->setText(j["f_fiscalname"].toString());
    ui->leCostPrice->setDouble(j["f_lastinputprice"].toDouble());
    ui->leAdg->setText(j["f_adg"].toString());
    ui->leMargin->setDouble(j["f_price_margin"].toDouble());
    ui->leMargin2->setDouble(j["f_price_margin2"].toDouble());
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(j["f_base_currency"].toInt()));
    ui->leStoreId->setValue(j["f_storeid"].toInt());
    ui->chSameStoreId->setChecked(ui->leStoreId->getInteger() == ui->leCode->getInteger());
    ui->chEnabled->setChecked(j["f_enabled"].toInt() > 0);
    ui->chOnlyWholeNumber->setChecked(j["f_wholenumber"].toInt() > 0);
    ui->chService->setChecked(j["f_service"].toInt() > 0);
    ui->chNoSpecialPrice->setChecked(j["f_nospecial_price"].toInt() > 0);
    ui->leComplectOutputQty->setDouble(j["f_complectout"].toDouble());
    ui->chComponentExit->setChecked(j["f_component_exit"].toInt() > 0);
    ui->leWebLink->setText(j["f_weblink"].toString());
    ui->leQueue->setInteger(j["f_queue"].toInt());
    QJsonArray ja = jdoc["complect"].toArray();

    for(int i = 0; i < ja.size(); i++) {
        int row = addGoodsRow();
        const QJsonObject &j = ja.at(i).toObject();
        ui->tblGoods->setInteger(row, 0, j["f_id"].toInt());
        ui->tblGoods->setInteger(row, 1, j["f_goods"].toInt());
        ui->tblGoods->setString(row, 2, j["f_goodsname"].toString());
        ui->tblGoods->lineEdit(row, 3)->setDouble(j["f_qty"].toDouble());
        ui->tblGoods->setString(row, 4, j["f_unitname"].toString());
        ui->tblGoods->lineEdit(row, 5)->setDouble(j["f_lastinputprice"].toDouble());
        ui->tblGoods->lineEdit(row, 6)->setDouble(j["f_total"].toDouble());
    }

    countTotal();
    ja = jdoc["goods_prices"].toArray();

    for(int i = 0; i < ja.size(); i++) {
        const QJsonObject &o = ja.at(i).toObject();

        for(int j = 0; j < ui->tblPricing->columnCount(); j++) {
            if(ui->tblPricing->lineEdit(0, j)->property("c").toInt() == o["f_currency"].toInt()) {
                ui->tblPricing->lineEdit(0, j)->setDouble(o["f_price1"].toDouble());
                ui->tblPricing->lineEdit(1, j)->setDouble(o["f_price2"].toDouble());
                ui->tblPricing->lineEdit(2, j)->setDouble(o["f_price1disc"].toDouble());
                ui->tblPricing->lineEdit(3, j)->setDouble(o["f_price2disc"].toDouble());
                break;
            }
        }
    }

    if(ui->cbCurrency->currentData().toInt() == 0) {
        ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    }

    ja = jdoc["astable"].toArray();

    for(int i = 0; i < ja.size(); i++) {
        const QJsonObject &o = ja.at(i).toObject();
        int asrow = -1;

        for(int i = 0; i < ui->tblAs->rowCount(); i++) {
            if(ui->tblAs->getInteger(i, 0) == o["f_asdbid"].toInt()) {
                asrow = i;
                break;
            }
        }

        if(asrow < 0) {
            throw std::runtime_error(QString("The database id (%1) not exists. Check database structure.").arg(
                                         asrow).toLocal8Bit().data());
        }

        ui->tblAs->lineEdit(asrow, 2)->setText(o["f_ascode"].toString());
    }

    if(__user->check(cp_t6_goods_only_price_edit)) {
        bool enabled = ui->leCode->getInteger() == 0;
        ui->tab_2->setEnabled(enabled);
        ui->tab_3->setEnabled(enabled);
        ui->tab_4->setEnabled(enabled);
        ui->leGroup->setEnabled(enabled);
        ui->leName->setEnabled(enabled);
        ui->leScanCode->setEnabled(enabled);
    }

    QJsonArray jbarcodes = jdoc["barcodes"].toArray();

    for(int i = 0; i < jbarcodes.count(); i++) {
        int r = ui->tblBarcodes->addEmptyRow();
        ui->tblBarcodes->setString(i, 0, jbarcodes.at(i).toString());
        auto *b = static_cast<QPushButton*>(ui->tblBarcodes->createWidget2<QPushButton>(r, 1));
        connect(b, &QPushButton::clicked, this, [this, b]() {
            int r, c;

            if(ui->tblBarcodes->findWidget(b, r, c)) {
                ui->tblBarcodes->removeRow(r);
            }
        });
    }

    fHttp->httpQueryFinished(sender());
}

void CE5Goods::printCard()
{
    C5Printing p;
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2000, 2700, QPageLayout::Portrait);
    p.setFontSize(16);
    p.setFontBold(true);
    p.ltext(QString("%1: %2").arg(tr("Printed"), QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 0);
    p.setFontSize(25);
    QString docTypeText = tr("Goods card") + " " + ui->leName->text() + " " + ui->leScanCode->text();
    p.ctext(QString("%1").arg(docTypeText));
    p.br();
    p.ltext(tr("Goods group")  + ": " + ui->leGroupName->text(), 5);
    p.rtext(tr("Internal code") + ": " + ui->leCode->text());
    p.br();
    points << 5 << 300;

    for(int i = 0; i < ui->tblPricing->columnCount(); i++) {
        points << 300;
    }

    vals << tr("Retail price");

    for(int i = 0; i < ui->tblPricing->columnCount(); i++) {
        vals << ui->tblPricing->lineEdit(0, i)->text();
    }

    p.tableText(points, vals, p.fLineHeight);
    p.br(p.fLineHeight + 20);
    vals.clear();
    vals << tr("Whosale price");

    for(int i = 0; i < ui->tblPricing->columnCount(); i++) {
        vals << ui->tblPricing->lineEdit(1, i)->text();
    }

    p.tableText(points, vals, p.fLineHeight);
    p.br(p.fLineHeight + 20);
    p.br();
    points.clear();
    vals.clear();

    if(ui->tblGoods->rowCount() > 0) {
        p.br();
        p.ctext(tr("Complecation"));
        p.br();
        points << 5 << 100  << 700 << 200 << 200 << 200 << 200;
        vals << tr("NN")  << tr("Name") << tr("Qty") << tr("Unit") << tr("Price") << tr("Total");
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);

        for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
            vals.clear();
            vals << QString::number(i + 1);

            for(int c = 2; c < ui->tblGoods->columnCount(); c++) {
                vals << ui->tblGoods->getString(i, c);
            }

            p.tableText(points, vals, p.fLineHeight + 20);
            p.br(p.fLineHeight + 20);
        }

        p.br(p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        p.rtext(QString("%1: %2").arg(tr("Complectation cost"), ui->leTotal->text()));
    }

    fEditor->close();
    C5PrintPreview pp(&p);
    pp.setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    pp.raise();  // for MacOS
    pp.activateWindow();
    pp.exec();
}

void CE5Goods::priceEdited(const QString &arg1)
{
    C5LineEdit *e = static_cast<C5LineEdit*>(sender());
    int r, c;

    if(!ui->tblPricing->findWidget(e, r, c)) {
        return;
    }

    C5LineEditWithSelector *l;

    switch(r) {
    case 0:
        l = ui->leMargin;
        break;

    case 1:
        l = ui->leMargin2;
        break;
    }

    int basecurrency = e->property("c").toInt();

    for(int i = 0; i < ui->tblPricing->columnCount(); i++) {
        if(ui->tblPricing->lineEdit(r, i) == e) {
            double costprice = ui->leCostPrice->getDouble();

            if(basecurrency != ui->cbCurrency->currentData().toInt()) {
                QString mcrossrate = QString("%1-%2").arg(ui->cbCurrency->currentData().toString(), QString::number(basecurrency));
                costprice *= fCrossRate[mcrossrate];
            }

            l->setDouble(((str_float(arg1) / costprice) - 1) * 100);
            continue;
        }

        QString crossrate = QString("%1-%2").arg(QString::number(basecurrency), ui->tblPricing->lineEdit(r,
                            i)->property("c").toString());
        double rate = fCrossRate[crossrate];
        ui->tblPricing->lineEdit(r, i)->setDouble(str_float(arg1) *rate);
    }

    if(l->text() == "nan" || l->text() == "inf") {
        l->setDouble(0);
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
    ltotal->setDouble(lqty->getDouble() *lprice->getDouble());
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
    ltotal->setDouble(lqty->getDouble() *lprice->getDouble());
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

    if(lqty->getDouble() > 0.0001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    } else {
        lprice->setDouble(0);
    }

    countTotal();
}

void CE5Goods::uploadImage()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Image"), "", "*.jpg;*.png;*.bmp");

    if(fn.isEmpty()) {
        return;
    }

    QPixmap pm;

    if(!pm.load(fn)) {
        C5Message::error(tr("Could not load image"));
        return;
    }

    QByteArray ba;
    QBuffer bigBuff(&ba);
    pm.save(&bigBuff, "JPG");
    fBigImage = ba.toBase64();

    do {
        if(fBigImage.isEmpty()) {
        }

        pm = pm.scaled(pm.width() * 0.8,  pm.height() * 0.8);
        ba.clear();
        QBuffer buff(&ba);
        buff.open(QIODevice::WriteOnly);
        pm.save(&buff, "JPG");
    } while(ba.size() > 100000);

    ui->lbImage->setPixmap(pm.scaled(ui->lbImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    fImage = QString(ba.toBase64());
}

void CE5Goods::removeImage()
{
    if(C5Message::question(tr("Remove image")) !=  QDialog::Accepted) {
        return;
    }

    fImage = "";
    ui->lbImage->setText(tr("Image"));
}

void CE5Goods::genScancode()
{
    if(!ui->leScanCode->isEmpty()) {
        C5Message::error(tr("Scancode field must be empty"));
        return;
    }

    C5Database db;
    db.exec("select * from c_goods_scancode_counter");
    db.nextRow();
    ui->leScanCode->setText(QString("%1").arg(db.getInt("f_counter") + 1, db.getInt("f_digitsnumber"), 10, QChar('0')));
    //    int checksum = fBarcode->ean8CheckSum(ui->leScanCode->text());
    //    ui->leScanCode->setText(QString("%1%2").arg(ui->leScanCode->text(), QString::number(checksum)));
    fScancodeGenerated = true;
}

void CE5Goods::on_btnNewGroup_clicked()
{
    CE5GoodsGroup *ep = new CE5GoodsGroup();
    C5Editor *e = C5Editor::createEditor(ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        ui->leGroup->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void CE5Goods::on_btnNewUnit_clicked()
{
    CE5GoodsUnit *ep = new CE5GoodsUnit();
    C5Editor *e = C5Editor::createEditor(ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        ui->leUnit->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void CE5Goods::on_btnAddGoods_clicked()
{
    QJsonArray vals;

    if(!C5Selector::getValue(cache_goods, vals)) {
        return;
    }

    int row = addGoodsRow();
    ui->tblGoods->setInteger(row, 1, vals.at(1).toInt());
    ui->tblGoods->setString(row, 2, vals.at(3).toString());
    ui->tblGoods->setString(row, 4, vals.at(4).toString());
    ui->tblGoods->lineEdit(row, 5)->setDouble(vals.at(6).toDouble());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
    setComplectFlag();
}

void CE5Goods::on_btnRemoveGoods_clicked()
{
    int row = ui->tblGoods->currentRow();

    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblGoods->item(row, 2)->text()) != QDialog::Accepted) {
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

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 6)->getDouble();
    }

    ui->leTotal->setDouble(total);
    setComplectFlag();
}

void CE5Goods::setComplectFlag()
{
    ui->leIsComplect->setInteger(ui->tblGoods->rowCount() == 0 ? 0 : 1);

    if(ui->leComplectOutputQty->getDouble() < 0.001) {
        ui->leComplectOutputQty->setDouble(1);
    }
}

void CE5Goods::countSalePrice(int r, double margin)
{
    int basecurrency = ui->cbCurrency->currentData().toInt();

    for(int c = 0; c < ui->tblPricing->columnCount(); c++) {
        if(ui->tblPricing->lineEdit(r, c)->property("c").toInt() == basecurrency) {
            ui->tblPricing->lineEdit(r, c)->setDouble(((margin / 100) *ui->leCostPrice->getDouble()) +
                ui->leCostPrice->getDouble());
        } else {
            QString crossrate = QString("%1-%2").arg(QString::number(basecurrency), ui->tblPricing->lineEdit(r,
                                c)->property("c").toString());
            double rate = fCrossRate[crossrate];
            ui->tblPricing->lineEdit(r, c)->setDouble((((margin / 100) *ui->leCostPrice->getDouble()) +
                    ui->leCostPrice->getDouble()) *rate);
        }
    }
}

void CE5Goods::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(ep, 0);
    QJsonObject data;

    if(e->getJsonObject(data)) {
        int row = addGoodsRow();
        data = data["goods"].toObject();
        ui->tblGoods->setData(row, 1, data["f_id"].toInt());
        ui->tblGoods->setData(row, 2, data["f_name"].toString());
        ui->tblGoods->setData(row, 4, data["f_unitname"].toString());
        ui->tblGoods->lineEdit(row, 3)->setFocus();
    }

    delete e;
    setComplectFlag();
}

void CE5Goods::on_btnNewPartner_clicked()
{
    CE5Partner *ep = new CE5Partner();
    C5Editor *e = C5Editor::createEditor(ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        ui->leSupplier->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void CE5Goods::on_btnCopy_clicked()
{
    QString clipbrd;

    for(int r = 0; r < ui->tblGoods->rowCount(); r++) {
        clipbrd += ui->tblGoods->getString(r, 1) + "\t";
        clipbrd += ui->tblGoods->getString(r, 2) + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 3)->text() + "\t";
        clipbrd += ui->tblGoods->getString(r, 4) + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 5)->text() + "\t";
        clipbrd += ui->tblGoods->lineEdit(r, 6)->text() + "\t";
        clipbrd += "\r\n";
    }

    if(!clipbrd.isEmpty()) {
        qApp->clipboard()->setText(clipbrd);
    }
}

void CE5Goods::on_btnPaste_clicked()
{
    QStringList rows = qApp->clipboard()->text().split("\r\n");

    foreach(const QString &s, rows) {
        QStringList cols = s.split("\t");

        if(cols.count() < 6) {
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
    switch(index) {
    case 3: {
        C5Database db;
        db[":f_id"] = ui->leCode->getInteger();
        db.exec("select * from c_goods_images where f_id=:f_id");

        if(db.nextRow()) {
            QPixmap p;

            if(p.loadFromData(db.getValue("f_data").toByteArray())) {
                ui->lbImage->setPixmap(p.scaled(ui->lbImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    }
    }
}

void CE5Goods::on_leScanCode_textChanged(const QString &arg1)
{
    if(fBarcode->isEan13(arg1)) {
        ui->lbScancodeType->setVisible(true);
        ui->lbScancodeType->setText("EAN13");
        ui->btnSetControlSum->setVisible(false);
    } else if(fBarcode->isEan13(arg1)) {
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

    if(checksum > -1) {
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

    if(ui->chSameStoreId->isChecked()) {
        ui->leStoreId->setValue(ui->leCode->getInteger());
    }
}

void CE5Goods::on_leUnitName_textChanged(const QString &arg1)
{
    ui->lbOutputUnit->setText(arg1);
}

void CE5Goods::on_btnPrintBarcode_clicked()
{
    // if(ui->rbGenEAN8->isChecked()) {
    //     if(ui->leScanCode->text().length() == 4) {
    //         QString code = QString("%1").arg(ui->leGroup->getInteger(), 3, 10,  QChar('0'));
    //         ui->leScanCode->setText(ui->leScanCode->text() + code);
    //     }
    //     if(ui->leScanCode->text().length() == 7) {
    //         ui->leScanCode->setText(Ean8Generator::last(ui->leScanCode->text()));
    //     }
    // }
    QPrintDialog pd;

    if(pd.exec() == QDialog::Accepted) {
        C5StoreBarcode::printOneBarcode(ui->leScanCode->text(), ui->tblPricing->lineEdit(0, 0)->text(), "", ui->leName->text(),
                                        pd);
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

void CE5Goods::on_btnNewModel_clicked()
{
    CE5GoodsModel *ep = new CE5GoodsModel();
    C5Editor *e = C5Editor::createEditor(ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
    }

    delete e;
}

void CE5Goods::on_leScanCode_returnPressed()
{
    ui->leScanCode->setText(C5ReplaceCharacter::replace(ui->leScanCode->text()));
}

void CE5Goods::on_leBarcode_returnPressed()
{
    int r = ui->tblBarcodes->addEmptyRow();
    ui->tblBarcodes->setString(r, 0, ui->leBarcode->text());
    auto *b = static_cast<QPushButton*>(ui->tblBarcodes->createWidget2<QPushButton>(r, 1));
    connect(b, &QPushButton::clicked, this, [this, b]() {
        int r, c;

        if(ui->tblBarcodes->findWidget(b, r, c)) {
            ui->tblBarcodes->removeRow(r);
        }
    });
    ui->leBarcode->clear();
    ui->tblBarcodes->resizeColumnsToContents();
    ui->leBarcode->setFocus();
}

void CE5Goods::on_leTotal_textChanged(const QString &arg1)
{
    //ui->leCostPrice->setText(arg1);
}

void CE5Goods::on_rbGenEAN8_clicked(bool checked)
{
    __c5config.setRegValue("gen_ean8", checked);
}

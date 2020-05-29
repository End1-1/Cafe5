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
#include <QClipboard>
#include <QFontDatabase>
#include <QCompleter>
#include <QMenu>
#include <QFileDialog>
#include <QStringListModel>
#include <QPaintEngine>

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
    /* Complectation */
    db[":f_base"] = id;
    db.exec("select c.f_id, c.f_goods, concat(g.f_name, ' ', g.f_scancode) as f_goodsname, c.f_qty, u.f_name as f_unitname, "
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
    countTotal();
}

bool CE5Goods::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (!CE5Editor::save(err, data)) {
        return false;
    }
    C5Database db(fDBParams);
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
    ui->chEnabled->setChecked(true);
    fStrings.insert(ui->leName->text());
    static_cast<QStringListModel*>(ui->leName->completer()->model())->setStringList(fStrings.values());
    return true;
}

void CE5Goods::clear()
{
    CE5Editor::clear();;
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->leLowLevel->setText("0");
    if (ui->tabWidget->currentIndex() > 1) {
        ui->tabWidget->setCurrentIndex(0);
    }
    ui->lbImage->setText(tr("Image"));
    ui->leTotal->clear();
}

void CE5Goods::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    C5LineEdit *lqty = ui->tblGoods->lineEdit(l->property("row").toInt(), 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(l->property("row").toInt(), 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(l->property("row").toInt(), 6);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void CE5Goods::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    C5LineEdit *lqty = ui->tblGoods->lineEdit(l->property("row").toInt(), 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(l->property("row").toInt(), 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(l->property("row").toInt(), 6);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void CE5Goods::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    C5LineEdit *lqty = ui->tblGoods->lineEdit(l->property("row").toInt(), 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(l->property("row").toInt(), 5);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(l->property("row").toInt(), 6);
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
    qApp->processEvents();
    QFile f(fn);
    if (f.open(QIODevice::ReadOnly)) {
        C5Database db(fDBParams);
        db[":f_id"] = ui->leCode->getInteger();
        db.exec("delete from c_goods_images where f_id=:f_id");
        db[":f_id"] = ui->leCode->getInteger();
        db[":f_data"] = f.readAll();
        db.exec("insert into c_goods_images (f_id, f_data) values (:f_id, :f_data)");
        f.close();
    }
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
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
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
    case 2: {
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

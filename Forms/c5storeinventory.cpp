#include "c5storeinventory.h"
#include "ui_c5storeinventory.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5printing.h"
#include "c5printpreview.h"
#include "c5config.h"
#include "c5user.h"
#include "ce5goods.h"
#include "c5message.h"
#include "ce5editor.h"
#include "c5utils.h"
#include "c5storedraftwriter.h"
#include <QShortcut>
#include <QUuid>

C5StoreInventory::C5StoreInventory(QWidget *parent) :
    C5Widget(parent),
    ui(new Ui::C5StoreInventory)
{
    ui->setupUi(this);
    fIcon = ":/goods.png";
    fLabel = tr("Store inventory");
    ui->leStore->setSelector(ui->leStoreName, cache_goods_store);
    ui->tblGoods->setColumnWidths(7, 0, 0, 300, 80, 80, 80, 80);
    ui->wSearch->setVisible(false);
    connect(ui->leStore, SIGNAL(keyPressed(QChar)), this, SLOT(keyPressed(QChar)));
    new QShortcut(QKeySequence("Ctrl+Shift++"), this, SLOT(keyShortcut()));
    new QShortcut(QKeySequence("Ctrl+Shift+="), this, SLOT(keyShortcut()));
    new QShortcut(QKeySequence("Ctrl+Shift+-"), this, SLOT(keyShortcut()));
}

C5StoreInventory::~C5StoreInventory()
{
    delete ui;
}

QToolBar *C5StoreInventory::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
    }
    return fToolBar;
}

bool C5StoreInventory::openDoc(QString id)
{
    fInternalID = id;
    C5Database db;
    db[":f_id"] = id;
    db.exec("select * from a_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid document number"));
        return false;
    }
    ui->leDocNum->setText(db.getString("f_userid"));
    ui->deDate->setDate(db.getDate("f_date"));
    ui->leComment->setText(db.getString("f_comment"));
    db[":f_document"] = id;
    db.exec("select d.f_id, d.f_goods, concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_name, \
            d.f_qty, u.f_name, d.f_price, d.f_total, d.f_store \
            from a_store_inventory d \
            left join c_goods g on g.f_id=d.f_goods \
            left join c_units u on u.f_id=g.f_unit \
            where d.f_document=:f_document ");
    while (db.nextRow()) {
        int row = addGoodsRow();
        ui->leStore->setValue(db.getInt("f_store"));
        ui->tblGoods->setString(row, 0, db.getString(0));
        ui->tblGoods->setInteger(row, 1, db.getInt(1));
        ui->tblGoods->setString(row, 2, db.getString(2));
        ui->tblGoods->lineEdit(row, 3)->setDouble(db.getDouble(3));
        ui->tblGoods->setString(row, 4, db.getString(4));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble(5));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble(6));
    }
    return true;
}

bool C5StoreInventory::removeDoc(QString id)
{
    C5Database db;
    db[":f_document"] = id;
    db.exec("delete from a_store_inventory where f_document=:f_document");
    db[":f_id"] = id;
    db.exec("delete from a_header where f_id=:f_id");
    return true;
}

bool C5StoreInventory::allowChangeDatabase()
{
    return false;
}

void C5StoreInventory::keyShortcut()
{
    QString s = static_cast<QShortcut *>(sender())->key().toString();
    keyPressed(s.at(s.length() - 1));
}

void C5StoreInventory::keyPressed(const QChar &c)
{
    switch (c.toLatin1()) {
        case '+':
            on_btnAddGoods_clicked();
            break;
        case '-':
            on_btnRemoveGoods_clicked();
            break;
    }
}

void C5StoreInventory::saveDoc()
{
    C5Database db;
    QString err;
    if (!docCheck(err)) {
        C5Message::error(err);
        return;
    }
    C5StoreDraftWriter dw(db);
    if (ui->leDocNum->isEmpty()) {
        ui->leDocNum->setInteger(dw.counterAType(DOC_TYPE_STORE_INVENTORY));
    }
    dw.writeAHeader(fInternalID, ui->leDocNum->text(), DOC_STATE_SAVED, DOC_TYPE_STORE_INVENTORY, __user->id(),
                    ui->deDate->date(), QDate::currentDate(), QTime::currentTime(), 0,
                    ui->leTotal->getDouble(), ui->leComment->text(),
                    0, __c5config.getValue(param_default_currency).toInt());
    db[":f_document"] = fInternalID;
    db.exec("delete from a_store_inventory where f_document=:f_document");
    QString longsql = "insert into a_store_inventory (f_id, f_document, f_store, f_goods, f_qty, f_price, f_total) values ";
    bool f = true;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (f) {
            f = false;
        } else {
            longsql.append(",");
        }
        QString id = QUuid::createUuid().toString().replace("{", "").replace("}", "");
        longsql.append(QString("('%1', '%2', %3, %4, %5, %6, %7)")
                       .arg(id)
                       .arg(fInternalID)
                       .arg(ui->leStore->getInteger())
                       .arg(ui->tblGoods->getInteger(i, 1))
                       .arg(ui->tblGoods->lineEdit(i, 3)->getDouble())
                       .arg(ui->tblGoods->lineEdit(i, 5)->getDouble())
                       .arg(ui->tblGoods->lineEdit(i, 6)->getDouble()));
        //        QString id = ui->tblGoods->getString(i, 0);
        //        dw.writeAStoreInventory(id, fInternalID, ui->leStore->getInteger(), ui->tblGoods->getInteger(i, 1),
        //                                ui->tblGoods->lineEdit(i, 3)->getDouble(),  ui->tblGoods->lineEdit(i, 5)->getDouble(),
        //                                ui->tblGoods->lineEdit(i, 6)->getDouble());
        ui->tblGoods->setString(i, 0, id);
    }
    if (!db.exec(longsql)) {
        C5Message::error(db.fLastError);
        return;
    }
    C5Message::info(tr("Saved"));
}

void C5StoreInventory::printDoc()
{
    if (ui->leDocNum->text().isEmpty()) {
        C5Message::error(tr("Document is not saved"));
        return;
    }
    C5Printing p;
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2000, 2700, QPageLayout::Portrait);
    p.setFontSize(25);
    p.setFontBold(true);
    QString docTypeText = tr("Store inventorization");
    p.ctext(QString("%1 N%2").arg(docTypeText).arg(ui->leDocNum->text()));
    p.br();
    p.br();
    p.setFontSize(20);
    p.setFontBold(false);
    QString storeInName = ui->leStoreName->text();
    p.br();
    p.setFontBold(true);
    points.clear();
    points << 50 << 200;
    vals << tr("Date");
    if (!storeInName.isEmpty()) {
        vals << tr("Store, input");
        points << 400;
    }
    if (!ui->leComment->isEmpty()) {
        vals << tr("Comment");
        points << 600;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    vals.clear();
    vals << ui->deDate->text();
    if (!storeInName.isEmpty()) {
        vals << ui->leStoreName->text();
    }
    if (!ui->leComment->isEmpty()) {
        vals << ui->leComment->text();
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    points.clear();
    vals.clear();
    p.setFontBold(true);
    points << 50;
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    points.clear();
    vals.clear();
    points << 50;
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br();
    p.br();
    p.br();
    points.clear();
    points << 50 << 100 << 200 << 600 << 250 << 250 << 250 << 270;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << tr("Goods")
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");
    p.setFontBold(true);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
            p.br();
        }
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, 1);
        vals << ui->tblGoods->getString(i, 2);
        vals << ui->tblGoods->lineEdit(i, 3)->text();
        vals << ui->tblGoods->getString(i, 4);
        vals << ui->tblGoods->lineEdit(i, 5)->text();
        vals << ui->tblGoods->lineEdit(i, 6)->text();
        p.tableText(points, vals, p.fLineHeight + 20);
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
        }
        p.br(p.fLineHeight + 20);
    }
    points.clear();
    points << 1200
           << 500
           << 270;
    vals.clear();
    vals << tr("Total amount");
    vals << ui->leTotal->text();
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.line(50, p.fTop, 700, p.fTop);
    p.line(1000, p.fTop, 1650, p.fTop);
    C5PrintPreview pp( &p);
    pp.exec();
}

void C5StoreInventory::on_btnAddGoods_clicked()
{
    QJsonArray vals;
    if (!C5Selector::getValueOfColumn(cache_goods, vals, 3)) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setString(row, 1, vals.at(1).toString());
    ui->tblGoods->setString(row, 2, vals.at(3).toString());
    ui->tblGoods->setString(row, 4, vals.at(4).toString());
    ui->tblGoods->lineEdit(row, 5)->setDouble(vals.at(6).toDouble());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
}

int C5StoreInventory::addGoodsRow()
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
    lprice->setValidator(new QDoubleValidator(0, 100000000, 2));
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, 6);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    connect(lqty, SIGNAL(textChanged(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textChanged(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textChanged(QString)), this, SLOT(tblTotalChanged(QString)));
    connect(lqty, SIGNAL(keyPressed(QChar)), this, SLOT(keyPressed(QChar)));
    connect(lprice, SIGNAL(keyPressed(QChar)), this, SLOT(keyPressed(QChar)));
    connect(ltotal, SIGNAL(keyPressed(QChar)), this, SLOT(keyPressed(QChar)));
    return row;
}

void C5StoreInventory::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 6)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

bool C5StoreInventory::docCheck(QString &err)
{
    if (ui->leStore->getInteger() == 0) {
        err += tr("Input store is not defined") + "<br>";
    }
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Cannot save an emtpy document") + "<br>";
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->lineEdit(i, 3)->getDouble() < 0.001) {
            err += QString("%1 #%2, %3, missing quantity").arg(tr("Row")).arg(i + 1).arg(ui->tblGoods->getString(i, 2)) + "<br>";
        }
    }
    return err.isEmpty();
}

void C5StoreInventory::on_btnRemoveGoods_clicked()
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

void C5StoreInventory::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget * >(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col + 2);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col + 3);
    ltotal->setDouble(lqty->getDouble() *lprice->getDouble());
    countTotal();
}

void C5StoreInventory::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget * >(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col - 2);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col + 1);
    ltotal->setDouble(lqty->getDouble() *lprice->getDouble());
    countTotal();
}

void C5StoreInventory::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget * >(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col - 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col - 1);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col);
    if (lqty->getDouble() > 0.001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    }
    countTotal();
}

void C5StoreInventory::on_btnNew_clicked()
{
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot add goods without code"));
            return;
        }
        int row = addGoodsRow();
        ui->tblGoods->setData(row, 1, data.at(0)["f_id"]);
        ui->tblGoods->setData(row, 2, data.at(0)["f_name"].toString() + " " + data.at(0)["f_scancode"].toString());
        ui->tblGoods->setData(row, 4, data.at(0)["f_unitname"]);
        ui->tblGoods->lineEdit(row, 3)->setFocus();
    }
    delete e;
}

void C5StoreInventory::on_leSearch_textChanged(const QString &arg1)
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->setRowHidden(i, !ui->tblGoods->getString(i, 2).contains(arg1));
    }
}

void C5StoreInventory::on_btnCloseSearch_clicked()
{
    ui->wSearch->setVisible(false);
}

void C5StoreInventory::on_leFind_textChanged(const QString &arg1)
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        bool rh = (!ui->tblGoods->getString(i, 1).contains(arg1, Qt::CaseInsensitive)
                   && !ui->tblGoods->getString(i, 2).contains(arg1, Qt::CaseInsensitive)) ;
        ui->tblGoods->setRowHidden(i, rh);
    }
}

void C5StoreInventory::on_btnBroadcast_clicked(bool checked)
{
    if (checked) {
        ui->btnBroadcast->setIcon(QIcon(":/wifi.png"));
    } else {
        ui->btnBroadcast->setIcon(QIcon(":/wifib.png"));
    }
}

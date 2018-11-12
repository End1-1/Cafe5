#include "c5storeinventory.h"
#include "ui_c5storeinventory.h"
#include "c5selector.h"
#include "c5cache.h"

C5StoreInventory::C5StoreInventory(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5StoreInventory)
{
    ui->setupUi(this);
    fIcon = ":/goods.png";
    fLabel = tr("Store document");
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store);
    ui->tblGoods->setColumnWidths(7, 0, 0, 300, 80, 80, 80, 80);
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

bool C5StoreInventory::openDoc(int id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from a_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid document number"));
        return false;
    }
    ui->leDocNum->setText(db.getString("f_id"));
    ui->deDate->setDate(db.getDate("f_date"));
    ui->leComment->setText(db.getString("f_comment"));
    db[":f_document"] = id;
    db.exec("select d.f_id, d.f_goods, g.f_name, d.f_qty, u.f_name, d.f_price, d.f_total, d.f_store \
            from a_store_inventory d \
            left join c_goods g on g.f_id=d.f_goods \
            left join c_units u on u.f_id=g.f_unit \
            where d.f_document=:f_document ");
    while (db.nextRow()) {
        int row = addGoodsRow();
        ui->leStore->setValue(db.getString("f_store"));
        ui->tblGoods->setInteger(row, 0, db.getInt(0));
        ui->tblGoods->setInteger(row, 1, db.getInt(1));
        ui->tblGoods->setString(row, 2, db.getString(2));
        ui->tblGoods->lineEdit(row, 3)->setDouble(db.getDouble(3));
        ui->tblGoods->setString(row, 4, db.getString(4));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble(5));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble(6));
    }
    return true;
}

bool C5StoreInventory::removeDoc(const QStringList &dbParams, int id)
{
    C5Database db(dbParams);
    db[":f_document"] = id;
    db.exec("delete from a_store_invenetory where f_document=:f_document");
    db[":f_id"] = id;
    db.exec("delete from a_header where f_id=:f_id");
    return true;
}

void C5StoreInventory::saveDoc()
{
    C5Database db(fDBParams);
    QString err;
    if (!docCheck(err)) {
        C5Message::error(err);
        return;
    }

    if (ui->leDocNum->getInteger() == 0) {
        db[":f_id"] = 0;
        ui->leDocNum->setInteger(db.insert("a_header"));
    }
    db[":f_state"] = DOC_STATE_SAVED;
    db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
    db[":f_operator"] = __userid;
    db[":f_date"] = ui->deDate->date();
    db[":f_createDate"] = QDate::currentDate();
    db[":f_createTime"] = QTime::currentTime();
    db[":f_partner"] = 0;
    db[":f_amount"] = ui->leTotal->getDouble();
    db[":f_comment"] = ui->leComment->text();
    db[":f_raw"] = "";
    db.update("a_header", where_id(ui->leDocNum->getInteger()));

    db[":f_document"] = ui->leDocNum->getInteger();
    db.exec("delete from a_store_inventory where f_document=:f_document");
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = 0;
        ui->tblGoods->setInteger(i, 0, db.insert("a_store_inventory"));
        db[":f_document"] = ui->leDocNum->getInteger();
        db[":f_store"] = ui->leStore->getInteger();
        db[":f_goods"] = ui->tblGoods->getInteger(i, 1);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        db[":f_total"] = ui->tblGoods->lineEdit(i, 6)->getDouble();
        db.update("a_store_inventory", where_id(ui->tblGoods->getInteger(i, 0)));
    }

    C5Message::info(tr("Saved"));
}

void C5StoreInventory::printDoc()
{

}

void C5StoreInventory::on_btnAddGoods_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_goods, vals)) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setString(row, 1, vals.at(0).toString());
    ui->tblGoods->setString(row, 2, vals.at(2).toString());
    ui->tblGoods->setString(row, 4, vals.at(3).toString());
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
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col + 2);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col + 3);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void C5StoreInventory::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col - 2);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col + 1);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void C5StoreInventory::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
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

#include "c5saledoc.h"
#include "ui_c5saledoc.h"
#include "c5utils.h"

#define col_uuid 0
#define col_checkbox 1
#define col_goods_code 2
#define col_barcode 3
#define col_name 4
#define col_qty 5
#define col_unit 6
#define col_price 7
#define col_amount 8
#define col_discount_type 9
#define col_discount_value 10
#define col_discount_amount 11
#define col_grandtotal 12

C5SaleDoc::C5SaleDoc(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5SaleDoc)
{
    ui->setupUi(this);
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 50, 0, 200, 300, 80, 80, 80, 80, 0, 0, 0, 0);
    ui->cbCurrency->setDBValues(dbParams, "select f_id, f_name from e_currency");
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
}

C5SaleDoc::~C5SaleDoc()
{
    delete ui;
}

void C5SaleDoc::setMode(int mode)
{
    fMode = mode;
    switch (fMode) {
    case 1:
        break;
    case 2:
        break;
    }
}

void C5SaleDoc::on_PriceTextChanged(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int r, c;
    if (ui->tblGoods->findWidget(l, r, c)) {
        ui->tblGoods->lineEdit(r, col_amount)->setDouble(str_float(arg1) * ui->tblGoods->lineEdit(r, col_qty)->getDouble());
        countGrandTotal();
    }
}

void C5SaleDoc::on_QtyTextChanged(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int r, c;
    if (ui->tblGoods->findWidget(l, r, c)) {
        ui->tblGoods->lineEdit(r, col_amount)->setDouble(str_float(arg1) * ui->tblGoods->lineEdit(r, col_price)->getDouble());
        countGrandTotal();
    }
}

void C5SaleDoc::on_TotalTextChanged(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int r, c;
    if (ui->tblGoods->findWidget(l, r, c)) {
        if (ui->tblGoods->lineEdit(r, col_qty)->getDouble() > 0.0001) {
            ui->tblGoods->lineEdit(r, col_price)->setDouble(str_float(arg1) / ui->tblGoods->lineEdit(r, col_qty)->getDouble());
            countGrandTotal();
        }
    }
}

void C5SaleDoc::on_leCmd_returnPressed()
{
    QString code = ui->leCmd->text().replace("?", "").replace(";", "");
    ui->leCmd->clear();
    C5Database db(fDBParams);
    db[":f_scancode"] = code;
    db.exec("select f_id from c_goods where f_scancode=:f_scancode");
    if (db.nextRow()) {
        addGoods(db.getInt(0), db);
        return;
    }
}

void C5SaleDoc::addGoods(int id, C5Database &db)
{
    int r = ui->tblGoods->addEmptyRow();
    db[":f_id"] = id;
    db.exec("select g.*, gu.f_name as f_unitname from c_goods g "
            "left join c_units gu on gu.f_id=g.f_unit "
            "where g.f_id=:f_id");
    db.nextRow();
    ui->tblGoods->setString(r, col_uuid, "");
    ui->tblGoods->createCheckbox(r, col_checkbox);
    ui->tblGoods->setInteger(r, col_goods_code, db.getInt("f_id"));
    ui->tblGoods->setString(r, col_barcode, db.getString("f_scancode"));
    ui->tblGoods->setString(r, col_name, db.getString("f_name"));
    ui->tblGoods->createLineEdit(r, col_qty);
    ui->tblGoods->setString(r, col_unit, db.getString("f_unitname"));
    ui->tblGoods->createLineEdit(r, col_price);
    ui->tblGoods->createLineEdit(r, col_amount);
    ui->tblGoods->setString(r, col_discount_type, "");
    ui->tblGoods->setString(r, col_discount_value, "");
    ui->tblGoods->setDouble(r, col_discount_amount, 0);
    ui->tblGoods->setDouble(r, col_grandtotal, 0);
    connect(ui->tblGoods->lineEdit(r, col_qty), &C5LineEdit::textEdited, this, &C5SaleDoc::on_QtyTextChanged);
    connect(ui->tblGoods->lineEdit(r, col_price), &C5LineEdit::textEdited, this, &C5SaleDoc::on_PriceTextChanged);
    connect(ui->tblGoods->lineEdit(r, col_amount), &C5LineEdit::textEdited, this, &C5SaleDoc::on_TotalTextChanged);
    db[":f_currency"] = ui->cbCurrency->currentData();
    db[":f_goods"] = id;
    QString priceField;
    switch (fMode) {
    case PRICEMODE_RETAIL:
        priceField = "f_price1";
        break;
    case PRICEMODE_WHOSALE:
        priceField = "f_price2";
        break;
    }
    db.exec(QString("select %1 from c_goods_prices where f_goods=:f_goods and f_currency=:f_currency").arg(priceField));
    if (db.nextRow()) {
        ui->tblGoods->lineEdit(r, col_price)->setDouble(db.getDouble(priceField));
    }
}

void C5SaleDoc::countGrandTotal()
{

}

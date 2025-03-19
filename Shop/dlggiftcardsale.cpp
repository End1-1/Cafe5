#include "dlggiftcardsale.h"
#include "ui_dlggiftcardsale.h"

DlgGiftCardSale::DlgGiftCardSale(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgGiftCardSale)
{
    ui->setupUi(this);
}

DlgGiftCardSale::~DlgGiftCardSale()
{
    delete ui;
}

void DlgGiftCardSale::on_leCode_returnPressed()
{
    ui->leCode->setText(ui->leCode->text().replace(";", "").replace("?", ""));
    C5Database db(__c5config.dbParams());
    db[":f_code"] = ui->leCode->text();
    db.exec("select * from b_gift_card where f_code=:f_code");
    if (db.nextRow() == false) {
        C5Message::error(tr("Invalid code"));
        return;
    }
    if (db.getDate("f_datesaled").isValid()) {
        C5Message::error(tr("This card already has been sold"));
        return;
    }
    db[":f_scancode"] = ui->leCode->text().right(4);
    db.exec("select * from c_goods where f_scancode=:f_scancode");
    if (db.nextRow() == false) {
        C5Message::error(tr("No reference for goods table"));
        return;
    }
    fGiftGoodsId = db.getInt("f_id");
    db.exec("select * from b_gift_card_sale_options");
    if (db.nextRow()) {
        QStringList prices = db.getString("f_value").split(",", Qt::SkipEmptyParts);
        for (const QString &s : prices) {
            QListWidgetItem *item = new QListWidgetItem(ui->lstPrices);
            item->setText(float_str(str_float(s), 2));
            item->setSizeHint(QSize(300, 50));
            ui->lstPrices->addItem(item);
        }
    }
    ui->lbCard->setText(ui->leCode->text().right(4));
    fGiftScancode = ui->lbCard->text();
}

void DlgGiftCardSale::on_btnRegister_clicked()
{
    if (ui->lstPrices->currentRow() < 0) {
        C5Message::error(tr("No price selected"));
        return;
    }
    C5Database db(__c5config.dbParams());
    db[":f_code"] = ui->leCode->text();
    db.exec("select * from b_gift_card where f_code=:f_code for update");
    if (db.nextRow() == false) {
        C5Message::error(tr("Invalid code"));
        return;
    }
    if (db.getDate("f_datesaled").isValid()) {
        C5Message::error(tr("This card already has been sold"));
        return;
    }
    fGiftPrice = str_float(ui->lstPrices->currentItem()->text());
    accept();
}

void DlgGiftCardSale::on_btnCancel_clicked()
{
    reject();
}

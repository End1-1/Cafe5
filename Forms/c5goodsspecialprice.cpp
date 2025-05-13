#include "c5goodsspecialprice.h"
#include "ui_c5goodsspecialprice.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5message.h"
#include "c5config.h"

C5GoodsSpecialPrice::C5GoodsSpecialPrice(QWidget *parent)
    : C5Dialog(__c5config.dbParams())
    , ui(new Ui::C5GoodsSpecialPrice)
{
    ui->setupUi(this);
}

C5GoodsSpecialPrice::~C5GoodsSpecialPrice()
{
    delete ui;
}

void C5GoodsSpecialPrice::on_leSpecialPrice_textChanged(const QString &arg1)
{
    ui->leRetailPriceDiff->setDouble(((arg1.toDouble() * 100) / ui->leRetailPrice->getDouble()) - 100);
    ui->leWhosalePriceDiff->setDouble(((arg1.toDouble() * 100) / ui->leWhosalePrice->getDouble()) - 100);
}

void C5GoodsSpecialPrice::on_btnOK_clicked()
{
    accept();
}

void C5GoodsSpecialPrice::on_btnCancel_clicked()
{
    reject();
}

void C5GoodsSpecialPrice::on_btnSelectPartner_clicked()
{
    QJsonArray values;
    if (!C5Selector::getValue(fDBParams, cache_goods_partners, values)) {
        return;
    }
    if (values.count() == 0) {
        return;
    }
    ui->lePartnerID->setInteger(values.at(1).toInteger());
    ui->lePartnerTIN->setText(values.at(9).toString());
    ui->lePartnerName->setText(values.at(2).toString());
    ui->lePartnerAddress->setText(values.at(3).toString());
}

void C5GoodsSpecialPrice::on_btnSelectGoods_clicked()
{
    QJsonArray vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods, vals, 3)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    ui->leGoodsID->setInteger(vals.at(1).toInt());
    ui->leGoodsGroup->setText(vals.at(2).toString());
    ui->leGoodsName->setText(vals.at(3).toString());
    ui->leBarcode->setText(vals.at(5).toString());
    ui->leRetailPrice->setDouble(vals.at(9).toDouble());
    ui->leWhosalePrice->setDouble(vals.at(10).toDouble());
    on_leSpecialPrice_textChanged(ui->leSpecialPrice->text());
}

#include "c5discountredeem.h"
#include "ui_c5discountredeem.h"
#include "c5cache.h"
#include "c5utils.h"
#include "c5message.h"

C5DiscountRedeem::C5DiscountRedeem(const QString &id, int partnerId)
    : C5Dialog(), ui(new Ui::C5DiscountRedeem)
{
    ui->setupUi(this);
    ui->leTransaction->setText(id);
    ui->lePartner->setSelector(ui->lePartnerName, cache_goods_partners);
    ui->leCard->setSelector(ui->leCardName, cache_discount_cards);
    ui->leCard->setCallbackDialog(this);

    if(id.isEmpty() == false) {
        ui->lePartner->setEnabled(false);
    }

    if(partnerId > 0) {
        ui->lePartner->setValue(partnerId);
    }
}

C5DiscountRedeem::~C5DiscountRedeem() { delete ui; }

void C5DiscountRedeem::selectorCallback(int selector, const QJsonArray &data)
{
    switch(selector) {
    case cache_discount_cards:
        processPartner(data);
        break;
    }
}

void C5DiscountRedeem::on_btnCancel_clicked()
{
    reject();
}

void C5DiscountRedeem::on_btnRedeem_clicked()
{
    QString err;

    if(ui->lePartner->getInteger() == 0) {
        err += tr("Partner must be selected")  + "<br>";
    }

    if(ui->leAmountToRedeem->getDouble() < 0.1) {
        err += tr("Amount must be greater then zero") + "<br>";
    }

    if(!err.isEmpty()) {
        C5Message::error(err);
        return;
    }

    fHttp->createHttpQueryLambda("/engine/v2/officen/discount-statistic/redeem",
    { {"partner", ui->lePartner->getInteger()}},
    [this](const QJsonObject & jo) {
        accept();
    },
    [this](const QJsonObject je) {
    });
}

void C5DiscountRedeem::processPartner(const QJsonArray &data)
{
    fHttp->createHttpQueryLambda("/engine/v2/officen/discount-statistic/get-balance", {
        {"partner", ui->lePartner->getInteger()},
        {"card", ui->leCard->getInteger()}
    },
    [this](const QJsonObject & jo) {
        QJsonObject jb = jo["data"].toObject();
        ui->leCurrentBalance->setDouble(jb["f_balance"].toDouble());
    },
    [this](const QJsonObject je) {
    });
}

void C5DiscountRedeem::on_leAmountToRedeem_textChanged(const QString &arg1)
{
    if(str_float(arg1) > ui->leCurrentBalance->getDouble()) {
        ui->leAmountToRedeem->setDouble(ui->leCurrentBalance->getDouble());
    }
}

#include "rworkstationconfigcommon.h"
#include "ui_rworkstationconfigcommon.h"

RWorkstationConfigCommon::RWorkstationConfigCommon(QWidget *parent)
    : RWorkstationConfigWidget(parent)
    , ui(new Ui::RWorkstationConfigCommon)
{
    auto *form = new QWidget(this);
    ui->setupUi(form);
    addFormWidget(form);
}

RWorkstationConfigCommon::~RWorkstationConfigCommon()
{
    delete ui;
}

void RWorkstationConfigCommon::applyConfig(const QJsonObject &config)
{
    ui->lePresentCard->setText(config.value(QStringLiteral("f_present_card_pattern")).toString());
    ui->leDiscountCard->setText(config.value(QStringLiteral("f_discount_card_pattern")).toString());
    ui->leAccumulateCard->setText(config.value(QStringLiteral("f_accumulate_card_pattern")).toString());
    ui->leScalePattern->setText(config.value(QStringLiteral("f_scale_pattern")).toString());
    ui->leQuickDebtPartner->setText(QString::number(config.value(QStringLiteral("f_quick_debt_partner_id")).toInt()));
    ui->lePrintServer->setText(config.value(QStringLiteral("print_sssserver")).toString());
    ui->chInputCashboxBeforeClose->setChecked(config.value(QStringLiteral("input_cashbox_amount_before_close")).toBool(true));
    ui->chBistro->setChecked(config.value(QStringLiteral("bistro")).toBool());
}

QJsonObject RWorkstationConfigCommon::collectConfig() const
{
    QJsonObject jo;
    jo.insert(QStringLiteral("f_present_card_pattern"), ui->lePresentCard->text().trimmed());
    jo.insert(QStringLiteral("f_discount_card_pattern"), ui->leDiscountCard->text().trimmed());
    jo.insert(QStringLiteral("f_accumulate_card_pattern"), ui->leAccumulateCard->text().trimmed());
    jo.insert(QStringLiteral("f_scale_pattern"), ui->leScalePattern->text().trimmed());
    jo.insert(QStringLiteral("f_quick_debt_partner_id"), ui->leQuickDebtPartner->text().trimmed().toInt());
    jo.insert(QStringLiteral("print_sssserver"), ui->lePrintServer->text().trimmed());
    jo.insert(QStringLiteral("input_cashbox_amount_before_close"), ui->chInputCashboxBeforeClose->isChecked());
    jo.insert(QStringLiteral("bistro"), ui->chBistro->isChecked());
    return jo;
}

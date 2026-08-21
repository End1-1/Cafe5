#include "rworkstationconfigshop.h"
#include "ui_rworkstationconfigshop.h"

RWorkstationConfigShop::RWorkstationConfigShop(QWidget *parent)
    : RWorkstationConfigWidget(parent)
    , ui(new Ui::RWorkstationConfigShop)
{
    mForm = new QWidget(this);
    ui->setupUi(mForm);
    addFormWidget(mForm);
}

RWorkstationConfigShop::~RWorkstationConfigShop()
{
    delete ui;
}

void RWorkstationConfigShop::applyConfig(const QJsonObject &config)
{
    ui->leCashboxId->setText(QString::number(config.value(QStringLiteral("f_cashbox_id")).toInt()));
    ui->leHallId->setText(QString::number(config.value(QStringLiteral("f_default_hall_id")).toInt()));
    ui->leFiscalId->setText(QString::number(config.value(QStringLiteral("f_fiscal_machine_id")).toInt()));
    ui->leStoreId->setText(QString::number(config.value(QStringLiteral("f_default_store_id")).toInt()));
    ui->leReceiptPhone->setText(config.value(QStringLiteral("receipt_phone")).toString());
    ui->leReceiptPrinter->setText(config.value(QStringLiteral("receipt_printer")).toString());
    ui->chReceiptNoTable->setChecked(config.value(QStringLiteral("receipt_no_table")).toBool(false));
    ui->chReceiptNoServiceHint->setChecked(config.value(QStringLiteral("receipt_no_service_hint")).toBool(false));
    ui->chReceiptNoDiscountHint->setChecked(config.value(QStringLiteral("receipt_no_discount_hint")).toBool(false));
    ui->chAssignSaleToAssociate->setChecked(config.value(QStringLiteral("assign_sale_to_associate")).toBool(false));
    ui->leArcusPort->setText(QString::number(config.value(QStringLiteral("arcus_port")).toInt()));
    ui->leArcustAddress->setText(config.value(QStringLiteral("arcus_address")).toString());
    ui->leArcusKey->setText(config.value(QStringLiteral("arcus_key")).toString());
}

QJsonObject RWorkstationConfigShop::collectConfig() const
{
    QJsonObject jo;
    jo.insert(QStringLiteral("f_cashbox_id"), ui->leCashboxId->text().trimmed().toInt());
    jo.insert(QStringLiteral("f_default_hall_id"), ui->leHallId->text().trimmed().toInt());
    jo.insert(QStringLiteral("f_fiscal_machine_id"), ui->leFiscalId->text().trimmed().toInt());
    jo.insert(QStringLiteral("f_default_store_id"), ui->leStoreId->text().trimmed().toInt());
    jo.insert(QStringLiteral("receipt_phone"), ui->leReceiptPhone->text().trimmed());
    jo.insert(QStringLiteral("receipt_printer"), ui->leReceiptPrinter->text().trimmed());
    jo.insert(QStringLiteral("receipt_no_table"), ui->chReceiptNoTable->isChecked());
    jo.insert(QStringLiteral("receipt_no_service_hint"), ui->chReceiptNoServiceHint->isChecked());
    jo.insert(QStringLiteral("receipt_no_discount_hint"), ui->chReceiptNoDiscountHint->isChecked());
    jo.insert(QStringLiteral("assign_sale_to_associate"), ui->chAssignSaleToAssociate->isChecked());
    jo.insert(QStringLiteral("arcus_port"), ui->leArcusPort->text().trimmed().toInt());
    jo.insert(QStringLiteral("arcus_address"), ui->leArcustAddress->text().trimmed());
    jo.insert(QStringLiteral("arcus_key"), ui->leArcusKey->text().trimmed());
    return jo;
}

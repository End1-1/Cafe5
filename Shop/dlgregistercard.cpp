#include "dlgregistercard.h"
#include "ui_dlgregistercard.h"
#include "c5selector.h"
#include "c5cache.h"

DlgRegisterCard::DlgRegisterCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRegisterCard)
{
    ui->setupUi(this);
    ui->leCode->setFocus();
}

DlgRegisterCard::~DlgRegisterCard()
{
    delete ui;
}

void DlgRegisterCard::on_toolButton_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValue(__c5config.dbParams(), cache_goods_partners, values)) {
        return;
    }
    if (values.count() == 0) {
        return;
    }
    ui->lePartner->setText(values.at(2).toString());
}


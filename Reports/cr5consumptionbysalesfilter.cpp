#include "cr5consumptionbysalesfilter.h"
#include "ui_cr5consumptionbysalesfilter.h"
#include "c5cache.h"
#include "c5config.h"

CR5ConsumptionBySalesFilter::CR5ConsumptionBySalesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5ConsumptionBySalesFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group);
    ui->chDraft->setChecked(__c5config.getRegValue("draftconsuption", false).toBool());
}

CR5ConsumptionBySalesFilter::~CR5ConsumptionBySalesFilter()
{
    delete ui;
}

QString CR5ConsumptionBySalesFilter::condition()
{
    return "";
}

int CR5ConsumptionBySalesFilter::store()
{
    return ui->leStore->getInteger();
}

bool CR5ConsumptionBySalesFilter::salesCounted()
{
    return ui->chSalesCounted->isChecked();
}

bool CR5ConsumptionBySalesFilter::draft()
{
    return ui->chDraft->isChecked();
}

QDate CR5ConsumptionBySalesFilter::date1()
{
    return ui->deStart->date();
}

QDate CR5ConsumptionBySalesFilter::date2()
{
    return ui->deEnd->date();
}

QString CR5ConsumptionBySalesFilter::group() const
{
    return ui->leGroup->text();
}

int CR5ConsumptionBySalesFilter::reportType()
{
    if (ui->rbQty->isChecked()) {
        return REPORTTYPE_QUANTITEIS;
    }
    if (ui->rbAmount->isChecked()) {
        return REPORTTYPE_AMOUNTS;
    }
    return 0;
}

void CR5ConsumptionBySalesFilter::on_chDraft_clicked(bool checked)
{
    __c5config.setRegValue("draftconsuption", checked);
}

#include "cr5materialinstorefilter.h"
#include "ui_cr5materialinstorefilter.h"
#include "c5cache.h"

CR5MaterialInStoreFilter::CR5MaterialInStoreFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5MaterialInStoreFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->leGroup->setSelector(fDBParams, ui->leGroupname, cache_goods_group).setMultiselection(true);
    ui->leMaterial->setSelector(fDBParams, ui->leMaterialName, cache_goods, 1, 3).setMultiselection(true);
    ui->leUnit->setSelector(fDBParams, ui->leUnitName, cache_goods_unit).setMultiselection(true);
    ui->leClass1->setSelector(fDBParams, ui->leClass1Name, cache_goods_classes).setMultiselection(true);
    ui->leClass2->setSelector(fDBParams, ui->leClass2Name, cache_goods_classes).setMultiselection(true);
    ui->leClass3->setSelector(fDBParams, ui->leClass3Name, cache_goods_classes).setMultiselection(true);
    ui->leClass4->setSelector(fDBParams, ui->leClass4Name, cache_goods_classes).setMultiselection(true);
    ui->leCurrency->setSelector(fDBParams, ui->leCurrencyName, cache_currency);
    ui->lePartner->setSelector(fDBParams, ui->lePartnerName, cache_goods_partners);
    restoreFilter(this);
}

CR5MaterialInStoreFilter::~CR5MaterialInStoreFilter()
{
    delete ui;
}

QString CR5MaterialInStoreFilter::condition()
{
    QString cond = QString(" h.f_date<=%1 ").arg(ui->deDate->toMySQLDate());
    if (ui->chShowDrafts->isChecked()) {
        cond += " and (h.f_state=2 or h.f_state=1) ";
    } else {
        cond += " and h.f_state=1 ";
    }
    if (!ui->leStore->isEmpty()) {
        cond += " and s.f_store in (" + ui->leStore->text() + ") ";
    }
    if (!ui->leGroup->isEmpty()) {
        cond += " and gg.f_id in (" + ui->leGroup->text() + ") ";
    }
    if (!ui->leMaterial->isEmpty()) {
        cond += " and s.f_goods in (" + ui->leMaterial->text() + ") ";
    }
    if (!ui->leClass1->isEmpty()) {
        cond += " and g.f_group1 in (" + ui->leClass1->text() + ") ";
    }
    if (!ui->leClass2->isEmpty()) {
        cond += " and g.f_group2 in (" + ui->leClass2->text() + ") ";
    }
    if (!ui->leClass3->isEmpty()) {
        cond += " and g.f_group3 in (" + ui->leClass3->text() + ") ";
    }
    if (!ui->leClass4->isEmpty()) {
        cond += " and g.f_group4 in (" + ui->leClass4->text() + ") ";
    }
    if (!ui->leUnit->isEmpty()) {
        cond += " and g.f_unit in (" + ui->leUnit->text() + ") ";
    }
    in(cond, "h.f_currency", ui->leCurrency);
    in(cond, "gpr.f_currency", ui->leCurrency);
    in(cond, "h.f_partner", ui->lePartner);
    return cond;
}

QString CR5MaterialInStoreFilter::filterText()
{
    QString s = QString("%1 %2").arg(tr("Date"), ui->deDate->text());
    inFilterText(s, ui->leStoreName);
    inFilterText(s, ui->leGroupname);
    inFilterText(s, ui->leMaterialName);
    inFilterText(s, ui->leUnitName);
    if (ui->chShowDrafts->isChecked()) {
        s += ", " + tr("Show drafts");
    }
    return s;
}

bool CR5MaterialInStoreFilter::showDrafts()
{
    return ui->chShowDrafts->isChecked();
}

bool CR5MaterialInStoreFilter::showZero()
{
    return ui->chShowZeroes->isChecked();
}

QString CR5MaterialInStoreFilter::currency() const
{
    return ui->leCurrency->text();
}

QString CR5MaterialInStoreFilter::currencyName() const
{
    return ui->leCurrencyName->text();
}

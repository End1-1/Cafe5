#include "cr5materialmoveuncomplectfilter.h"
#include "ui_cr5materialmoveuncomplectfilter.h"
#include "c5cache.h"

CR5MaterialmoveUncomplectFilter::CR5MaterialmoveUncomplectFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5MaterialmoveUncomplectFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->leGroup->setSelector(fDBParams, ui->leGroupname, cache_goods_group).setMultiselection(true);
    ui->leMaterial->setSelector(fDBParams, ui->leMaterialName, cache_goods, 1, 3).setMultiselection(true);
    ui->leUnit->setSelector(fDBParams, ui->leUnitName, cache_goods_unit).setMultiselection(true);
    ui->leDocType->setSelector(fDBParams, ui->leDocTypeName, cache_doc_type).setMultiselection(true);
    ui->leAction->setSelector(fDBParams, ui->leActionName, cache_store_inout);
}

CR5MaterialmoveUncomplectFilter::~CR5MaterialmoveUncomplectFilter()
{
    delete ui;
}

QString CR5MaterialmoveUncomplectFilter::condition()
{
    return "";
}

QString CR5MaterialmoveUncomplectFilter::cond()
{
    QString cond = QString(" where h.f_date between %1 and %2 and h.f_state=1 ").arg(ui->deDate1->toMySQLDate()).arg(ui->deDate2->toMySQLDate());
    if (!store().isEmpty()) {
        cond += QString(" and s.f_store in (%1) ").arg(store());
    }
    if (!group().isEmpty()) {
        cond += QString(" and gg.f_id in (%1) ").arg(group());
    }
    if (!goods().isEmpty()) {
        cond += QString(" and g.f_id in (%1) ").arg(goods());
    }
    if (!ui->leDocType->isEmpty()) {
        cond += QString(" and h.f_type in (%1) ").arg(ui->leDocType->text());
    }
    if (!ui->leAction->isEmpty()) {
        cond += QString(" and s.f_type=%1").arg(ui->leAction->text());
    }
    return cond;
}

const QString CR5MaterialmoveUncomplectFilter::unit() const
{
    return ui->leUnit->text();
}

const QString CR5MaterialmoveUncomplectFilter::store() const
{
    return ui->leStore->text();
}

const QString CR5MaterialmoveUncomplectFilter::goods() const
{
    return ui->leMaterial->text();
}

const QString CR5MaterialmoveUncomplectFilter::group() const
{
    return ui->leGroup->text();
}

int CR5MaterialmoveUncomplectFilter::viewMode()
{
    if (ui->rbGoods->isChecked()) {
        return 0;
    }
    if (ui->rbGroups->isChecked()) {
        return 1;
    }
    return 0;
}

#include "cr5materialinstoreuncomplect.h"
#include "cr5materialinstoreuncomplectfilter.h"
#include "c5message.h"

CR5MaterialInStoreUncomplect::CR5MaterialInStoreUncomplect(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Materials in the store without complects");
    fSimpleQuery = true;
    fFilter = new CR5MaterialInStoreUncomplectFilter();
    fFilterWidget = fFilter;
    fTranslation["f_code"] = tr("Code");
    fTranslation["f_storage"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_total"] = tr("Total");
    fTranslation["f_totalsale"] = tr("Total retail");
    fTranslation["f_totalsale2"] = tr("Total whosale");
    fColumnsSum.append("f_qty");
    fColumnsSum.append("f_total");
    fColumnsSum.append("f_totalsale");
    fColumnsSum.append("f_totalsale2");
}

QToolBar* CR5MaterialInStoreUncomplect::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }

    return fToolBar;
}

void CR5MaterialInStoreUncomplect::buildQuery()
{
    if(fFilter->unit().isEmpty()) {
        C5Message::error(tr("Unit of complected goods must be defined"));
        return;
    }

    QString group;
    QString fields;

    switch(fFilter->viewMode()) {
    case 0:
        group = "group by f_code, f_storage, f_group, f_goods, f_scancode ";
        fields = "f_code, f_storage, f_group, f_goods, f_scancode,sum(f_qty) as f_qty, f_unit, "
                 "sum(f_total) as f_total,sum(f_totalsale) as f_totalsale,sum(f_totalsale2) as f_totalsale2 ";
        break;

    case 1:
        group = "group by f_storage, f_group ";
        fields = "f_storage, f_group, sum(f_qty) as f_qty,  "
                 "sum(f_total) as f_total, sum(f_totalsale) as f_totalsale,sum(f_totalsale2) as f_totalsale2 ";
        break;
    }

    QString cond;

    if(!fFilter->store().isEmpty()) {
        cond += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }

    if(!fFilter->group().isEmpty()) {
        cond += QString(" and gg.f_id in (%1)").arg(fFilter->group());
    }

    if(!fFilter->goods().isEmpty()) {
        cond += QString(" and g.f_id in (%1)").arg(fFilter->goods());
    }

    fSqlQuery = QString("select %5 from ( "
                        "select 'A', g.f_id as f_code,ss.f_name as f_storage,gg.f_name as f_group,g.f_name as f_goods, "
                        "g.f_scancode,sum(s.f_qty*s.f_type) as f_qty, "
                        "u.f_name as f_unit,sum(s.f_total*s.f_type) as f_total,"
                        "sum(s.f_qty*s.f_type)*if(gpr.f_price1disc>0, gpr.f_price1disc, gpr.f_price1) as f_totalsale, "
                        "sum(s.f_qty*s.f_type)*if(gpr.f_price2disc>0, gpr.f_price2disc, gpr.f_price2) as f_totalsale2  "
                        "from a_store s "
                        "inner join c_goods g on g.f_id=s.f_goods "
                        "inner join c_storages ss on ss.f_id=s.f_store "
                        "inner join c_groups gg on gg.f_id=g.f_group "
                        "inner join c_units u on u.f_id=g.f_unit and u.f_id not in(%2) "
                        "left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1 "
                        "inner join a_header h on h.f_id=s.f_document  where  h.f_date<='%1'  and h.f_state=1  %3 "
                        "group by 1, g.f_id,ss.f_name,gg.f_name,g.f_name,u.f_name having sum(s.f_qty*s.f_type) <> 0 "
                        "union all "
                        "select 'B', gc.f_goods as f_code,ss.f_name as f_storage,gg.f_name as f_group,g.f_name as f_goods, "
                        "g.f_scancode,sum(s.f_qty*s.f_type)*gc.f_qty as f_qty, "
                        //"u.f_name as f_unit,sum(((s.f_qty*s.f_type)*gc.f_qty)*g.f_lastinputprice*s.f_type) as f_total, "
                        "u.f_name as f_unit,sum(((s.f_qty*s.f_type)*gc.f_qty)*s.f_price*s.f_type) as f_total, "
                        "sum(s.f_qty*s.f_type)*if(gpr.f_price1disc>0, gpr.f_price1disc, gpr.f_price1) as f_totalsale, "
                        "sum(s.f_qty*s.f_type)*if(gpr.f_price2disc>0, gpr.f_price2disc, gpr.f_price2) as f_totalsale2  "
                        "from a_store s  "
                        "inner join c_goods_complectation gc on gc.f_base=s.f_goods "
                        "inner join c_goods g on g.f_id=gc.f_goods "
                        "inner join c_goods gu on gu.f_id=s.f_goods "
                        "left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1 "
                        "inner join c_storages ss on ss.f_id=s.f_store "
                        "inner join c_groups gg on gg.f_id=g.f_group  "
                        "inner join c_units u on u.f_id=g.f_unit "
                        "inner join a_header h on h.f_id=s.f_document  "
                        "where  h.f_date<='%1'  and h.f_state=1 and gu.f_unit in(%2) %3 "
                        "group by 1, gc.f_goods,g.f_id,ss.f_name,gg.f_name,g.f_name,u.f_name, gc.f_base "
                        "having sum(s.f_qty*s.f_type) <> 0 "
                        ") k %4 "
                       )
                .arg(fFilter->date().toString(FORMAT_DATE_TO_STR_MYSQL))
                .arg(fFilter->unit())
                .arg(cond)
                .arg(group)
                .arg(fields);
    C5ReportWidget::buildQuery();
}

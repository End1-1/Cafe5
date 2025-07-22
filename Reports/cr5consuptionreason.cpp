#include "cr5consuptionreason.h"
#include "cr5consuptionreasonfilter.h"
#include "c5tablemodel.h"

CR5ConsuptionReason::CR5ConsuptionReason(QWidget *parent) :
    C5ReportWidget(parent)
{
    fLabel = tr("Reason for consuption");
    fIcon = ":/goods.png";
//    fSimpleQuery = false;
//    fMainTable = "o_body ob";
//    fLeftJoinTables << "left join o_header oh on oh.f_id=ob.f_header [oh]"
//                    << "left join d_dish d on d.f_id=ob.f_dish [d]"
//                    << "left join d_part2 dp on dp.f_id=d.f_part [dp]"
//                    << "left join o_goods og on og.f_body=ob.f_id [og]"
//                    << "left join c_goods g on g.f_id=og.f_goods [g]"
//                    << "left join c_storages st on st.f_id=og.f_store [st]"
//                       ;
//    fColumnsFields << "oh.f_id"
//                   << "oh.f_datecash"
//                   << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
//                   << "dp.f_name as f_partname"
//                   << "d.f_name as f_dishname"
//                   << "g.f_name as f_goodsname"
//                   << "st.f_name as f_storename"
//                   << "og.f_qty as f_recipeqty"
//                   << "ob.f_qty1 as f_saleqty"
    ;
//    fColumnsGroup << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
//                   << "oh.f_id"
//                   << "oh.f_datecash"
//                   << "dp.f_name as f_partname"
//                   << "d.f_name as f_dishname"
//                   << "g.f_name as f_goodsname"
//                   << "og.f_qty as f_recipeqty"
//                      ;
    fColumnsSum << "f_recipeqty"
                << "f_saleqty"
                << "f_output"
                ;
    fTranslation["f_prefix"] = tr("Order");
    fTranslation["f_id"] = tr("UUID");
    fTranslation["f_datecash"] = tr("Date");
    fTranslation["f_partname"] = tr("Part");
    fTranslation["f_dishname"] = tr("Dish");
    fTranslation["f_goodsname"] = tr("Goods");
    fTranslation["f_storename"] = tr("Storage");
    fTranslation["f_recipeqty"] = tr("Qty, recipe");
    fTranslation["f_saleqty"] = tr("Qty, sale");
//    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_prefix"] = true;
//    fColumnsVisible["oh.f_id"] = true;
//    fColumnsVisible["os.f_name as f_statename"] = true;
//    fColumnsVisible["oh.f_datecash"] = true;
//    fColumnsVisible["dp.f_name as f_partname"] = true;
//    fColumnsVisible["d.f_name as f_dishname"] = true;
//    fColumnsVisible["g.f_name as f_goodsname"] = true;
//    fColumnsVisible["st.f_name as f_storename"] = true;
//    fColumnsVisible["og.f_qty as f_recipeqty"] = true;
//    fColumnsVisible["ob.f_qty1 as f_saleqty"] = true;
    //fOrderCondition = "order by oh.f_datecash, dp.f_name";
    restoreColumnsVisibility();
    fFilterWidget = new CR5ConsuptionReasonFilter();
    fFilter = static_cast<CR5ConsuptionReasonFilter*>(fFilterWidget);
}

QToolBar* CR5ConsuptionReason::toolBar()
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

void CR5ConsuptionReason::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();

    if(fColumnsVisible["oh.f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}

void CR5ConsuptionReason::setFilterParams(const QDate &d1, const QDate &d2, int store, int goods)
{
    fFilter->setDateFilter(d1, d2);
    fFilter->setStoreFilter(store);
    fFilter->setGoodsFilter(goods);
    buildQuery();
}

void CR5ConsuptionReason::buildQuery()
{
    fSimpleQuery = true;
//    fSqlQuery = "select oh.f_id,oh.f_datecash,concat(oh.f_prefix, oh.f_hallid) as f_prefix,"
//                "dp.f_name as f_partname,d.f_name as f_dishname,g.f_name as f_goodsname,"
//                "st.f_name as f_storename,og.f_qty as f_recipeqty,ob.f_qty1 as f_saleqty, "
//                "og.f_qty*ob.f_qty1 as f_output "
//                "from o_body ob "
//                "left join o_header oh on oh.f_id=ob.f_header "
//                "left join d_dish d on d.f_id=ob.f_dish "
//                "left join d_part2 dp on dp.f_id=d.f_part "
//                "left join d_recipes og on og.f_dish=ob.f_dish "
//                "left join c_goods g on g.f_id=og.f_goods "
//                "left join c_storages st on st.f_id=ob.f_store  "
//                "%where% "
//                "order by oh.f_datecash, dp.f_name";
    fSqlQuery = "select "
                "dp.f_name as f_partname,d.f_name as f_dishname,g.f_name as f_goodsname,"
                "st.f_name as f_storename,og.f_qty as f_recipeqty,sum(ob.f_qty1) as f_saleqty, "
                "sum(og.f_qty*ob.f_qty1) as f_output "
                "from o_body ob "
                "left join o_header oh on oh.f_id=ob.f_header "
                "left join d_dish d on d.f_id=ob.f_dish "
                "left join d_part2 dp on dp.f_id=d.f_part "
                "left join d_recipes og on og.f_dish=ob.f_dish "
                "left join c_goods g on g.f_id=og.f_goods "
                "left join c_storages st on st.f_id=ob.f_store  "
                "%where% "
                "group by 1,2,3,4,5 "
                "union "
                "select "
                "dp.f_name as f_partname,d.f_name as f_dishname,g.f_name as f_goodsname,"
                "st.f_name as f_storename,og.f_qty as f_recipeqty,sum(ob.f_qty1) as f_saleqty, "
                "sum(og.f_qty*ob.f_qty1*ds.f_qty) as f_output "
                "from o_body ob "
                "left join o_header oh on oh.f_id=ob.f_header "
                "left join d_dish d on d.f_id=ob.f_dish "
                "left join d_part2 dp on dp.f_id=d.f_part "
                "inner join d_dish_set ds on ds.f_id=ob.f_dish "
                "left join d_recipes og on og.f_dish=ds.f_part "
                "left join c_goods g on g.f_id=og.f_goods "
                "left join c_storages st on st.f_id=ob.f_store  "
                "%where% "
                "group by 1,2,3,4,5 "
                "order by 1,2 ";
    fSqlQuery.replace("%1", fFilter->condition());
    C5ReportWidget::buildQuery();
}

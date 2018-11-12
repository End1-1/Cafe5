#include "cr5goodsmovement.h"
#include "cr5goodsmovementfilter.h"

CR5GoodsMovement::CR5GoodsMovement(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Movements in the store");
    fSimpleQuery = false;

    fMainTable = "a_store s";
    fLeftJoinTables << "inner join a_type_sign st on st.f_id=s.f_type [st]"
                    << "inner join c_storages ss on ss.f_id=s.f_store [ss]"
                    << "inner join c_goods g on g.f_id=s.f_goods [g]"
                    << "inner join c_groups gg on gg.f_id=g.f_group [gg]"
                    << "inner join c_units u on u.f_id=g.f_unit [u]"
                    << "inner join a_header a on a.f_id=s.f_document [a]"
                       ;

    fColumnsFields << "a.f_date"
                   << "s.f_document"
                   << "ss.f_name as f_store"
                   << "st.f_name as f_type"
                   << "gg.f_name as f_group"
                   << "g.f_name as f_goods"
                   << "sum(s.f_qty) as f_qty"
                   << "u.f_name as f_unit"
                   << "s.f_price"
                   << "sum(s.f_total) as f_total"
                      ;

    fColumnsGroup << "a.f_date"
                  << "s.f_document"
                  << "st.f_name as f_type"
                  << "ss.f_name as f_store"
                  << "gg.f_name as f_group"
                  << "g.f_name as f_goods"
                  << "s.f_price"
                  << "u.f_name as f_unit"
                  << "g.f_name as f_goods"
                      ;

    fColumnsSum << "f_qty"
                << "f_total"
                      ;

    fTranslation["f_date"] = tr("Date");
    fTranslation["f_document"]  = tr("Document");
    fTranslation["f_type"] = tr("In/Out");
    fTranslation["f_store"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_total"] = tr("Amount");;

    fColumnsVisible["a.f_date"] = true;
    fColumnsVisible["s.f_document"] = true;
    fColumnsVisible["st.f_name as f_type"] = true;
    fColumnsVisible["ss.f_name as f_store"] = true;
    fColumnsVisible["gg.f_name as f_group"] = true;
    fColumnsVisible["g.f_name as f_goods"] = true;
    fColumnsVisible["sum(s.f_qty) as f_qty"] = true;
    fColumnsVisible["u.f_name as f_unit"] = true;
    fColumnsVisible["s.f_price"] = true;
    fColumnsVisible["sum(s.f_total) as f_total"] = true;

    restoreColumnsVisibility();

    fFilterWidget = new CR5GoodsMovementFilter(fDBParams);
}

QToolBar *CR5GoodsMovement::toolBar()
{
    if (!fToolBar) {
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

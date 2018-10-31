#include "cr5materialsinstore.h"
#include "cr5materialinstorefilter.h"

CR5MaterialsInStore::CR5MaterialsInStore(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Materials in the store");
    fSimpleQuery = false;

    fMainTable = "a_store s";
    fLeftJoinTables << "inner join a_header h on h.f_id=s.f_document [h]"
                    << "inner join c_storages ss on ss.f_id=s.f_store [ss]"
                    << "inner join c_units u on u.f_id=g.f_unit [u]"
                    << "inner join c_groups gg on gg.f_id=g.f_group [gg]"
                    << "inner join c_goods g on g.f_id=s.f_goods [g]"
                       ;

    fColumnsFields << "ss.f_name as f_storage"
                   << "gg.f_name as f_group"
                   << "g.f_name as f_goods"
                   << "sum(s.f_qty*s.f_type) as f_qty"
                   << "u.f_name as f_unit"
                   << "s.f_price"
                   << "sum(s.f_total*s.f_type) as f_total"
                      ;

    fColumnsGroup << "ss.f_name as f_storage"
                   << "gg.f_name as f_group"
                   << "s.f_price"
                   << "u.f_name as f_unit"
                   << "g.f_name as f_goods"
                      ;

    fColumnsSum << "f_qty"
                << "f_total"
                      ;

    fHavindCondition = " having sum(s.f_qty*s.f_type) > 0.001 ";

    fTranslation["f_storage"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_total"] = tr("Amount");;

    fColumnsVisible["ss.f_name as f_storage"] = true;
    fColumnsVisible["gg.f_name as f_group"] = true;
    fColumnsVisible["g.f_name as f_goods"] = true;
    fColumnsVisible["sum(s.f_qty*s.f_type) as f_qty"] = true;
    fColumnsVisible["u.f_name as f_unit"] = true;
    fColumnsVisible["s.f_price"] = true;
    fColumnsVisible["sum(s.f_total*s.f_type) as f_total"] = true;

    restoreColumnsVisibility();

    fFilterWidget = new CR5MaterialInStoreFilter(fDBParams);
}

QToolBar *CR5MaterialsInStore::toolBar()
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

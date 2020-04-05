#include "cr5goods.h"
#include "ce5goods.h"
#include "cr5goodsfilter.h"

CR5Goods::CR5Goods(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Goods");

    fSimpleQuery = false;

    fMainTable = "c_goods gg";
    fLeftJoinTables << "left join c_groups g on g.f_id=gg.f_group [g]"
                    << "left join c_partners cp on cp.f_id=gg.f_supplier [cp]"
                    << "left join c_goods_classes gca on gca.f_id=gg.f_group1 [gca]"
                    << "left join c_goods_classes gcb on gcb.f_id=gg.f_group2 [gcb]"
                    << "left join c_goods_classes gcc on gcc.f_id=gg.f_group3 [gcc]"
                    << "left join c_goods_classes gcd on gcd.f_id=gg.f_group4 [gcd]"
                    << "left join c_units u on u.f_id=gg.f_unit [u]"
                       ;

    fColumnsFields << "gg.f_id"
                   << "cp.f_taxname"
                   << "g.f_name as f_groupname"
                   << "u.f_name as f_unitname"
                   << "gg.f_name"
                   << "gg.f_scancode"
                   << "gg.f_saleprice"
                   << "gg.f_saleprice2"
                   << "gg.f_lowlevel"
                   << "gg.f_lastinputprice"
                   << "gca.f_name as gname1"
                   << "gcb.f_name as gname2"
                   << "gcc.f_name as gname3"
                   << "gcd.f_name as gname4"
                      ;

    fColumnsVisible["gg.f_id"] = true;
    fColumnsVisible["cp.f_taxname"] = true;
    fColumnsVisible["g.f_name as f_groupname"] = true;
    fColumnsVisible["u.f_name as f_unitname"] = true;
    fColumnsVisible["gg.f_name"] = true;
    fColumnsVisible["gg.f_saleprice"] = true;
    fColumnsVisible["gg.f_saleprice2"] = true;
    fColumnsVisible["gg.f_lowlevel"] = true;
    fColumnsVisible["gg.f_lastinputprice"] = true;
    fColumnsVisible["gca.f_name as gname1"] = true;
    fColumnsVisible["gcb.f_name as gname2"] = true;
    fColumnsVisible["gcc.f_name as gname3"] = true;
    fColumnsVisible["gcd.f_name as gname4"] = true;
    fColumnsVisible["gg.f_scancode"] = true;

    fTranslation["f_id"] = tr("Code");
    fTranslation["f_taxname"] = tr("Supplier");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_unitname"] = tr("Unit");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_saleprice"] = tr("Retail price");
    fTranslation["f_saleprice2"] = tr("Wholesale price");
    fTranslation["f_lowlevel"] = tr("Low level");
    fTranslation["f_lastinputprice"] = tr("Last input price");
    fTranslation["gname1"] = tr("Class 1");
    fTranslation["gname2"] = tr("Class 2");
    fTranslation["gname3"] = tr("Class 3");
    fTranslation["gname4"] = tr("Class 4");
    fTranslation["f_scancode"] = tr("Scancode");

    restoreColumnsVisibility();
    fEditor = new CE5Goods(dbParams);
    fFilterWidget = new CR5GoodsFilter(dbParams);
}

QToolBar *CR5Goods::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

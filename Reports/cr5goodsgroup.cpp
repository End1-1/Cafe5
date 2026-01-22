#include "cr5goodsgroup.h"
#include "ce5goodsgroup.h"

CR5GoodsGroup::CR5GoodsGroup(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/goods.png";
    fLabel = tr("Group of goods");
    fSqlQuery = R"(
    select gr.f_id, grp.f_name as f_parent, gr.f_name, gr.f_class, gr.f_taxdept, gr.f_adgcode, gr.f_chargevalue,
    g.qnt
    from c_groups gr
    left join c_groups grp on grp.f_id=gr.f_parent
    left join (select g.f_group, count(g.f_id) as qnt from c_goods g group by 1) g on g.f_group = gr.f_id
    )";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_parent"] = tr("Parent");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_class"] = tr("Class");
    fTranslation["f_taxdept"] = tr("Tax dept");
    fTranslation["f_adgcode"] = tr("ADG code");
    fTranslation["f_chargevalue"] = tr("Charge value");
    fTranslation["f_color"] = tr("Color");
    fTranslation["qnt"] = tr("Qty");
    fEditor = new CE5GoodsGroup();
}

QToolBar* CR5GoodsGroup::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }

    return fToolBar;
}

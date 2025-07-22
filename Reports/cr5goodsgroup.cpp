#include "cr5goodsgroup.h"
#include "ce5goodsgroup.h"

CR5GoodsGroup::CR5GoodsGroup(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Group of goods");
    fSqlQuery = "select f_id, f_name, f_class, f_taxdept, f_adgcode, f_chargevalue from c_groups ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_class"] = tr("Class");
    fTranslation["f_taxdept"] = tr("Tax dept");
    fTranslation["f_adgcode"] = tr("ADG code");
    fTranslation["f_chargevalue"] = tr("Charge value");
    fEditor = new CE5GoodsGroup();
}

QToolBar *CR5GoodsGroup::toolBar()
{
    if (!fToolBar) {
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

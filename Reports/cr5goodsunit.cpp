#include "cr5goodsunit.h"
#include "ce5goodsunit.h"

CR5GoodsUnit::CR5GoodsUnit(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIconName = ":/goods.png";
    fLabel = tr("Units");

    fSqlQuery = "select f_id, f_name, f_fullname, f_defaultqty from c_units ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_fullname"] = tr("Full caption");
    fTranslation["f_defaultqty"] = tr("Default qty");

    fEditor = new CE5GoodsUnit();
}

QToolBar *CR5GoodsUnit::toolBar()
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

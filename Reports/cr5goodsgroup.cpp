#include "cr5goodsgroup.h"
#include "ce5goodsgroup.h"

CR5GoodsGroup::CR5GoodsGroup(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Group of goods");

    fSqlQuery = "select f_id, f_name from c_groups ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");

    fEditor = new CE5GoodsGroup(dbParams);
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

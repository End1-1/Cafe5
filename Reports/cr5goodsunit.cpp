#include "cr5goodsunit.h"

CR5GoodsUnit::CR5GoodsUnit(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Units");

    fSqlQuery = "select f_id, f_name from c_units ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");

    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));

    QList<int> colsForUpdate;
    colsForUpdate << 1;
    setTableForUpdate("c_units", colsForUpdate);
}

QToolBar *CR5GoodsUnit::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbSave
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

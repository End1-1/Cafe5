#include "cr5goodsstorages.h"

CR5GoodsStorages::CR5GoodsStorages(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Storages");

    fSqlQuery = "select f_id, f_name from c_storages ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");

    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));

    QList<int> colsForUpdate;
    colsForUpdate << 1;
    setTableForUpdate("c_storages", colsForUpdate);
}

QToolBar *CR5GoodsStorages::toolBar()
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

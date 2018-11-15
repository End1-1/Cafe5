#include "cr5menunames.h"

CR5MenuNames::CR5MenuNames(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Menu names");
    fSqlQuery = "select f_id, f_name, f_date, f_comment from d_menu_names";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_comment"] = tr("Comment");

    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(2, new C5DateDelegate(fTableView));
    fTableView->setItemDelegateForColumn(3, new C5TextDelegate(fTableView));

    QList<int> colsForUpdate;
    colsForUpdate << 1 << 2 << 3;
    setTableForUpdate("d_menu_names", colsForUpdate);
}

QToolBar *CR5MenuNames::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }
    return fToolBar;
}

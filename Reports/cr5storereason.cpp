#include "cr5storereason.h"
#include "ce5storereason.h"

CR5StoreReason::CR5StoreReason(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIconName = ":/graph.png";
    fLabel = tr("Store move reason");
    fSqlQuery = "select f_id, f_name from a_reason";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fEditor = new CE5StoreReason();
}

QToolBar *CR5StoreReason::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

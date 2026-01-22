#include "cr5mfproduct.h"
#include "ce5mfproduct.h"

CR5MFProduct::CR5MFProduct(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIconName = ":/manufacturing.png";
    fLabel = tr("Products");
    fSqlQuery = "select f_id, f_name from mf_actions_group";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fEditor = new CE5MFProduct();
}

QToolBar *CR5MFProduct::toolBar()
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

#include "cr5mfworkshops.h"
#include "ce5mfworkshop.h"

CR5MFWorkshops::CR5MFWorkshops(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/manufacturing.png";
    fLabel = tr("Workshops");
    fSqlQuery = "select f_id, f_name from mf_stage";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fEditor = new CE5MFWorkshop();
}

QToolBar *CR5MFWorkshops::toolBar()
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

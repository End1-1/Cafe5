#include "cr5mfactions.h"
#include "ce5mfprocess.h"

CR5MfActions::CR5MfActions(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/manufacturing.png";
    fLabel = tr("Manufacture actions");
    fSqlQuery = "select f_id, f_name from mf_actions";
    fEditor = new CE5MFProcess(dbParams);
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
}

QToolBar *CR5MfActions::toolBar()
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

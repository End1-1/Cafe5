#include "cr5mfactionstage.h"
#include "ce5mfprocessstage.h"

CR5MFActionStage::CR5MFActionStage(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/manufacturing.png";
    fLabel = tr("Stages of actions");
    fSqlQuery = "select f_id, f_name from mf_actions_state";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fEditor = new CE5MFProcessStage();
}

QToolBar *CR5MFActionStage::toolBar()
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

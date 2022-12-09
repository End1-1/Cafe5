#include "cr5mfactions.h"
#include "ce5mfprocess.h"

CR5MfActions::CR5MfActions(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/manufacturing.png";
    fLabel = tr("Manufacture actions");
    fSqlQuery = "select a.f_id, a.f_name, a.f_state, s.f_name as f_statename "
                "from mf_actions a "
                "left join mf_actions_state s on s.f_id=a.f_state "
                "order by s.f_name ";
    fEditor = new CE5MFProcess(dbParams);
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_state"] = tr("Stage code");
    fTranslation["f_statename"] = tr("Stage");
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

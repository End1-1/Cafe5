#include "cr5mfactivetasks.h"
#include "c5user.h"
#include "dlgconfigtable.h"
#include "ce5mftask.h"

CR5MFActiveTasks::CR5MFActiveTasks(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/manufacturing.png";
    fLabel = tr("Tasks");
    fSimpleQuery = true;
    fSqlQuery = "select t.f_id, t.f_datecreate , tm.f_name as f_statename, "
            "g.f_name as f_productname, w.f_name as f_workshopname, s.f_name as f_stagename, "
            "t.f_qty, t.f_ready, t.f_out "
            "from mf_tasks t "
            "left join mf_tasks_state tm on tm.f_id=t.f_state "
            "left join mf_stage w on w.f_id=t.f_workshop "
            "left join mf_actions_state s on s.f_id=t.f_stage "
            "left join mf_actions_group g on g.f_id=t.f_product ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_datecreate"] = tr("Date");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_productname"] = tr("Product");
    fTranslation["f_ready"] = tr("Ready");
    fTranslation["f_workshopname"] = tr("Workshop");
    fTranslation["f_stagename"] = tr("Stage");
    fTranslation["f_out"] = tr("Out");
    fEditor = new CE5MFTask(dbParams);
}

QToolBar *CR5MFActiveTasks::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        if (__user->check(cp_t10_create_task)) {
            btn << ToolBarButtons::tbNew;
        }
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
        auto *cp = new QAction(QIcon(":/qr_code.png"), tr("Configure\ntablet"));
        connect(cp, &QAction::triggered, this, &CR5MFActiveTasks::configTablet);
        fToolBar->addAction(cp);
    }
    return fToolBar;
}

void CR5MFActiveTasks::configTablet()
{
    DlgConfigTable(fDBParams).exec();
}

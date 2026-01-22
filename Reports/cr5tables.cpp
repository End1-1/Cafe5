#include "cr5tables.h"
#include "ce5table.h"
#include "ce5createtablesforhall.h"

CR5Tables::CR5Tables(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/table.png";
    fLabel = tr("Tables");
    fSimpleQuery = false;
    fMainTable = "h_tables t";
    fLeftJoinTables << "left join h_halls h on h.f_id=t.f_hall [h]";
    fColumnsFields << "t.f_id"
                   << "h.f_name as f_hallname"
                   << "t.f_name";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_name"] = tr("Name");
    fColumnsVisible["t.f_id"] = true;
    fColumnsVisible["h.f_name as f_hallname"] = true;
    fColumnsVisible["t.f_name"] = true;
    restoreColumnsVisibility();
    fEditor = new CE5Table();
}

QToolBar* CR5Tables::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/constructor.png"), tr("Create hall"), this, SLOT(createHall()));
    }

    return fToolBar;
}

void CR5Tables::createHall()
{
    CE5CreateTablesForHall::createTableForHall(mUser);
}

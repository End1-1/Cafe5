#include "cr5tables.h"
#include "ce5table.h"
#include "ce5createtablesforhall.h"
#include "c5cache.h"
#include "c5message.h"
#include "c5officewidget.h"
#include "c5tablemodel.h"
#include "c5user.h"
#include "ninterface.h"

CR5Tables::CR5Tables(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/table.png";
    fLabel = tr("Tables");
    fSimpleQuery = false;
    fMainTable = "h_tables t";
    fLeftJoinTables << "left join h_halls h on h.f_id=t.f_hall [h]";
    fLeftJoinTables << "left join s_settings_names sn on sn.f_id=t.f_special_config [sn]";
    fColumnsFields << "t.f_id"
                   << "h.f_name as f_hallname"
                   << "t.f_name"
                   << "sn.f_name as f_config_name";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_name"] = tr("Name");
    fColumnsVisible["t.f_id"] = true;
    fColumnsVisible["h.f_name as f_hallname"] = true;
    fColumnsVisible["t.f_name"] = true;
    fColumnsVisible["sn.f_name as f_config_name"] = true;
    restoreColumnsVisibility();
    fEditor = new CE5Table();
}

QToolBar* CR5Tables::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbDelete
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/constructor.png"), tr("Create hall"), this, SLOT(createHall()));
    }

    return fToolBar;
}

void CR5Tables::removeWithId(int id, int row)
{
    if(id <= 0 || !fModel || row < 0 || row >= fModel->rowCount()) {
        return;
    }

    if(id == 1) {
        C5Message::error(tr("Cannot delete default table"));
        return;
    }

    if(C5Message::question(tr("Delete selected table?")) != QDialog::Accepted) {
        return;
    }

    QJsonObject params{
        {QStringLiteral("editor"), QStringLiteral("form_tables")},
        {QStringLiteral("f_id"), id},
    };

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/remove"),
                      C5OfficeWidget::mUser->mSessionKey,
                      this,
                      params,
                      [this, row](const QJsonObject &) {
                          fModel->removeRow(row);
                          C5Cache::cache(cache_tables)->refresh();
                          C5Message::info(tr("Deleted"));
                      },
                      [](const QJsonObject &) { return false; },
                      true,
                      5000,
                      false);
}

void CR5Tables::createHall()
{
    CE5CreateTablesForHall::createTableForHall(mUser);
}

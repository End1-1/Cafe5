#include "cr5settings.h"
#include "c5settingswidget.h"
#include "c5tablemodel.h"

CR5Settings::CR5Settings(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/configure.png";
    fLabel = tr("Settings");
    fSqlQuery = "select f_id, f_name, f_description from s_settings_names ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_description"] = tr("Description");
    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(2, new C5TextDelegate(fTableView));
    QList<int> colsForUpdate;
    colsForUpdate << 1 << 2;
    setTableForUpdate("s_settings_names", colsForUpdate);
    fSettingsWidget = new C5SettingsWidget(dbParams, this);
    hl()->addWidget(fSettingsWidget);
}

QToolBar *CR5Settings::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbSave
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/delete.png"), tr("Delete\nsettings"), this, SLOT(removeSettings()));
    }
    return fToolBar;
}

void CR5Settings::saveDataChanges()
{
    C5Grid::saveDataChanges();
    fSettingsWidget->save();
}

void CR5Settings::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    if (int id = rowId()) {
        fSettingsWidget->setSettingsId(id);
    } else {
        fSettingsWidget->clear(fSettingsWidget);
    }
}

void CR5Settings::removeSettings()
{
    int row;
    if (int id = rowId(row, 0)) {
        if (C5Message::question(tr("Confirm removal")) != QDialog::Accepted) {
            return;
        }
        C5Database db(fDBParams);
        db[":f_settigns"] = id;
        db.exec("delete from s_settings_values where f_settings=:f_settings");
        db[":f_id"] = id;
        db.exec("delete from s_settings_names where f_id=:f_id");
        fModel->removeRow(row);
        fSettingsWidget->clear(0);
    }
}

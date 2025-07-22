#include "cr5settings.h"
#include "c5settingswidget.h"
#include "c5tablemodel.h"
#include "c5database.h"

CR5Settings::CR5Settings(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/configure.png";
    fLabel = tr("Settings");
    fSqlQuery = "select f_id, f_name, f_description from s_settings_names ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_description"] = tr("Description");
    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(2, new C5TextDelegate(fTableView));
    fEditor = new C5SettingsWidget();
    fSettingsWidget = nullptr;
}

QToolBar *CR5Settings::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/delete.png"), tr("Delete\nsettings"), this, SLOT(removeSettings()));
    }
    return fToolBar;
}

void CR5Settings::removeSettings()
{
    int row;
    if (int id = rowId(row, 0)) {
        if (C5Message::question(tr("Confirm removal")) != QDialog::Accepted) {
            return;
        }
        C5Database db;
        db[":f_settings"] = id;
        db.exec("delete from s_settings_values where f_settings=:f_settings");
        db[":f_id"] = id;
        db.exec("delete from s_settings_names where f_id=:f_id");
        fModel->removeRow(row);
        if (fSettingsWidget) {
            fSettingsWidget->clear(0);
        }
    }
}

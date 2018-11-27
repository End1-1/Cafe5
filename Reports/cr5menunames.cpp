#include "cr5menunames.h"
#include "ce5menuname.h"

CR5MenuNames::CR5MenuNames(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Menu names");
    fSqlQuery = "select f_id, f_name, f_datestart, f_dateend, f_enabled, f_comment from d_menu_names";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_datestart"] = tr("Start date");
    fTranslation["f_dateend"] = tr("End date");
    fTranslation["f_enabled"] = tr("Enabled");
    fTranslation["f_comment"] = tr("Comment");

    fEditor = new CE5MenuName(dbParams);
}

QToolBar *CR5MenuNames::toolBar()
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

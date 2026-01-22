#include "cr5hall.h"
#include "ce5halls.h"

CR5Hall::CR5Hall(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIconName = ":/hall.png";
    fLabel = tr("Halls");
    fSimpleQuery = false;

    fMainTable = "h_halls h";

    fColumnsFields << "h.f_id"
                   << "h.f_name"
                   << "h.f_counter"
                   << "h.f_prefix"
                   << "s.f_name as f_settingsname"
                   << "h.f_counterhall";

    fLeftJoinTables << "left join s_settings_names s on s.f_id=h.f_settings [s]";

    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_counter"] = tr("Counter");
    fTranslation["f_prefix"] = tr("Order prefix");
    fTranslation["f_settingsname"] = tr("Default settings");
    fTranslation["f_counterhall"] = tr("Hall counter");

    fColumnsVisible["h.f_id"] = true;
    fColumnsVisible["h.f_name"] = true;
    fColumnsVisible["h.f_counter"] = true;
    fColumnsVisible["h.f_prefix"] = true;
    fColumnsVisible["s.f_name as f_settingsname"] = true;
    fColumnsVisible["h.f_counterhall"] = true;

    restoreColumnsVisibility();

    fEditor = new CE5Halls();
}

QToolBar *CR5Hall::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

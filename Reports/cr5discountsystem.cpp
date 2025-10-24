#include "cr5discountsystem.h"
#include "ce5discountcard.h"

CR5DiscountSystem::CR5DiscountSystem(QWidget *parent) :
    C5ReportWidget(parent)
{
    fLabel = tr("Dicount system");
    fIcon = ":/discount.png";
    fSimpleQuery = false;
    fMainTable = "b_cards_discount d";
    fLeftJoinTables << "left join c_partners c on c.f_id=d.f_client [c]";
    fLeftJoinTables << "left join b_card_types dt on dt.f_id=d.f_mode [dt]";
    fColumnsFields << "d.f_id"
                   << "d.f_number"
                   << "dt.f_name as dtname"
                   << "concat_ws(', ', c.f_contact, c.f_taxname, c.f_name, c.f_phone) as f_contact"
                   << "d.f_value"
                   << "c.f_info"
                   << "d.f_code"
                   << "d.f_datestart"
                   << "d.f_dateend"
                   << "d.f_active";
    fTranslation["f_id"] = tr("Code");
    fTranslation["dtname"] = tr("Mode");
    fTranslation["f_number"] = tr("Card number");
    fTranslation["f_contact"] = tr("Contact name");
    fTranslation["f_value"] = tr("Discount");
    fTranslation["f_info"] = tr("Client info");
    fTranslation["f_code"] = tr("Card code");
    fTranslation["f_datestart"] = tr("Start date");
    fTranslation["f_dateend"] = tr("End date");
    fTranslation["f_active"] = tr("State");
    fColumnsVisible["d.f_id"] = true;
    fColumnsVisible[ "d.f_number"] = true;
    fColumnsVisible["dt.f_name as dtname"] = true;
    fColumnsVisible["concat_ws(', ', c.f_contact, c.f_taxname, c.f_name, c.f_phone) as f_contact"] = true;
    fColumnsVisible["d.f_value"] = true;
    fColumnsVisible["c.f_info"] = true;
    fColumnsVisible["d.f_code"] = true;
    fColumnsVisible["d.f_datestart"] = true;
    fColumnsVisible["d.f_dateend"] = true;
    fColumnsVisible["d.f_active"] = true;
    restoreColumnsVisibility();
    fEditor = new CE5DiscountCard();
}

QToolBar* CR5DiscountSystem::toolBar()
{
    if(!fToolBar) {
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

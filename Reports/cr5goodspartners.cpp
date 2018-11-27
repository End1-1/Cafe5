#include "cr5goodspartners.h"
#include "c5cache.h"
#include "c5editor.h"
#include "ce5partner.h"

CR5GoodsPartners::CR5GoodsPartners(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Partners");
    fSqlQuery = "select * from c_partners ";

    fTranslation["f_id"] = tr("Code");
    fTranslation["f_taxname"] = tr("Name");
    fTranslation["f_taxcode"] = tr("Taxpayer id");
    fTranslation["f_contact"] = tr("Contact");
    fTranslation["f_info"] = tr("Info");
    fTranslation["f_phone"] = tr("Phone");
    fTranslation["f_email"] = tr("Email");

    fEditor = new CE5Partner(fDBParams);
}

QToolBar *CR5GoodsPartners::toolBar()
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

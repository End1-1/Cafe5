#include "cr5customers.h"
#include "ce5client.h"

CR5Customers::CR5Customers(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/customers.png";
    fLabel = tr("Customers");
    fSimpleQuery = true;
    fSqlQuery = "select f_Id, f_firstname, f_lastname, f_info from b_clients";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_firstname"] = tr("First name");
    fTranslation["f_lastname"] = tr("Last name");
    fTranslation["f_info"] = tr("Info");
    fEditor = new CE5Client(dbParams);
}

QToolBar *CR5Customers::toolBar()
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

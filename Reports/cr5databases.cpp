#include "cr5databases.h"
#include "ce5databases.h"

CR5Databases::CR5Databases(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/database.png";
    fLabel = tr("Databases");
    fSqlQuery = "select f_id, f_host, f_db, f_name, f_description, f_user, f_password from s_db ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_host"] = tr("Host");
    fTranslation["f_db"] = tr("Database");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_description"] = tr("Description");
    fTranslation["f_user"] = tr("User");
    fTranslation["f_password"] = tr("Password");

    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(2, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(3, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(4, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(5, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(6, new C5TextDelegate(fTableView));

    fEditor = new CE5Databases(dbParams);
}

QToolBar *CR5Databases::toolBar()
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

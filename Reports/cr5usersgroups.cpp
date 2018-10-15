#include "cr5usersgroups.h"

CR5UsersGroups::CR5UsersGroups(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/users_groups.png";
    fLabel = tr("Users groups");
    fSqlQuery = "select f_id, f_name from s_user_group";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));
    QList<int> colsForUpdate;
    colsForUpdate << 1;
    setTableForUpdate("s_user_group", colsForUpdate);
}

QToolBar *CR5UsersGroups::toolBar()
{
    QList<ToolBarButtons> btn;
    btn << ToolBarButtons::tbNew
        << ToolBarButtons::tbSave
        << ToolBarButtons::tbClearFilter
        << ToolBarButtons::tbRefresh
        << ToolBarButtons::tbExcel
        << ToolBarButtons::tbPrint;
    return createStandartToolbar(btn);
}

void CR5UsersGroups::newGroup()
{
    C5Message::info("New group");
}

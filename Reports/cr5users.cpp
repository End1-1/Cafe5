#include "cr5users.h"
#include "c5cacheusersgroups.h"
#include "c5cacheusersstate.h"

CR5Users::CR5Users(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/users_groups.png";
    fLabel = tr("Users");
    fSqlQuery = "select u.f_id, g.f_name as f_groupname, s.f_name as f_statename, u.f_login, u.f_first, u.f_last \
                from s_user u \
                left join s_user_group g on g.f_id=u.f_group \
                left join s_user_state s on s.f_id=u.f_state ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_login"] = tr("Username");
    fTranslation["f_first"] = tr("First name");
    fTranslation["f_last"] = tr("Last name");

    C5CacheUsersGroups *cg = static_cast<C5CacheUsersGroups*>(createCache(cache_users_groups));
    C5ComboDelegate *cbGroups = new C5ComboDelegate("f_group", cg, fTableView);
    fTableView->setItemDelegateForColumn(1, cbGroups);

    C5CacheUsersState *cs = static_cast<C5CacheUsersState*>(createCache(cache_users_states));
    C5ComboDelegate *cbStates = new C5ComboDelegate("f_state", cs, fTableView);
    fTableView->setItemDelegateForColumn(2, cbStates);

    fTableView->setItemDelegateForColumn(3, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(4, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(5, new C5TextDelegate(fTableView));

    QList<int> colsForUpdate;
    colsForUpdate << 3 << 4 << 5;
    setTableForUpdate("s_user", colsForUpdate);
}

QToolBar *CR5Users::toolBar()
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

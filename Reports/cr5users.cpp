#include "cr5users.h"
#include "c5passwords.h"
#include "ce5user.h"
#include "cr5usersfilter.h"

CR5Users::CR5Users(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/users_groups.png";
    fLabel = tr("Users");
    fSqlQuery = R"(
        select u.f_id, g.f_name as f_groupname, concat(t.f_last, ' ', t.f_first) as f_teamleadname,
        s.f_name as f_statename, u.f_login, u.f_first, u.f_last, u.f_phone, ss.f_name as f_configname
        from s_user u
        left join s_user_group g on g.f_id=u.f_group
        left join s_user t on t.f_id=u.f_teamlead
        left join s_user_state s on s.f_id=u.f_state
        left join s_settings_names ss on ss.f_id=u.f_config
    )";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_teamleadname"] = tr("Teamlead");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_login"] = tr("Username");
    fTranslation["f_first"] = tr("First name");
    fTranslation["f_last"] = tr("Last name");
    fTranslation["f_configname"] = tr("Config");
    fTranslation["f_phone"] = tr("Phone");
    fEditor = new CE5User();
    fFilterWidget = new CR5UsersFilter();
}

QToolBar* CR5Users::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/password.png"), tr("Set\npasswords"), this, SLOT(setPasswords()));
    }

    return fToolBar;
}

void CR5Users::setPasswords()
{
    if(int id = rowId(0)) {
        C5Passwords::setPasswords(id, mUser);
    };
}

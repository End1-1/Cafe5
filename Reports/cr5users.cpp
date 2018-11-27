#include "cr5users.h"
#include "c5passwords.h"
#include "ce5user.h"

CR5Users::CR5Users(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
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
    fEditor = new CE5User(dbParams);
}

QToolBar *CR5Users::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
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
    if (int id = rowId(0)) {
        C5Passwords::setPasswords(fDBParams, id);
    };
}

#include "cr5usersgroups.h"

CR5UsersGroups::CR5UsersGroups(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/users_groups.png";
    fLabel = tr("Users groups");
    fSqlQuery = "select f_id, f_name from s_user_group";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
}

QToolBar *CR5UsersGroups::toolBar()
{
    if (createToolBar()) {
        fToolBar->addAction(QIcon(":/new.png"), tr("New group"), this, SLOT(newGroup()));
    }
    return fToolBar;
}

void CR5UsersGroups::newGroup()
{
    C5Message::info("New group");
}

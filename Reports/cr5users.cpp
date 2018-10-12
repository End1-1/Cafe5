#include "cr5users.h"

CR5Users::CR5Users(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/users_groups.png";
    fLabel = tr("Users");
    fSqlQuery = "select f_id, f_login, f_first, f_last from s_user";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_login"] = tr("Username");
    fTranslation["f_first"] = tr("First name");
    fTranslation["f_last"] = tr("Last name");
}

QToolBar *CR5Users::toolBar()
{
    if (createToolBar()) {
        fToolBar->addAction(QIcon(":/new.png"), tr("New user"), this, SLOT(newUser()));
        fToolBar->addAction(QIcon(":/edit.png"), tr("Edit user"), this, SLOT(editUser()));
    }
    return fToolBar;
}

void CR5Users::newUser()
{
    C5Message::info("New user");
}

void CR5Users::editUser()
{
    C5Message::info("Edit user");
}

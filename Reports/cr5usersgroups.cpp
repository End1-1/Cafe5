#include "cr5usersgroups.h"
#include "c5grouppermissionseditor.h"
#include "ce5usergroup.h"
#include "c5mainwindow.h"

CR5UsersGroups::CR5UsersGroups(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/users_groups.png";
    fLabel = tr("Users groups");
    fSqlQuery = "select f_id, f_name, f_starttime, f_duration from s_user_group";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_starttime"] = tr("Start time");
    fTranslation["f_duration"] = tr("Duration");
    fEditor = new CE5UserGroup();
}

QToolBar* CR5UsersGroups::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/permissions.png"), tr("Set group\npermissions"), this, SLOT(setPermissions()));
    }

    return fToolBar;
}

void CR5UsersGroups::setPermissions()
{
    if(int id = rowId(0)) {
        C5GroupPermissionsEditor *p = __mainWindow->createTab<C5GroupPermissionsEditor>();
        p->setPermissionsGroupId(id);
    }
}

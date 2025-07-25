#ifndef CR5USERSGROUPS_H
#define CR5USERSGROUPS_H

#include "c5reportwidget.h"

class CR5UsersGroups : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5UsersGroups(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

private slots:
    void setPermissions();
};

#endif // CR5USERSGROUPS_H

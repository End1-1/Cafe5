#ifndef CR5USERS_H
#define CR5USERS_H

#include "c5reportwidget.h"

class CR5Users : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Users(QWidget *parent = 0);

    virtual QToolBar *toolBar();

private slots:
    void newUser();

    void editUser();
};

#endif // CR5USERS_H

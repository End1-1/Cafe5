#ifndef CR5USERS_H
#define CR5USERS_H

#include "c5reportwidget.h"

class CR5Users : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Users(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

private slots:
    void setPasswords();
};

#endif // CR5USERS_H

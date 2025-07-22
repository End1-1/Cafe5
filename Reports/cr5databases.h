#ifndef CR5DATABASES_H
#define CR5DATABASES_H

#include "c5reportwidget.h"

class CR5Databases : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Databases(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

private slots:
    void actionAccess();

    void backupDatabase();

    void resetDatabase();

    void done(QJsonObject);
};

#endif // CR5DATABASES_H

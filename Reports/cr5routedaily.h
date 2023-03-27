#ifndef CR5ROUTEDAILY_H
#define CR5ROUTEDAILY_H

#include "c5reportwidget.h"
#include <QObject>

class CR5RouteDaily : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5RouteDaily(const QStringList &dbParams, QWidget *parent = nullptr);
    virtual QToolBar *toolBar() override;
};

#endif // CR5ROUTEDAILY_H

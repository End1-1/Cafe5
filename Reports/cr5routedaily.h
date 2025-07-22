#ifndef CR5ROUTEDAILY_H
#define CR5ROUTEDAILY_H

#include "c5reportwidget.h"
#include <QObject>

class CR5RouteDaily : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5RouteDaily(QWidget *parent = nullptr);
    virtual QToolBar *toolBar() override;

protected:
    virtual void completeRefresh() override;
protected slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index) override;
};

#endif // CR5ROUTEDAILY_H

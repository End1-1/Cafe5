#ifndef CR5MENUREVIEW_H
#define CR5MENUREVIEW_H

#include "c5reportwidget.h"

class CR5MenuReview : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MenuReview(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

protected:
    virtual void refreshData() override;

private slots:
    void dblClick(int row, int column, const QList<QVariant> &v);
};

#endif // CR5MENUREVIEW_H

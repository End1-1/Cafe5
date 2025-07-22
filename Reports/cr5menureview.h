#ifndef CR5MENUREVIEW_H
#define CR5MENUREVIEW_H

#include "c5reportwidget.h"

class CR5MenuReview : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MenuReview(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void restoreColumnsWidths();

protected:
    virtual void refreshData() override;

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &v) override;

private slots:
    void groupAction();
};

#endif // CR5MENUREVIEW_H

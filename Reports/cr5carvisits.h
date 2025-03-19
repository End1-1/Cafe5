#ifndef CR5CARVISITS_H
#define CR5CARVISITS_H

#include "c5reportwidget.h"

class CR5CarVisits : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5CarVisits(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

protected:
    virtual void restoreColumnsWidths() override;

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;
};

#endif // CR5CARVISITS_H

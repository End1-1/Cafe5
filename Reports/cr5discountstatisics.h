#ifndef CR5DISCOUNTSTATISICS_H
#define CR5DISCOUNTSTATISICS_H

#include "c5reportwidget.h"

class CR5DiscountStatisics : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DiscountStatisics(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void restoreColumnsWidths() override;

protected:
    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;
};

#endif // CR5DISCOUNTSTATISICS_H

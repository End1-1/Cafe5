#ifndef CR5COMMONSALES_H
#define CR5COMMONSALES_H

#include "c5reportwidget.h"

class CR5CommonSalesFilter;

class CR5CommonSales : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5CommonSales(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

protected:
    virtual void editRow(int columnWidthId) override;

    virtual void restoreColumnsWidths() override;

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;

private slots:
    void transferToRoom();

    void templates();

private:
    CR5CommonSalesFilter *fFilter;
};

#endif // CR5COMMONSALES_H

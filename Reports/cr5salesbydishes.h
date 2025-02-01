#ifndef CR5SALESBYDISHES_H
#define CR5SALESBYDISHES_H

#include "c5reportwidget.h"

class CR5SalesByDishes : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5SalesByDishes(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void restoreColumnsWidths();

protected:
    virtual bool tblDoubleClicked(int row, int column, const QVector<QJsonValue> &v);

private slots:
    void actionShowDish();
};

#endif // CR5SALESBYDISHES_H

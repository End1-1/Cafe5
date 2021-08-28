#ifndef CR5COMMONSALES_H
#define CR5COMMONSALES_H

#include "c5reportwidget.h"

class CR5CommonSales : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5CommonSales(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected:
    virtual void editRow(int columnWidthId);

    virtual void restoreColumnsWidths();

    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &values);

private slots:
    void transferToRoom();

    void exportToAS();
};

#endif // CR5COMMONSALES_H

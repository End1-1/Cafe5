#ifndef CR5STOREDOCUMENTS_H
#define CR5STOREDOCUMENTS_H

#include "c5reportwidget.h"

class CR5StoreDocuments : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5StoreDocuments(const QStringList &dbParams, QWidget *parent = 0);

    virtual void buildQuery();

    QToolBar *toolBar();

private slots:
    void tblDoubleClickEvent(int row, int column, const QList<QVariant> &values);
};

#endif // CR5STOREDOCUMENTS_H

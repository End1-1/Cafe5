#ifndef CR5STOREDOCUMENTS_H
#define CR5STOREDOCUMENTS_H

#include "c5reportwidget.h"

class CR5StoreDocumentsFilter;

class CR5StoreDocuments : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5StoreDocuments(QWidget *parent = nullptr);

    virtual void buildQuery() override;

    QToolBar *toolBar() override;

    CR5StoreDocumentsFilter *fFilter;

protected:
    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;

};

#endif // CR5STOREDOCUMENTS_H

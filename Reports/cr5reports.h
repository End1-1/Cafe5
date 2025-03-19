#ifndef CR5REPORTS_H
#define CR5REPORTS_H

#include "c5reportwidget.h"

class CR5ReportsFilter;

class CR5Reports : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5Reports(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    void setReport(int id);

    virtual void buildQuery() override;

public slots:
    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;

    virtual void setSearchParameters() override;

protected:
    virtual void completeRefresh() override;

private:
    QString fHandlerUuid;
    QString fDeleteHandler;
    QString fFilterHandler;
    QString fQuery;
    CR5ReportsFilter *fFilter;

private slots:
    void removeHandler(bool checked = false);
};

#endif // CR5REPORTS_H

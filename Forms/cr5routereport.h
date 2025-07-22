#ifndef CR5ROUTEREPORT_H
#define CR5ROUTEREPORT_H

#include "c5document.h"

namespace Ui {
class CR5RouteReport;
}

class CR5RouteReport : public C5Document
{
    Q_OBJECT

public:
    explicit CR5RouteReport(QWidget *parent = nullptr);
    ~CR5RouteReport();
    virtual QToolBar *toolBar() override;

private:
    Ui::CR5RouteReport *ui;

private slots:
    void newRow();
};

#endif // CR5ROUTEREPORT_H

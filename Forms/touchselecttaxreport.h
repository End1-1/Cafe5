#ifndef TOUCHSELECTTAXREPORT_H
#define TOUCHSELECTTAXREPORT_H

#include "c5dialog.h"

namespace Ui {
class TouchSelectTaxReport;
}

class TouchSelectTaxReport : public C5Dialog
{
    Q_OBJECT

public:
    explicit TouchSelectTaxReport();
    ~TouchSelectTaxReport();
    static bool getReportType(int &r);

private slots:
    void on_btnReject_clicked();

    void on_btnPrintTaxX_clicked();

    void on_btnPrintTaxZ_clicked();

private:
    Ui::TouchSelectTaxReport *ui;
};

#endif // TOUCHSELECTTAXREPORT_H

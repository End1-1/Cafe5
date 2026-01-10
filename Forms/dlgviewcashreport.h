#ifndef DLGVIEWCASHREPORT_H
#define DLGVIEWCASHREPORT_H

#include "c5dialog.h"

namespace Ui
{
class DlgViewCashReport;
}

class DlgViewCashReport : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgViewCashReport(C5User *user);
    ~DlgViewCashReport();
    void addTotal(double t);
    void addTitle(const QString &t);
    void addRow(const QString &t, double v);

private slots:
    void on_btnClose_clicked();

    void on_btnPrint_clicked();

private:
    Ui::DlgViewCashReport* ui;
};

#endif // DLGVIEWCASHREPORT_H

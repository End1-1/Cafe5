#ifndef DLGREPORTS_H
#define DLGREPORTS_H

#include "c5waiterdialog.h"

namespace Ui
{
class DlgReports;
}

class C5User;

class DlgReports : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgReports(C5User *user);

    ~DlgReports();

    static void openReports(C5User *user);

private:
    Ui::DlgReports* ui;

    int fCurrentHall;

    QDate mDate1;

    QDate mDate2;

    void getDailyCommon();

    void setLangIcon();

private slots:

    void handleDailyCommon(const QJsonObject &obj);

    void printReportResponse(const QJsonObject &obj);

    void handleReceipt(const QJsonObject &obj);

    void handleTaxback(const QJsonObject &obj);

    void handleTaxReport(const QJsonObject &obj);

    void on_btnRefresh_clicked();

    void on_btnExit_clicked();

    void on_btnReports_clicked();

    void on_btnPrintOrderReceipt_clicked();

    void on_btnReceiptLanguage_clicked();

    void on_btnHall_clicked();

    void on_btnReturnTaxReceipt_clicked();

    void on_btnPrintTaxX_clicked();

    void on_btnPrintTaxZ_clicked();
    void on_btnParams_clicked();
};

#endif // DLGREPORTS_H

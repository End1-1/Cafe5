#ifndef DLGREPORTS_H
#define DLGREPORTS_H

#include "c5dialog.h"

namespace Ui {
class DlgReports;
}

class C5User;

class DlgReports : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgReports(const QStringList &dbParams);

    ~DlgReports();

    static void openReports(C5User *user);

private:
    Ui::DlgReports *ui;

    C5User *fUser;

    void getDailyCommon(const QDate &date1 = QDate::currentDate(), const QDate &date2 = QDate::currentDate());

private slots:
    void handleDailyCommon(const QJsonObject &obj);

    void handleReportsList(const QJsonObject &obj);

    void handlePrintReport(const QJsonObject &obj);

    void on_btnRefresh_clicked();

    void on_btnDateLeft1_clicked();

    void on_btnDateLeft2_clicked();

    void on_btnDateRight1_clicked();

    void on_btnDateRight2_clicked();

    void on_btnExit_clicked();

    void on_btnReports_clicked();
};

#endif // DLGREPORTS_H

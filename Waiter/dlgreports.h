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

protected:
    virtual void showEvent(QShowEvent *e) override;

private:
    Ui::DlgReports* ui;

    int fCurrentHall;

    QDate mDate1;

    QDate mDate2;

    void getDailyCommon();

    void setLangIcon();

private slots:

    void handleDailyCommon(const QJsonObject &obj);

    void on_btnRefresh_clicked();

    void on_btnExit_clicked();

    void on_btnReports_clicked();

    void on_btnParams_clicked();

    void on_btnOpenReport_clicked();
};

#endif // DLGREPORTS_H

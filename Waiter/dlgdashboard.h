#pragma once

#include "c5waiterdialog.h"
#include <QJsonObject>

namespace Ui
{
class DlgDashboard;
}

class DlgDashboard : public C5WaiterDialog
{
    Q_OBJECT
public:
    explicit DlgDashboard(const QJsonObject &jcashbox, C5User *user);

    ~DlgDashboard();

    int exec() override;

protected:
    virtual void showEvent(QShowEvent *e) override;

private slots:
    void on_btnOpenCashbox_clicked();

    void on_btnCloseCashbox_clicked();

    void on_btnGoHall_clicked();

    void on_btnOrders_clicked();

    void on_btnClose_clicked();

    void on_btnReports_clicked();

private:
    Ui::DlgDashboard* ui;

    QJsonObject mCashboxData;

    void setup();

    int mRoute = 0;
};

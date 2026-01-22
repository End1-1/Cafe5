#pragma once

#include "c5waiterdialog.h"
#include <QJsonObject>

namespace Ui
{
class DlgRealReports;
}

class QListWidgetItem;

class C5Printing;

class DlgRealReports : public C5WaiterDialog
{
    Q_OBJECT
public:
    explicit DlgRealReports(C5User *user);

    ~DlgRealReports();

protected:
    virtual void showEvent(QShowEvent *e) override;

private slots:
    void on_btnBack_clicked();

    void on_lst_itemClicked(QListWidgetItem *item);

    void on_btnPrint_clicked();

    void on_btnParams_clicked();

    void on_btnReload_clicked();

private:
    Ui::DlgRealReports* ui;

    C5Printing* mPrinting;

    QJsonArray mLastReport;

    QJsonObject mLastParams;

    QListWidgetItem* mLastClickedItem = nullptr;

    void loadReport(QListWidgetItem *item);
};

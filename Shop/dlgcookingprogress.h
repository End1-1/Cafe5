#ifndef DLGCOOKINGPROGRESS_H
#define DLGCOOKINGPROGRESS_H

#include "c5shopdialog.h"
#include <QJsonArray>

namespace Ui
{
class DlgCookingProgress;
}

class C5User;

class DlgCookingProgress : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit DlgCookingProgress(C5User *user, QWidget *parent = nullptr);
    ~DlgCookingProgress() override;

private slots:
    void on_btnRefresh_clicked();
    void on_btnReady_clicked();
    void on_btnServed_clicked();
    void on_btnClose_clicked();

private:
    Ui::DlgCookingProgress *ui;
    QJsonArray mOrders;

    void reload();
    void fillTable();
    void setSelectedLinesStatus(int status);
    QStringList selectedLineIds() const;
};

#endif // DLGCOOKINGPROGRESS_H

#pragma once

#include <QDialog>

class C5User;
class QJsonObject;

namespace Ui
{
class DlgCashDiscrepancy;
}

/** Shortage / overage from shift open or close: mark resolved and leave a comment. */
class DlgCashDiscrepancy : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCashDiscrepancy(C5User *user, int id, QWidget *parent = nullptr);
    ~DlgCashDiscrepancy() override;

private slots:
    void loadData();
    void trySave();

private:
    void applyRow(const QJsonObject &row);

    Ui::DlgCashDiscrepancy *ui = nullptr;
    C5User *mUser = nullptr;
    int mId = 0;
};

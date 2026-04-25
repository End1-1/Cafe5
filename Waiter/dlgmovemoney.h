#pragma once

#include "c5dialog.h"

namespace Ui
{
class DlgMoveMoney;
}

class DlgMoveMoney : public C5Dialog
{
    Q_OBJECT
public:
    explicit DlgMoveMoney(C5User *user);

    ~DlgMoveMoney() override;

    void setMode(bool isCashIn);

    double amount() const;

    QString comment() const;

private slots:
    void on_btnAmountKbd_clicked();

    void on_btnCommentKbd_clicked();

    void on_btnSave_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgMoveMoney *ui;
};

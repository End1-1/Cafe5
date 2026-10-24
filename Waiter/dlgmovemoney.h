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

    void setAmount(double value);

    void setMode(int mode);

    void setExpenseTypes(bool enabled);

    void setCashboxName(const QString &name);

    double amount() const;

    int operationType() const;

    QString comment() const;

private slots:
    void on_btnAmountKbd_clicked();

    void on_btnCommentKbd_clicked();

    void on_btnSave_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgMoveMoney *ui;
    bool mShowExpenseTypes = false;
    int mMode = 0;
};

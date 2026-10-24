#pragma once

#include "c5dialog.h"

namespace Ui {
class DlgOrderMore;
}

class DlgOrderMore : public C5Dialog
{
    Q_OBJECT

public:
    enum Choice { None = 0, Service = 1, Discount = 2 };

    explicit DlgOrderMore(C5User *user, QWidget *parent = nullptr);
    ~DlgOrderMore();

    void setOptions(const QString &serviceText, bool serviceEnabled,
                    const QString &discountText, bool discountEnabled);

    Choice choice() const { return mChoice; }

private slots:
    void on_btnMoreService_clicked();
    void on_btnMoreDiscount_clicked();

private:
    Ui::DlgOrderMore *ui;
    Choice mChoice = None;
};

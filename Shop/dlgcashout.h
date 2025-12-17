#ifndef DLGCASHOUT_H
#define DLGCASHOUT_H

#include "c5shopdialog.h"

namespace Ui
{
class DlgCashout;
}

class DlgCashout : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit DlgCashout(C5User *user);

    ~DlgCashout();

private slots:

    void on_btnCancel_clicked();

    void on_btnSave_clicked();

    void responseOfCreate(const QJsonObject &jdoc);

private:
    Ui::DlgCashout* ui;
};

#endif // DLGCASHOUT_H

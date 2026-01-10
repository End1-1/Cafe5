#ifndef DLGSELECTCURRENCY_H
#define DLGSELECTCURRENCY_H

#include "c5dialog.h"

namespace Ui
{
class DlgSelectCurrency;
}

class DlgSelectCurrency : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSelectCurrency(C5User *user);

    ~DlgSelectCurrency();

    static bool getCurrency(C5User *user, int& id, QString &name);

private slots:
    void on_btnReject_clicked();

    void on_btnOK_clicked();

private:
    Ui::DlgSelectCurrency* ui;
};

#endif // DLGSELECTCURRENCY_H

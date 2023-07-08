#ifndef DLGCASHBUTTONOPIONS_H
#define DLGCASHBUTTONOPIONS_H

#include "c5dialog.h"

namespace Ui {
class DlgCashButtonOpions;
}

class DlgCashButtonOpions : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgCashButtonOpions(const QStringList &dbParams);
    ~DlgCashButtonOpions();
    static int getOptions();

private slots:
    void on_btnCancel_clicked();

    void on_btnPrintReport_clicked();

    void on_btnCloseCash_clicked();

private:
    Ui::DlgCashButtonOpions *ui;
};

#endif // DLGCASHBUTTONOPIONS_H

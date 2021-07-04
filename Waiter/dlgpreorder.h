#ifndef DLGPREORDER_H
#define DLGPREORDER_H

#include "c5dialog.h"

class C5OrderDriver;

namespace Ui {
class DlgPreorder;
}

class DlgPreorder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPreorder(C5OrderDriver *w, const QStringList &dbParams);

    ~DlgPreorder();

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

    void on_btnDatePopup_clicked();

    void on_btnTimeUp_clicked();

    void on_btnTimeDown_clicked();

    void on_btnDateUp_clicked();

    void on_btnDateDown_clicked();

    void on_btnGuestUp_clicked();

    void on_btnGuestDown_clicked();

    void on_btnEditCash_clicked();

    void on_btnEditCard_clicked();

private:
    Ui::DlgPreorder *ui;

    C5OrderDriver *fOrder;
};

#endif // DLGPREORDER_H

#ifndef DLGTOOLS_H
#define DLGTOOLS_H

#include "c5dialog.h"

namespace Ui {
class DlgTools;
}

class C5WaiterOrderDoc;
class DlgOrder;

class DlgTools : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgTools(C5WaiterOrderDoc *order, DlgOrder *dlg, const QStringList &dbParams);

    ~DlgTools();

private slots:
    void on_btnSetPreorder_clicked();

private:
    Ui::DlgTools *ui;

    C5WaiterOrderDoc *fOrder;

    DlgOrder *fDlg;
};

#endif // DLGTOOLS_H

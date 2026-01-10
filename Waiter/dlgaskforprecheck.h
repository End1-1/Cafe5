#ifndef DLGASKFORPRECHECK_H
#define DLGASKFORPRECHECK_H

#include "c5dialog.h"

namespace Ui {
class DlgAskForPrecheck;
}

class DlgAskForPrecheck : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgAskForPrecheck(C5User *user);

    ~DlgAskForPrecheck();
    
    static int get(C5User *user);

private slots:
    void on_btnPrintPrecheck_clicked();

    void on_btnDoNotPrintPrecheck_clicked();

private:
    Ui::DlgAskForPrecheck *ui;
};

#endif // DLGASKFORPRECHECK_H

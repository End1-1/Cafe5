#ifndef DLGPRECHECKOPTIONS_H
#define DLGPRECHECKOPTIONS_H

#include "c5dialog.h"

#define PRECHECK_NONE 0
#define PRECHECK_REPEAT 10
#define PRECHECK_CANCEL 20

namespace Ui {
class DlgPrecheckOptions;
}

class DlgPrecheckOptions : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPrecheckOptions(C5User *user);

    ~DlgPrecheckOptions();
    
    static int precheck(C5User *user);

private slots:
    void on_btnCancel_clicked();

    void on_btnRepeatPrecheck_clicked();

    void on_btnCancelPrecheck_clicked();

private:
    Ui::DlgPrecheckOptions *ui;
};

#endif // DLGPRECHECKOPTIONS_H

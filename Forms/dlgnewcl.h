#ifndef DLGNEWCL_H
#define DLGNEWCL_H

#include "c5dialog.h"

namespace Ui
{
class DlgNewCL;
}

class DlgNewCL : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgNewCL(C5User *user);
    ~DlgNewCL();
    static bool createCL(C5User *user, QString &code, QString &name);

private slots:
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::DlgNewCL* ui;
};

#endif // DLGNEWCL_H

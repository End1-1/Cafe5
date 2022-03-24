#ifndef DLGNEWCL_H
#define DLGNEWCL_H

#include "c5dialog.h"

namespace Ui {
class DlgNewCL;
}

class DlgNewCL : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgNewCL(const QStringList &dbParams);
    ~DlgNewCL();
    static bool createCL(const QStringList &dbParams, QString &code, QString &name);

private slots:
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::DlgNewCL *ui;
};

#endif // DLGNEWCL_H

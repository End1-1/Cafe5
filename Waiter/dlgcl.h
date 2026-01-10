#ifndef DLGCL_H
#define DLGCL_H

#include "c5dialog.h"

namespace Ui
{
class DlgCL;
}

class DlgCL : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgCL(C5User *user);

    ~DlgCL();

    static bool getCL(C5User *user, QString &id, QString &name);

private:
    Ui::DlgCL* ui;

    int fRow;

private slots:
    void kbdAccept();

    void searchCL(const QString &arg);

    void on_tblGuest_doubleClicked(const QModelIndex &index);
};

#endif // DLGCL_H

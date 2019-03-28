#ifndef DLGCL_H
#define DLGCL_H

#include "c5dialog.h"

namespace Ui {
class DlgCL;
}

class DlgCL : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgCL(const QStringList &dbParams);

    ~DlgCL();

    static bool getCL(QString &id, QString &name);

private:
    Ui::DlgCL *ui;

    int fRow;

private slots:
    void kbdAccept();

    void searchCL(const QString &arg);

    void on_tblGuest_doubleClicked(const QModelIndex &index);
};

#endif // DLGCL_H

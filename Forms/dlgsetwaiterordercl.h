#ifndef DLGSETWAITERORDERCL_H
#define DLGSETWAITERORDERCL_H

#include "c5dialog.h"

namespace Ui {
class DlgSetWaiterOrderCL;
}

class DlgSetWaiterOrderCL : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSetWaiterOrderCL(const QStringList &dbParams);
    ~DlgSetWaiterOrderCL();
    static bool getCL(const QStringList &dbParams, QString &code, QString &name);

private slots:
    void on_btnNew_clicked();

    void on_leSearch_textChanged(const QString &arg1);

    void on_tbl_doubleClicked(const QModelIndex &index);

private:
    Ui::DlgSetWaiterOrderCL *ui;
};

#endif // DLGSETWAITERORDERCL_H

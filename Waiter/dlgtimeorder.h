#ifndef DLGTIMEORDER_H
#define DLGTIMEORDER_H

#include "c5dialog.h"

namespace Ui {
class DlgTimeorder;
}

class DlgTimeorder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgTimeorder();

    ~DlgTimeorder();

    static bool timeOrder(int &order);

private slots:
    void on_btnCancel_clicked();

    void on_btnTime1_clicked();

    void on_btnTime2_clicked();

    void on_btnTime3_clicked();

private:
    Ui::DlgTimeorder *ui;

    int fOrder;
};

#endif // DLGTIMEORDER_H

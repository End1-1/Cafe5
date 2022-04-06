#ifndef DLGQTY_H
#define DLGQTY_H

#include "c5dialog.h"

namespace Ui {
class DlgQty;
}

class DlgQty : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgQty();

    ~DlgQty();

    static bool getQty(double &qty, const QString &name);

private slots:
    void on_btnClear_clicked();

    void on_btn1_clicked();

    void on_btn2_clicked();

    void on_btn3_clicked();

    void on_btn4_clicked();

    void on_btn5_clicked();

    void on_btn6_clicked();

    void on_btn7_clicked();

    void on_btn8_clicked();

    void on_btn9_clicked();

    void on_btnDec_clicked();

    void on_btn0_clicked();

    void on_btn05_clicked();

    void on_btn15_clicked();

    void on_btn033_clicked();

    void on_btn025_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

private:
    Ui::DlgQty *ui;

    void click(const QString &c);
};

#endif // DLGQTY_H

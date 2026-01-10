#ifndef TOUCHENTERTAXRECEIPTNUMBER_H
#define TOUCHENTERTAXRECEIPTNUMBER_H

#include "c5dialog.h"

namespace Ui {
class TouchEnterTaxReceiptNumber;
}

class TouchEnterTaxReceiptNumber : public C5Dialog
{
    Q_OBJECT

public:
    explicit TouchEnterTaxReceiptNumber(C5User *user);
    ~TouchEnterTaxReceiptNumber();
    static bool getTaxReceiptNumber(QString &number, C5User *user);

private slots:
    void on_btnBackspace_clicked();
    void on_btnOk_clicked();
    void on_btn1_clicked();
    void on_btn0_clicked();
    void on_btn2_clicked();
    void on_btn3_clicked();
    void on_btn4_clicked();
    void on_btn5_clicked();
    void on_btn6_clicked();
    void on_btn7_clicked();
    void on_btn8_clicked();
    void on_btn9_clicked();

    void on_btnCancel_clicked();

private:
    Ui::TouchEnterTaxReceiptNumber *ui;
    void buttonClicked(QPushButton *b);
};

#endif // TOUCHENTERTAXRECEIPTNUMBER_H
